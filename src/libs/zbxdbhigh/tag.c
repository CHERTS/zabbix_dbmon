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

#include "../zbxalgo/vectorimpl.h"
#include "db.h"

ZBX_PTR_VECTOR_IMPL(db_tag_ptr, zbx_db_tag_t *)

zbx_db_tag_t	*zbx_db_tag_create(const char *tag_tag, const char *tag_value)
{
	zbx_db_tag_t	*tag;

	tag = (zbx_db_tag_t *)zbx_malloc(NULL, sizeof(zbx_db_tag_t));
	tag->tagid = 0;
	tag->flags = ZBX_FLAG_DB_TAG_UNSET;
	tag->tag = zbx_strdup(NULL, tag_tag);
	tag->value = zbx_strdup(NULL, tag_value);
	tag->tag_orig = NULL;
	tag->value_orig = NULL;

	return tag;
}

void	zbx_db_tag_free(zbx_db_tag_t *tag)
{
	if (0 != (tag->flags & ZBX_FLAG_DB_TAG_UPDATE_TAG))
		zbx_free(tag->tag_orig);

	if (0 != (tag->flags & ZBX_FLAG_DB_TAG_UPDATE_VALUE))
		zbx_free(tag->value_orig);

	zbx_free(tag->tag);
	zbx_free(tag->value);
	zbx_free(tag);
}

/******************************************************************************
 *                                                                            *
 * Purpose: roll back tag updates done during merge process                   *
 *                                                                            *
 * Return value: SUCCEED - updates were rolled back                           *
 *               FAIL    - new tag, rollback impossible                       *
 *                                                                            *
 ******************************************************************************/
static int	db_tag_rollback(zbx_db_tag_t *tag)
{
	if (0 == tag->tagid)
		return FAIL;

	if (0 != (tag->flags & ZBX_FLAG_DB_TAG_UPDATE_TAG))
	{
		zbx_free(tag->tag);
		tag->tag = tag->tag_orig;
		tag->tag_orig = NULL;
		tag->flags &= (~ZBX_FLAG_DB_TAG_UPDATE_TAG);
	}

	if (0 != (tag->flags & ZBX_FLAG_DB_TAG_UPDATE_VALUE))
	{
		zbx_free(tag->value);
		tag->value = tag->value_orig;
		tag->value_orig = NULL;
		tag->flags &= (~ZBX_FLAG_DB_TAG_UPDATE_VALUE);
	}

	return SUCCEED;
}

#define ZBX_TAG_OP(tag) (0 == tag->tagid ? "create" : "update")

typedef enum
{
	ZBX_DB_TAG_TAG,
	ZBX_DB_TAG_VALUE
}
zbx_db_tag_field_t;

/******************************************************************************
 *                                                                            *
 * Purpose: check validness of a single tag field (tag or value)              *
 *                                                                            *
 * Return value: SUCCEED - tag field is valid                                 *
 *               FAIL    - otherwise                                          *
 *                                                                            *
 ******************************************************************************/
static int	db_tag_check_field(const zbx_db_tag_t *tag, zbx_db_tag_field_t type, const char *owner, char **error)
{
	const char	*field, *str;
	size_t		field_len, str_len;

	switch (type)
	{
		case ZBX_DB_TAG_TAG:
			field = "tag";
			str = tag->tag;
			field_len = TAG_NAME_LEN;
			break;
		case ZBX_DB_TAG_VALUE:
			field = "value";
			str = tag->value;
			field_len = TAG_VALUE_LEN;
			break;
		default:
			THIS_SHOULD_NEVER_HAPPEN;

			if (NULL != error)
			{
				*error = zbx_strdcatf(*error, "Cannot %s %s tag: invalid field type.\n", ZBX_TAG_OP(tag),
						owner);
			}
			return FAIL;
	}

	if (SUCCEED != zbx_is_utf8(str))
	{
		if (NULL != error)
		{
			char	*ptr_utf8;

			ptr_utf8 = zbx_strdup(NULL, str);
			zbx_replace_invalid_utf8(ptr_utf8);
			*error = zbx_strdcatf(*error, "Cannot %s %s tag: %s \"%s\" has invalid UTF-8 sequence.\n",
					ZBX_TAG_OP(tag), owner, field, ptr_utf8);
			zbx_free(ptr_utf8);
		}

		return FAIL;
	}

	str_len = zbx_strlen_utf8(str);

	if (field_len < str_len)
	{
		if (NULL != error)
		{
			*error = zbx_strdcatf(*error, "Cannot %s %s tag: %s \"%128s...\" is too long.\n",
					ZBX_TAG_OP(tag), owner, field, str);
		}
		return FAIL;
	}

	return SUCCEED;
}

