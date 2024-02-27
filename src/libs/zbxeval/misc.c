/*
** Zabbix
** Copyright (C) 2001-2024 Zabbix SIA
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**/

#include "common.h"
#include "log.h"

#include "zbxalgo.h"
#include "zbxvariant.h"
#include "zbxserialize.h"
#include "eval.h"

#define ZBX_EVAL_STATIC_BUFFER_SIZE	4096

/******************************************************************************
 *                                                                            *
 * Purpose: reserve number of bytes in the specified buffer, reallocating if  *
 *          necessary                                                         *
 *                                                                            *
 * Parameters: buffer      - [IN/OUT] the buffer                              *
 *             buffer_size - [INT/OUT] the deserialized value                 *
 *             reserve     - [IN] the number of bytes to reserve              *
 *             ptr         - [IN/OUT] a pointer to an offset in buffer        *
 *                                                                            *
 * Comments: Initially static buffer is used, allocating dynamic buffer when  *
 *           static buffer is too small.                                      *
 *                                                                            *
 ******************************************************************************/
static	void	reserve_buffer(unsigned char **buffer, size_t *buffer_size, size_t reserve, unsigned char **ptr)
{
	size_t		offset = *ptr - *buffer, new_size;

	if (offset + reserve <= *buffer_size)
		return;

	new_size = *buffer_size * 1.5;

	if (ZBX_EVAL_STATIC_BUFFER_SIZE == *buffer_size)
	{
		unsigned char	*old = *buffer;

		*buffer = zbx_malloc(NULL, new_size);
		memcpy(*buffer, old, offset);
	}
	else
		*buffer = zbx_realloc(*buffer, new_size);

	*buffer_size = new_size;
	*ptr = *buffer + offset;
}

static void	serialize_variant(unsigned char **buffer, size_t *size, const zbx_variant_t *value,
		unsigned char **ptr)
{
	size_t		len;

	reserve_buffer(buffer, size, 1, ptr);
	**ptr = value->type;
	(*ptr)++;

	switch (value->type)
	{
		case ZBX_VARIANT_UI64:
			reserve_buffer(buffer, size, sizeof(value->data.ui64), ptr);
			*ptr += zbx_serialize_uint64(*ptr, value->data.ui64);
			break;
		case ZBX_VARIANT_DBL:
			reserve_buffer(buffer, size, sizeof(value->data.dbl), ptr);
			*ptr += zbx_serialize_double(*ptr, value->data.dbl);
			break;
		case ZBX_VARIANT_STR:
			len = strlen(value->data.str) + 1;
			reserve_buffer(buffer, size, len, ptr);
			memcpy(*ptr, value->data.str, len);
			*ptr += len;
			break;
		case ZBX_VARIANT_NONE:
			break;
		default:
			zabbix_log(LOG_LEVEL_DEBUG, "TYPE: %d", value->type);
			THIS_SHOULD_NEVER_HAPPEN;
			(*ptr)[-1] = ZBX_VARIANT_NONE;
			break;
	}
}

static zbx_uint32_t	deserialize_variant(const unsigned char *ptr,  zbx_variant_t *value)
{
	const unsigned char	*start = ptr;
	unsigned char		type;
	zbx_uint64_t		ui64;
	double			dbl;
	char			*str;
	size_t			len;

	ptr += zbx_deserialize_char(ptr, &type);

	switch (type)
	{
		case ZBX_VARIANT_UI64:
			ptr += zbx_deserialize_uint64(ptr, &ui64);
			zbx_variant_set_ui64(value, ui64);
			break;
		case ZBX_VARIANT_DBL:
			ptr += zbx_deserialize_double(ptr, &dbl);
			zbx_variant_set_dbl(value, dbl);
			break;
		case ZBX_VARIANT_STR:
			len = strlen((const char *)ptr) + 1;
			str = zbx_malloc(NULL, len);
			memcpy(str, ptr, len);
			zbx_variant_set_str(value, str);
			ptr += len;
			break;
		case ZBX_VARIANT_NONE:
			zbx_variant_set_none(value);
			break;
		default:
			THIS_SHOULD_NEVER_HAPPEN;
			zbx_variant_set_none(value);
			break;
	}

	return ptr - start;
}

/******************************************************************************
 *                                                                            *
 * Purpose: serialize evaluation context into buffer                          *
 *                                                                            *
 * Parameters: ctx         - [IN] the evaluation context                      *
 *             malloc_func - [IN] the buffer memory allocation function,      *
 *                                optional (by default the buffer is          *
 *                                allocated in heap)                          *
 *             data  - [OUT] the buffer with serialized evaluation context    *
 *                                                                            *
 * Comments: Location of the replaced tokens (with token.value set) are not   *
 *           serialized, making it impossible to reconstruct the expression   *
 *           text with replaced tokens.                                       *
 *           Context serialization/deserialization must be used for           *
 *           context caching.                                                 *
 *                                                                            *
 * Return value: The size of serialized data.                                 *
 *                                                                            *
 ******************************************************************************/