/******************************************************************************
 *                                                                            *
 * Purpose: check validness of all fields for a list of tags                  *
 *                                                                            *
 * Return value: SUCCEED - tags have valid fields                             *
 *               FAIL    - otherwise                                          *
 *                                                                            *
 ******************************************************************************/
static int	check_tag_fields(zbx_vector_db_tag_ptr_t *tags, const char *owner, char **error)
{
	int	i, ret = SUCCEED;

	for (i = 0; i < tags->values_num; i++)
	{
		zbx_db_tag_t	*tag = tags->values[i];
		int		errors = 0;

		if (0 != (tag->flags & ZBX_FLAG_DB_TAG_REMOVE))
			continue;

		if ('\0' == *tag->tag)
		{
			if (NULL != error)
			{
				*error = zbx_strdcatf(*error, "Cannot %s %s tag: empty tag name.\n", ZBX_TAG_OP(tag),
						owner);
			}
			errors += FAIL;
		}

		errors += db_tag_check_field(tag, ZBX_DB_TAG_TAG, owner, error);
		errors += db_tag_check_field(tag, ZBX_DB_TAG_VALUE, owner, error);

		if (0 > errors)
		{
			if (SUCCEED != db_tag_rollback(tag))
			{
				zbx_db_tag_free(tag);
				zbx_vector_db_tag_ptr_remove_noorder(tags, i--);
			}

			ret = FAIL;
		}
	}

	return ret;
}

/******************************************************************************
 *                                                                            *
 * Purpose: check new tags for duplicate tag+value combinations               *
 *                                                                            *
 * Parameters: tags  - [IN/OUT] the tags to check                             *
 *             owner - [IN] the owned object (host, item, trigger)            *
 *             error - [OUT] the error message                                *
 *                                                                            *
 * Return value: SUCCEED - tags have no duplicates                            *
 *               FAIL    - otherwise                                          *
 *                                                                            *
 * Comments: Existing tags are rolled back to their original values, while    *
 *           new tags are removed.                                            *
 *                                                                            *
 ******************************************************************************/
static int	check_duplicate_tags(zbx_vector_db_tag_ptr_t *tags, const char *owner, char **error)
{
	int	i, j, ret = SUCCEED;

	for (i = 0; i < tags->values_num; i++)
	{
		zbx_db_tag_t	*left = tags->values[i];

		for (j = 0; j < i; j++)
		{
			zbx_db_tag_t	*right = tags->values[j];

			if (0 == strcmp(left->tag, right->tag) && 0 == strcmp(left->value, right->value))
			{
				if (NULL != error)
				{
					*error = zbx_strdcatf(*error, "Cannot %s %s tag: \"%s: %s\" already exists.\n",
							ZBX_TAG_OP(left), owner, left->tag, right->value);
				}

				zbx_db_tag_free(left);
				zbx_vector_db_tag_ptr_remove_noorder(tags, i--);

				ret = FAIL;
				break;
			}
		}
	}

	return ret;
}