size_t	zbx_eval_serialize(const zbx_eval_context_t *ctx, zbx_mem_malloc_func_t malloc_func,
		unsigned char **data)
{
	int		i;
	unsigned char	buffer_static[ZBX_EVAL_STATIC_BUFFER_SIZE], *buffer = buffer_static, *ptr = buffer, len_buff[6];
	size_t		buffer_size = ZBX_EVAL_STATIC_BUFFER_SIZE;
	zbx_uint32_t	len, len_offset;

	if (NULL == malloc_func)
		malloc_func = ZBX_DEFAULT_MEM_MALLOC_FUNC;

	ptr += zbx_serialize_uint31_compact(ptr, ctx->stack.values_num);

	for (i = 0; i < ctx->stack.values_num; i++)
	{
		const zbx_eval_token_t	*token = &ctx->stack.values[i];

		/* reserve space for maximum possible worst case scenario with empty variant:         */
		/*  4 bytes token type, 6 bytes per compact uint31 and 1 byte empty variant (4+3*6+1) */
		reserve_buffer(&buffer, &buffer_size, 23, &ptr);

		ptr += zbx_serialize_value(ptr, token->type);
		ptr += zbx_serialize_uint31_compact(ptr, token->opt);
		ptr += zbx_serialize_uint31_compact(ptr, token->loc.l);
		ptr += zbx_serialize_uint31_compact(ptr, token->loc.r);

		serialize_variant(&buffer, &buffer_size, &token->value, &ptr);
	}

	len = ptr - buffer;

	len_offset = zbx_serialize_uint31_compact(len_buff, len);

	*data = malloc_func(NULL, len + len_offset);
	memcpy(*data, len_buff, len_offset);
	memcpy(*data + len_offset, buffer, len);

	if (buffer != buffer_static)
		zbx_free(buffer);

	return len + len_offset;
}

/******************************************************************************
 *                                                                            *
 * Purpose: deserialize evaluation context from buffer                        *
 *                                                                            *
 * Parameters: ctx        - [OUT] the evaluation context                      *
 *             expression - [IN] the expression the evaluation context was    *
 *                               created from                                 *
 *             rules      - [IN] the composition and evaluation rules         *
 *             data       - [IN] the buffer with serialized context           *
 *                                                                            *
 ******************************************************************************/
void	zbx_eval_deserialize(zbx_eval_context_t *ctx, const char *expression, zbx_uint64_t rules,
		const unsigned char *data)
{
	zbx_uint32_t	i, tokens_num, len, pos;

	memset(ctx, 0, sizeof(zbx_eval_context_t));
	ctx->expression = expression;
	ctx->rules = rules;

	data += zbx_deserialize_uint31_compact(data, &len);
	data += zbx_deserialize_uint31_compact(data, &tokens_num);
	zbx_vector_eval_token_create(&ctx->stack);
	zbx_vector_eval_token_reserve(&ctx->stack, tokens_num);
	ctx->stack.values_num = tokens_num;

	for (i = 0; i < tokens_num; i++)
	{
		zbx_eval_token_t	*token = &ctx->stack.values[i];

		data += zbx_deserialize_value(data, &token->type);
		data += zbx_deserialize_uint31_compact(data, &token->opt);

		data += zbx_deserialize_uint31_compact(data, &pos);
		token->loc.l = pos;
		data += zbx_deserialize_uint31_compact(data, &pos);
		token->loc.r = pos;

		data += deserialize_variant(data, &token->value);
	}
}

static int	compare_tokens_by_loc(const void *d1, const void *d2)
{
	const zbx_eval_token_t	*t1 = *(const zbx_eval_token_t * const *)d1;
	const zbx_eval_token_t	*t2 = *(const zbx_eval_token_t * const *)d2;

	ZBX_RETURN_IF_NOT_EQUAL(t1->loc.l, t2->loc.l);
	return 0;
}

static const zbx_eval_token_t	*eval_get_next_function_token(const zbx_eval_context_t *ctx, int token_index)
{
	if (0 != (ctx->stack.values[token_index].type & ZBX_EVAL_CLASS_FUNCTION))
		return NULL;

	for(int i = token_index + 1; i < ctx->stack.values_num; i++)
	{
		const zbx_eval_token_t	*token = &ctx->stack.values[i];
		if (0 != (token->type & ZBX_EVAL_CLASS_FUNCTION))
		{
			if (token->opt < (zbx_uint32_t)(i - token_index))
				return NULL;

			return &ctx->stack.values[i];
		}
	}

	return NULL;
}

/******************************************************************************
 *                                                                            *
 * Purpose: print token into string quoting/escaping if necessary             *
 *                                                                            *
 * Parameters: ctx        - [IN] the evaluation context                       *
 *             str        - [IN/OUT] the output buffer                        *
 *             str_alloc  - [IN/OUT] the output buffer size                   *
 *             str_offset - [IN/OUT] the output buffer offset                 *
 *             token      - [IN] the token to print                           *
 *                                                                            *
 ******************************************************************************/
static void	eval_token_print_alloc(const zbx_eval_context_t *ctx, char **str, size_t *str_alloc,
	size_t *str_offset, const zbx_eval_token_t *token)
{
	int			quoted = 0, check_value = 0;
	const char		*value_str;
	const zbx_eval_token_t	*func_token;

	if (ZBX_VARIANT_NONE == token->value.type)
		return;

	if (ZBX_VARIANT_ERR == token->value.type)
	{
		if (0 == (ctx->rules & ZBX_EVAL_COMPOSE_MASK_ERROR))
			zbx_snprintf_alloc(str, str_alloc, str_offset, "ERROR(%s)", token->value.data.err);
		else
			zbx_strcpy_alloc(str, str_alloc, str_offset, "*ERROR*");
		return;
	}

	switch (token->type)
	{
		case ZBX_EVAL_TOKEN_FUNCTIONID:
			if (0 != (ctx->rules & ZBX_EVAL_COMPOSE_FUNCTIONID))
			{
				zbx_variant_t	functionid;

				zbx_variant_copy(&functionid, &token->value);

				if (SUCCEED == zbx_variant_convert(&functionid, ZBX_VARIANT_UI64))
				{
					zbx_snprintf_alloc(str, str_alloc, str_offset, "{" ZBX_FS_UI64 "}",
							functionid.data.ui64);
					return;
				}
				zbx_variant_clear(&functionid);
			}
			break;
		case ZBX_EVAL_TOKEN_VAR_STR:
			quoted = 1;
			break;
		case ZBX_EVAL_TOKEN_VAR_MACRO:
			if (0 == (ctx->rules & ZBX_EVAL_COMPOSE_LLD))
				check_value = 1;
			break;
		case ZBX_EVAL_TOKEN_VAR_USERMACRO:
			if (0 != (ctx->rules & ZBX_EVAL_COMPOSE_QUOTE))
				quoted = 1;
			else if (0 == (ctx->rules & ZBX_EVAL_COMPOSE_LLD))
				check_value = 1;
			break;
		case ZBX_EVAL_TOKEN_VAR_LLDMACRO:
			if (0 != (ctx->rules & ZBX_EVAL_COMPOSE_QUOTE))
				quoted = 1;
			else if (0 != (ctx->rules & ZBX_EVAL_COMPOSE_LLD))
				check_value = 1;
			break;
	}

	if (0 != check_value)
	{
		if (ZBX_VARIANT_STR == token->value.type &&
				SUCCEED != zbx_eval_suffixed_number_parse(token->value.data.str, NULL))
		{
			quoted = 1;
		}
	}

	value_str = zbx_variant_value_desc(&token->value);

	if (0 == quoted)
		zbx_strcpy_alloc(str, str_alloc, str_offset, value_str);
	else
	{
		func_token = eval_get_next_function_token(ctx, (int)(token - ctx->stack.values));
		zbx_strquote_alloc_opt(str, str_alloc, str_offset, value_str,
				NULL != func_token && ZBX_EVAL_TOKEN_HIST_FUNCTION == func_token->type ?
				ZBX_STRQUOTE_SKIP_BACKSLASH : ZBX_STRQUOTE_DEFAULT);
	}
}

/******************************************************************************
 *                                                                            *
 * Purpose: compose expression by replacing processed tokens (with values) in *
 *          the original expression                                           *
 *                                                                            *
 * Parameters: ctx        - [IN] the evaluation context                       *
 *             expression - [OUT] the composed expression                     *
 *                                                                            *
 ******************************************************************************/
void	zbx_eval_compose_expression(const zbx_eval_context_t *ctx, char **expression)
{
	zbx_vector_ptr_t	tokens;
	const zbx_eval_token_t	*token;
	int			i;
	size_t			pos = 0, expression_alloc = 0, expression_offset = 0;

	/* Handle exceptions that are set when expression evaluation failed.     */
	/* Exception stack consists of two tokens - error message and exception. */
	if (2 == ctx->stack.values_num && ZBX_EVAL_TOKEN_EXCEPTION == ctx->stack.values[1].type)
	{
		zbx_strcpy_alloc(expression, &expression_alloc, &expression_offset, "throw(");
		eval_token_print_alloc(ctx, expression, &expression_alloc, &expression_offset, &ctx->stack.values[0]);
		zbx_chrcpy_alloc(expression, &expression_alloc, &expression_offset, ')');
		return;
	}

	zbx_vector_ptr_create(&tokens);

	for (i = 0; i < ctx->stack.values_num; i++)
	{
		if (ZBX_VARIANT_NONE != ctx->stack.values[i].value.type)
			zbx_vector_ptr_append(&tokens, &ctx->stack.values[i]);
	}

	zbx_vector_ptr_sort(&tokens, compare_tokens_by_loc);

	for (i = 0; i < tokens.values_num; i++)
	{
		token = (const zbx_eval_token_t *)tokens.values[i];

		if (0 != token->loc.l)
		{
			zbx_strncpy_alloc(expression, &expression_alloc, &expression_offset, ctx->expression + pos,
					token->loc.l - pos);
		}
		pos = token->loc.r + 1;
		eval_token_print_alloc(ctx, expression, &expression_alloc, &expression_offset, token);
	}

	if ('\0' != ctx->expression[pos])
		zbx_strcpy_alloc(expression, &expression_alloc, &expression_offset, ctx->expression + pos);

	zbx_vector_ptr_destroy(&tokens);
}