/******************************************************************************
 *                                                                            *
 * Purpose: merge new tags into existing                                      *
 *                                                                            *
 * Parameters: dst - [IN/OUT] vector of existing tags                         *
 *             src - [IN/OUT] vector or new tags                              *
 *             owner  - [IN] the tag owner (host, item, trigger),             *
 *                           optional - must be specified if error parameter  *
 *                           is not null                                      *
 *             error  - [IN,OUT] the error message (appended to existing),    *
 *                           optional                                         *
 *                                                                            *
 * Comments: The tags are merged using the following logic:                   *
 *           1) tags with matching name+value are left as it is               *
 *           2) tags with matching names will have their values updated       *
 *           3) tags without matches will have:                               *
 *              a) their name and value updated if there are new tags left    *
 *              b) flagged to be removed otherwise                            *
 *           4) all leftover new tags will be created                         *
 *                                                                            *
 * Return value: SUCCEED - tags were merged without issues                    *
 *               FAIL - tags were merged with errors                          *
 *                                                                            *
 ******************************************************************************/
int	zbx_merge_tags(zbx_vector_db_tag_ptr_t *dst, zbx_vector_db_tag_ptr_t *src, const char *owner, char **error)
{
	int	i, j, ret;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s() old_tags:%d new_tags:%d", __func__, dst->values_num, src->values_num);

	ret = check_duplicate_tags(src, owner, error);

	/* perform exact tag + value match */
	for (i = 0; i < dst->values_num; i++)
	{
		for (j = 0; j < src->values_num; j++)
		{
			if (0 == strcmp(dst->values[i]->tag, src->values[j]->tag) &&
					0 == strcmp(dst->values[i]->value, src->values[j]->value))
			{
				break;
			}
		}

		if (j != src->values_num)
		{
			zbx_db_tag_free(src->values[j]);
			zbx_vector_db_tag_ptr_remove_noorder(src, j);
			continue;
		}

		dst->values[i]->flags = ZBX_FLAG_DB_TAG_REMOVE;
	}

	if (0 == src->values_num)
		goto out;

	/* perform tag match */
	for (i = 0; i < dst->values_num; i++)
	{
		if (ZBX_FLAG_DB_TAG_REMOVE != dst->values[i]->flags)
			continue;

		for (j = 0; j < src->values_num; j++)
		{
			if (0 == strcmp(dst->values[i]->tag, src->values[j]->tag))
				break;
		}

		if (j != src->values_num)
		{
			dst->values[i]->value_orig = dst->values[i]->value;
			dst->values[i]->value = src->values[j]->value;
			dst->values[i]->flags = ZBX_FLAG_DB_TAG_UPDATE_VALUE;
			src->values[j]->value = NULL;
			zbx_db_tag_free(src->values[j]);
			zbx_vector_db_tag_ptr_remove_noorder(src, j);
			continue;
		}
	}

	if (0 == src->values_num)
		goto out;

	/* update rest of the tags */
	for (i = 0; i < dst->values_num && 0 < src->values_num; i++)
	{
		if (ZBX_FLAG_DB_TAG_REMOVE != dst->values[i]->flags)
			continue;

		dst->values[i]->tag_orig = dst->values[i]->tag;
		dst->values[i]->value_orig = dst->values[i]->value;
		dst->values[i]->tag = src->values[0]->tag;
		dst->values[i]->value = src->values[0]->value;
		dst->values[i]->flags = ZBX_FLAG_DB_TAG_UPDATE_TAG | ZBX_FLAG_DB_TAG_UPDATE_VALUE;
		src->values[0]->tag = NULL;
		src->values[0]->value = NULL;
		zbx_db_tag_free(src->values[0]);
		zbx_vector_db_tag_ptr_remove_noorder(src, 0);
	}

	/* add leftover new tags */
	zbx_vector_db_tag_ptr_append_array(dst, src->values, src->values_num);
	zbx_vector_db_tag_ptr_clear(src);
out:
	if (SUCCEED != check_tag_fields(dst, owner, error))
		ret = FAIL;

	zbx_vector_db_tag_ptr_sort(dst, ZBX_DEFAULT_UINT64_PTR_COMPARE_FUNC);

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s()  tags:%d", __func__, dst->values_num);

	return ret;
}