/******************************************************************************
 *                                                                            *
 * Purpose: check if string has possible user macro                           *
 *                                                                            *
 * Parameters: str - [IN] the string to check                                 *
 *             len - [IN] the string length                                   *
 *                                                                            *
 * Return value: SUCCEED - the string might contain a user macro              *
 *               FAIL    - otherwise                                          *
 *                                                                            *
 ******************************************************************************/
static int	eval_has_usermacro(const char *str, size_t len)
{
	const char	*ptr;

	if (4 > len)
		return FAIL;

	/* stop earlier to account for at least one character macro name and terminating '}' */
	for (ptr = str; ptr < str + len - 3; )
	{
		if ('{' == *ptr++)
		{
			if ('$' == *ptr)
				return SUCCEED;
			ptr++;
		}
	}

	return FAIL;
}

/******************************************************************************
 *                                                                            *
 * Purpose: expand user macros in item query                                  *
 *                                                                            *
 * Parameters: itemquery   - [IN] the evaluation context                      *
 *             len         - [IN] the item query length                       *
 *             hostids     - [IN] the linked hostids                          *
 *             hostids_num - [IN] the number of linked hostids                *
 *             resolver_cb - [IN] the resolver callback                       *
 *             out         - [OUT] the item query with expanded macros        *
 *             error       - [OUT] the error message, optional. If specified  *
 *                                 the function will return failure at the    *
 *                                 first failed macro expansion               *
 *                                                                            *
 * Return value: SUCCEED - the macros were expanded successfully              *
 *               FAIL    - error parameter was given and at least one of      *
 *                         macros was not expanded                            *
 *                                                                            *
 ******************************************************************************/
static int	eval_query_expand_user_macros(const char *itemquery, size_t len, zbx_uint64_t *hostids, int hostids_num,
		zbx_macro_resolve_func_t resolver_cb, char **out, char **error)
{
	zbx_eval_context_t	ctx;
	zbx_item_query_t	query;
	int			i, ret = SUCCEED;
	char			*errmsg = NULL, *filter = NULL;

	if (len != zbx_eval_parse_query(itemquery, len, &query))
	{
		if (NULL != error)
		{
			*error = zbx_strdup(NULL, "cannot parse item query");
			return FAIL;
		}
		return SUCCEED;
	}

	if (NULL == query.filter)
		goto out;

	if (SUCCEED != zbx_eval_parse_expression(&ctx, query.filter,
			ZBX_EVAL_PARSE_QUERY_EXPRESSION | ZBX_EVAL_COMPOSE_QUOTE, &errmsg))
	{
		if (NULL != error)
		{
			ret = FAIL;
			*error = zbx_dsprintf(NULL, "cannot parse item query filter: %s", errmsg);
		}

		zbx_free(errmsg);
		goto out;
	}

	for (i = 0; i < ctx.stack.values_num; i++)
	{
		zbx_eval_token_t	*token = &ctx.stack.values[i];
		char			*value, *tmp;

		switch (token->type)
		{
			case ZBX_EVAL_TOKEN_VAR_USERMACRO:
				ret = resolver_cb(ctx.expression + token->loc.l, token->loc.r - token->loc.l + 1,
						hostids, hostids_num, &value, error);
				break;
			case ZBX_EVAL_TOKEN_VAR_STR:
				if (SUCCEED != eval_has_usermacro(ctx.expression + token->loc.l,
						token->loc.r - token->loc.l + 1))
				{
					continue;
				}
				tmp = zbx_substr_unquote(ctx.expression, token->loc.l, token->loc.r);
				ret = resolver_cb(tmp, strlen(tmp), hostids, hostids_num, &value, error);
				zbx_free(tmp);
				break;
			default:
				continue;
		}

		if (SUCCEED != ret)
		{
			zbx_eval_clear(&ctx);
			goto out;
		}

		zbx_variant_set_str(&token->value, value);
	}

	zbx_eval_compose_expression(&ctx, &filter);
	zbx_eval_clear(&ctx);

	*out = zbx_dsprintf(NULL, "/%s/%s?[%s]", ZBX_NULL2EMPTY_STR(query.host), query.key, filter);

out:
	zbx_free(filter);
	zbx_eval_clear_query(&query);

	return ret;
}

/******************************************************************************
 *                                                                            *
 * Purpose: expand user macros in parsed expression                           *
 *                                                                            *
 * Parameters: ctx         - [IN] the evaluation context                      *
 *             hostids     - [IN] the linked hostids                          *
 *             hostids_num - [IN] the number of linked hostids                *
 *             resolver_cb - [IN] the resolver callback                       *
 *             error       - [OUT] the error message, optional. If specified  *
 *                                 the function will return failure at the    *
 *                                 first failed macro expansion               *
 *                                                                            *
 * Return value: SUCCEED - the macros were expanded successfully              *
 *               FAIL    - error parameter was given and at least one of      *
 *                         macros was not expanded                            *
 *                                                                            *
 ******************************************************************************/
int	zbx_eval_expand_user_macros(const zbx_eval_context_t *ctx, zbx_uint64_t *hostids, int hostids_num,
		zbx_macro_resolve_func_t resolver_cb, char **error)
{
	int	i, ret = SUCCEED;

	for (i = 0; i < ctx->stack.values_num; i++)
	{
		zbx_eval_token_t	*token = &ctx->stack.values[i];
		char			*value = NULL, *tmp;

		/* workaround is needed for calculated items for history functions */
		if (ZBX_VARIANT_STR == token->value.type)
		{
			if (SUCCEED != resolver_cb(token->value.data.str, strlen(token->value.data.str), hostids,
				hostids_num, &value, error))
			{
				return FAIL;
			}

			zbx_variant_clear(&token->value);
			zbx_variant_set_str(&token->value, value);

			continue;
		}

		switch (token->type)
		{
			case ZBX_EVAL_TOKEN_VAR_USERMACRO:
				ret = resolver_cb(ctx->expression + token->loc.l, token->loc.r - token->loc.l + 1,
						hostids, hostids_num, &value, error);
				break;
			case ZBX_EVAL_TOKEN_VAR_STR:
			case ZBX_EVAL_TOKEN_VAR_NUM:
			case ZBX_EVAL_TOKEN_ARG_PERIOD:
				if (SUCCEED != eval_has_usermacro(ctx->expression + token->loc.l,
						token->loc.r - token->loc.l + 1))
				{
					continue;
				}
				tmp = zbx_substr_unquote(ctx->expression, token->loc.l, token->loc.r);
				ret = resolver_cb(tmp, strlen(tmp), hostids, hostids_num, &value, error);
				zbx_free(tmp);
				break;
			case ZBX_EVAL_TOKEN_ARG_QUERY:
				if (SUCCEED != eval_has_usermacro(ctx->expression + token->loc.l,
						token->loc.r - token->loc.l + 1))
				{
					continue;
				}
				ret = eval_query_expand_user_macros(ctx->expression + token->loc.l,
						token->loc.r - token->loc.l + 1, hostids, hostids_num, resolver_cb,
						&value, error);
				break;
			default:
				continue;
		}

		if (SUCCEED != ret)
			return FAIL;

		if (NULL != value)
			zbx_variant_set_str(&token->value, value);
	}

	return SUCCEED;
}

/******************************************************************************
 *                                                                            *
 * Purpose: set eval context to exception that will be returned when executed *
 *                                                                            *
 * Parameters: ctx     - [IN] the evaluation context                          *
 *             message - [IN] the exception message (the memory is owned by   *
 *                       context)                                             *
 *                                                                            *
 ******************************************************************************/
void	zbx_eval_set_exception(zbx_eval_context_t *ctx, char *message)
{
	zbx_eval_token_t	*token;

	memset(ctx, 0, sizeof(zbx_eval_context_t));
	zbx_vector_eval_token_create(&ctx->stack);
	zbx_vector_eval_token_reserve(&ctx->stack, 2);
	ctx->stack.values_num = 2;

	token = ctx->stack.values;
	memset(token, 0, 2 * sizeof(zbx_eval_token_t));
	token->type = ZBX_EVAL_TOKEN_VAR_STR;
	zbx_variant_set_str(&token->value, message);
	(++token)->type = ZBX_EVAL_TOKEN_EXCEPTION;
}

/******************************************************************************
 *                                                                            *
 * Purpose: extract functionid from token                                     *
 *                                                                            *
 * Parameters: expression - [IN] the original expression                      *
 *             token      - [IN] the token                                    *
 *             functionid - [OUT] the extracted functionid                    *
 *                                                                            *
 * Return value: SUCCEED - functionid was extracted successfully              *
 *               FAIL    - otherwise (incorrect token or invalid data)        *
 *                                                                            *
 * Comment: The extracted functionid will be cached as token value, so the    *
 *          next time it can be used without extracting the value from        *
 *          expression.                                                       *
 *                                                                            *
 ******************************************************************************/
static int	expression_extract_functionid(const char *expression, zbx_eval_token_t *token, zbx_uint64_t *functionid)
{
	if (ZBX_EVAL_TOKEN_FUNCTIONID != token->type)
		return FAIL;

	switch (token->value.type)
	{
		case ZBX_VARIANT_UI64:
			*functionid = token->value.data.ui64;
			return SUCCEED;
		case ZBX_VARIANT_NONE:
			if (SUCCEED != is_uint64_n(expression + token->loc.l + 1, token->loc.r - token->loc.l - 1,
					functionid))
			{
				THIS_SHOULD_NEVER_HAPPEN;
				break;
			}
			zbx_variant_set_ui64(&token->value, *functionid);
			return SUCCEED;
	}

	return FAIL;
}

/******************************************************************************
 *                                                                            *
 * Purpose: deserialize expression and extract specified tokens into values   *
 *                                                                            *
 * Parameters: data       - [IN] the serialized expression                    *
 *             expression - [IN] the original expression                      *
 *             mask       - [IN] the tokens to extract                        *
 *                                                                            *
 * Return value: Expression evaluation context.                               *
 *                                                                            *
 ******************************************************************************/
zbx_eval_context_t	*zbx_eval_deserialize_dyn(const unsigned char *data, const char *expression,
		zbx_uint64_t mask)
{
	zbx_eval_context_t	*ctx;
	int			i;
	zbx_uint64_t		functionid;
	char			*value;

	ctx = (zbx_eval_context_t *)zbx_malloc(NULL, sizeof(zbx_eval_context_t));
	zbx_eval_deserialize(ctx, expression, ZBX_EVAL_TRIGGER_EXPRESSION, data);

	for (i = 0; i < ctx->stack.values_num; i++)
	{
		zbx_eval_token_t	*token = &ctx->stack.values[i];

		switch (token->type)
		{
			case ZBX_EVAL_TOKEN_FUNCTIONID:
				if (0 == (mask & ZBX_EVAL_EXTRACT_FUNCTIONID))
					continue;
				expression_extract_functionid(expression, token, &functionid);
				break;
			case ZBX_EVAL_TOKEN_VAR_STR:
				if (0 != (mask & ZBX_EVAL_EXTRACT_VAR_STR) && ZBX_VARIANT_NONE == token->value.type)
				{
					/* extract string variable value for macro resolving */
					value = zbx_substr_unquote(expression, token->loc.l, token->loc.r);
					zbx_variant_set_str(&token->value, value);
				}
				break;
			case ZBX_EVAL_TOKEN_VAR_MACRO:
				if (0 != (mask & ZBX_EVAL_EXTRACT_VAR_MACRO) && ZBX_VARIANT_NONE == token->value.type)
				{
					/* extract macro for resolving */
					value = zbx_substr_unquote(expression, token->loc.l, token->loc.r);
					zbx_variant_set_str(&token->value, value);
				}
				break;
		}
	}

	return ctx;
}

/******************************************************************************
 *                                                                            *
 * Purpose: get functionids from parsed expression                            *
 *                                                                            *
 * Parameters: ctx         - [IN] the evaluation context                      *
 *             functionids - [OUT] the extracted functionids                  *
 *                                                                            *
 ******************************************************************************/
void	zbx_eval_get_functionids(zbx_eval_context_t *ctx, zbx_vector_uint64_t *functionids)
{
	int	i;

	for (i = 0; i < ctx->stack.values_num; i++)
	{
		zbx_eval_token_t	*token = &ctx->stack.values[i];
		zbx_uint64_t		functionid;

		if (SUCCEED == expression_extract_functionid(ctx->expression, token, &functionid))
			zbx_vector_uint64_append(functionids, functionid);
	}
}

/******************************************************************************
 *                                                                            *
 * Purpose: get functionids from parsed expression in the order they are      *
 *          written                                                           *
 *                                                                            *
 * Parameters: ctx         - [IN] the evaluation context                      *
 *             functionids - [OUT] the extracted functionids                  *
 *                                                                            *
 ******************************************************************************/
void	zbx_eval_get_functionids_ordered(zbx_eval_context_t *ctx, zbx_vector_uint64_t *functionids)
{
	int			i;
	zbx_vector_ptr_t	tokens;

	zbx_vector_ptr_create(&tokens);

	for (i = 0; i < ctx->stack.values_num; i++)
	{
		if (ZBX_EVAL_TOKEN_FUNCTIONID == ctx->stack.values[i].type)
			zbx_vector_ptr_append(&tokens, &ctx->stack.values[i]);
	}

	zbx_vector_ptr_sort(&tokens, compare_tokens_by_loc);

	for (i = 0; i < tokens.values_num; i++)
	{
		zbx_eval_token_t	*token = (zbx_eval_token_t *)tokens.values[i];
		zbx_uint64_t		functionid;

		if (SUCCEED == expression_extract_functionid(ctx->expression, token, &functionid))
			zbx_vector_uint64_append(functionids, functionid);
	}

	zbx_vector_ptr_destroy(&tokens);
}

/******************************************************************************
 *                                                                            *
 * Purpose: check if expression contains timer function calls (date, time,    *
 *          now, dayofweek, dayofmonth)                                       *
 *                                                                            *
 * Parameters: ctx - [IN] the evaluation context                              *
 *                                                                            *
 * Return value: SUCCEED - expression contains timer function call(s)         *
 *               FAIL    - otherwise                                          *
 *                                                                            *
 ******************************************************************************/
int	zbx_eval_check_timer_functions(const zbx_eval_context_t *ctx)
{
	int	i;

	for (i = 0; i < ctx->stack.values_num; i++)
	{
		zbx_eval_token_t	*token = &ctx->stack.values[i];

		if (ZBX_EVAL_TOKEN_FUNCTION != token->type)
			continue;

		if (SUCCEED == eval_compare_token(ctx, &token->loc, "date", ZBX_CONST_STRLEN("date")))
			return SUCCEED;
		if (SUCCEED == eval_compare_token(ctx, &token->loc, "time", ZBX_CONST_STRLEN("time")))
			return SUCCEED;
		if (SUCCEED == eval_compare_token(ctx, &token->loc, "now", ZBX_CONST_STRLEN("now")))
			return SUCCEED;
		if (SUCCEED == eval_compare_token(ctx, &token->loc, "dayofmonth", ZBX_CONST_STRLEN("dayofmonth")))
			return SUCCEED;
		if (SUCCEED == eval_compare_token(ctx, &token->loc, "dayofweek", ZBX_CONST_STRLEN("dayofweek")))
			return SUCCEED;
	}

	return FAIL;
}

/******************************************************************************
 *                                                                            *
 * Purpose: extract functionids from serialized expression                    *
 *                                                                            *
 * Parameters: expression  - [IN] the original expression                     *
 *             data        - [IN] the serialized expression                   *
 *             functionids - [OUT] the extracted functionids                  *
 *                                                                            *
 ******************************************************************************/
void	zbx_get_serialized_expression_functionids(const char *expression, const unsigned char *data,
	zbx_vector_uint64_t *functionids)
{
	zbx_uint32_t		i, tokens_num, len, loc_l, loc_r, opt;
	zbx_token_type_t	type;
	zbx_uint64_t		functionid;
	unsigned char		var_type;

	data += zbx_deserialize_uint31_compact(data, &len);
	data += zbx_deserialize_uint31_compact(data, &tokens_num);

	for (i = 0; i < tokens_num; i++)
	{
		data += zbx_deserialize_value(data, &type);
		data += zbx_deserialize_uint31_compact(data, &opt);
		data += zbx_deserialize_uint31_compact(data, &loc_l);
		data += zbx_deserialize_uint31_compact(data, &loc_r);

		data += zbx_deserialize_char(data, &var_type);

		switch (var_type)
		{
			case ZBX_VARIANT_UI64:
				data += sizeof(zbx_uint64_t);
				break;
			case ZBX_VARIANT_DBL:
				data += sizeof(double);
				break;
			case ZBX_VARIANT_STR:
				data += strlen((const char *)data) + 1;
				break;
			case ZBX_VARIANT_NONE:
				break;
			default:
				THIS_SHOULD_NEVER_HAPPEN;
				return;
		}

		if (ZBX_EVAL_TOKEN_FUNCTIONID == type)
		{
			if (SUCCEED == is_uint64_n(expression + loc_l + 1, loc_r - loc_l - 1, &functionid))
				zbx_vector_uint64_append(functionids, functionid);
			else
				THIS_SHOULD_NEVER_HAPPEN;
		}
	}

	zbx_vector_uint64_sort(functionids, ZBX_DEFAULT_UINT64_COMPARE_FUNC);
	zbx_vector_uint64_uniq(functionids, ZBX_DEFAULT_UINT64_COMPARE_FUNC);
}

/******************************************************************************
 *                                                                            *
 * Purpose: the Nth constant in expression                                    *
 *                                                                            *
 * Parameters: ctx   - [IN] the evaluation context                            *
 *             index - [IN] the constant index                                *
 *             value - [OUT] the constant value                               *
 *                                                                            *
 ******************************************************************************/
void	zbx_eval_get_constant(const zbx_eval_context_t *ctx, int index, char **value)
{
	int	i;

	for (i = 0; i < ctx->stack.values_num; i++)
	{
		zbx_eval_token_t	*token = &ctx->stack.values[i];

		switch (token->type)
		{
			case ZBX_EVAL_TOKEN_VAR_STR:
			case ZBX_EVAL_TOKEN_VAR_NUM:
			case ZBX_EVAL_TOKEN_VAR_USERMACRO:
				if (index == (int)token->opt + 1)
				{
					zbx_free(*value);
					if (ZBX_VARIANT_NONE != token->value.type)
						*value = zbx_strdup(NULL, zbx_variant_value_desc(&token->value));
					else
						*value = zbx_substr_unquote(ctx->expression, token->loc.l, token->loc.r);
					return;
				}
				break;
		}
	}
}

/******************************************************************************
 *                                                                            *
 * Purpose: replace functionid in parsed expression with new functionid macro *
 *                                                                            *
 * Parameters: ctx            - [IN] the evaluation context                   *
 *             old_functionid - [IN] the constant index                       *
 *             new_functionid - [OUT] the constant value                      *
 *                                                                            *
 ******************************************************************************/
void	zbx_eval_replace_functionid(zbx_eval_context_t *ctx, zbx_uint64_t old_functionid, zbx_uint64_t new_functionid)
{
	int	i;

	for (i = 0; i < ctx->stack.values_num; i++)
	{
		zbx_eval_token_t	*token = &ctx->stack.values[i];
		zbx_uint64_t		token_functionid;

		if (ZBX_EVAL_TOKEN_FUNCTIONID != token->type)
			continue;

		if (ZBX_VARIANT_UI64 != token->value.type)
		{
			if (ZBX_VARIANT_NONE != token->value.type)
				continue;

			if (SUCCEED != is_uint64_n(ctx->expression + token->loc.l + 1, token->loc.r - token->loc.l - 1,
					&token_functionid))
			{
				THIS_SHOULD_NEVER_HAPPEN;
				continue;
			}
			zbx_variant_set_ui64(&token->value, token_functionid);
		}

		if (token->value.data.ui64 == old_functionid)
		{
			zbx_variant_set_ui64(&token->value, new_functionid);

			/* mark functionid as replaced to check if any non-replaced functionids are left */
			token->opt = ZBX_MAX_UINT31_1;
		}
	}
}

/******************************************************************************
 *                                                                            *
 * Purpose: validate parsed expression to check if all functionids were       *
 *          replaced                                                          *
 *                                                                            *
 * Parameters: ctx     [IN] the evaluation context                            *
 *             error - [IN] the error message                                 *
 *                                                                            *
 ******************************************************************************/
int	zbx_eval_validate_replaced_functionids(zbx_eval_context_t *ctx, char **error)
{
	int	i;

	for (i = 0; i < ctx->stack.values_num; i++)
	{
		zbx_eval_token_t	*token = &ctx->stack.values[i];

		if (ZBX_EVAL_TOKEN_FUNCTIONID != token->type)
			continue;

		if (ZBX_MAX_UINT31_1 != token->opt)
		{
			*error = zbx_dsprintf(*error, "non-updated functionid found at \"%s\"",
					ctx->expression + token->loc.l);
			return FAIL;
		}
	}

	return SUCCEED;
}

/******************************************************************************
 *                                                                            *
 * Purpose: copy parsed expression                                            *
 *                                                                            *
 * Parameters: dst        - [OUT] the destination evaluation context          *
 *             src        - [IN] the source evaluation context                *
 *             expression - [IN] copied destination expression                *
 *                                                                            *
 ******************************************************************************/
void	zbx_eval_copy(zbx_eval_context_t *dst, const zbx_eval_context_t *src, const char *expression)
{
	int	i;

	dst->expression = expression;
	dst->rules = src->rules;
	zbx_vector_eval_token_create(&dst->stack);
	zbx_vector_eval_token_reserve(&dst->stack, src->stack.values_num);

	zbx_vector_eval_token_append_array(&dst->stack, src->stack.values, src->stack.values_num);
	for (i = 0; i < dst->stack.values_num; i++)
	{
		if (ZBX_VARIANT_NONE != src->stack.values[i].value.type)
			zbx_variant_copy(&dst->stack.values[i].value, &src->stack.values[i].value);
	}
}

/******************************************************************************
 *                                                                            *
 * Purpose: format function evaluation error message                          *
 *                                                                            *
 * Parameters: function  - [IN] the function name                             *
 *             host      - [IN] the host name, can be NULL                    *
 *             key       - [IN] the item key, can be NULL                     *
 *             parameter - [IN] the function parameters list                  *
 *             error     - [IN] the error message                             *
 *                                                                            *
 * Return value: The formatted error message.                                 *
 *                                                                            *
 ******************************************************************************/
char	*zbx_eval_format_function_error(const char *function, const char *host, const char *key,
		const char *parameter, const char *error)
{
	char	*msg = NULL;
	size_t	msg_alloc = 0, msg_offset = 0;

	zbx_snprintf_alloc(&msg, &msg_alloc, &msg_offset, "Cannot evaluate function %s(/%s/%s",
			function, (NULL != host ? host : "?"), (NULL != key ? key : "?"));

	if (NULL != parameter && '\0' != *parameter)
		zbx_snprintf_alloc(&msg, &msg_alloc, &msg_offset, ",%s", parameter);

	zbx_chrcpy_alloc(&msg, &msg_alloc, &msg_offset, ')');

	if (NULL != error && '\0' != *error)
		zbx_snprintf_alloc(&msg, &msg_alloc, &msg_offset, ": %s", error);

	zbx_chrcpy_alloc(&msg, &msg_alloc, &msg_offset, '.');

	return msg;
}

/******************************************************************************
 *                                                                            *
 * Purpose: copy history query into vector and replace it with vector index   *
 *                                                                            *
 * Parameters: ctx  - [IN] the evaluation context                             *
 *             refs - [OUT] the item references                               *
 *                                                                            *
 ******************************************************************************/
void	zbx_eval_extract_item_refs(zbx_eval_context_t *ctx, zbx_vector_str_t *refs)
{
	int	i;

	for (i = 0; i < ctx->stack.values_num; i++)
	{
		zbx_eval_token_t	*token = &ctx->stack.values[i];

		if (ZBX_EVAL_TOKEN_ARG_QUERY != token->type)
			continue;

		if (ZBX_VARIANT_STR == token->value.type)
		{
			zbx_vector_str_append(refs, token->value.data.str);
			zbx_variant_set_none(&token->value);
		}
		else
			zbx_vector_str_append(refs, zbx_substr(ctx->expression, token->loc.l, token->loc.r));

		zbx_variant_clear(&token->value);
		zbx_variant_set_ui64(&token->value, refs->values_num - 1);
	}
}
