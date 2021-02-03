/*
** Zabbix
** Copyright (C) 2001-2021 Zabbix SIA
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

package mysql

import (
	"context"
	"database/sql"
	"sync"
	"time"

	"github.com/go-sql-driver/mysql"

	"zabbix.com/pkg/uri"

	"zabbix.com/pkg/log"
	"zabbix.com/pkg/zbxerr"
)

type MyClient interface {
	Query(ctx context.Context, query string, args ...interface{}) (rows *sql.Rows, err error)
	QueryRow(ctx context.Context, query string, args ...interface{}) (row *sql.Row, err error)
}

type MyConn struct {
	client         *sql.DB
	lastTimeAccess time.Time
}

func (conn *MyConn) Query(ctx context.Context, query string, args ...interface{}) (rows *sql.Rows, err error) {
	rows, err = conn.client.QueryContext(ctx, query, args...)

	if ctxErr := ctx.Err(); ctxErr != nil {
		err = ctxErr
	}

	return
}

// Query wraps DB.QueryRowContext.
func (conn *MyConn) QueryRow(ctx context.Context, query string, args ...interface{}) (row *sql.Row, err error) {
	row = conn.client.QueryRowContext(ctx, query, args...)

	if ctxErr := ctx.Err(); ctxErr != nil {
		err = ctxErr
	}

	return
}

// updateAccessTime updates the last time a connection was accessed.
func (conn *MyConn) updateAccessTime() {
	conn.lastTimeAccess = time.Now()
}

// ConnManager is thread-safe structure for manage connections.
type ConnManager struct {
	sync.Mutex
	connMutex      sync.Mutex
	connections    map[uri.URI]*MyConn
	keepAlive      time.Duration
	connectTimeout time.Duration
	callTimeout    time.Duration
	Destroy        context.CancelFunc
}

// NewConnManager initializes connManager structure and runs Go Routine that watches for unused connections.
func NewConnManager(keepAlive, connectTimeout, callTimeout,
	hkInterval time.Duration) *ConnManager {
	ctx, cancel := context.WithCancel(context.Background())

	connMgr := &ConnManager{
		connections:    make(map[uri.URI]*MyConn),
		keepAlive:      keepAlive,
		connectTimeout: connectTimeout,
		callTimeout:    callTimeout,
		Destroy:        cancel, // Destroy stops originated goroutines and closes connections.
	}

	go connMgr.housekeeper(ctx, hkInterval)

	return connMgr
}

// closeUnused closes each connection that has not been accessed at least within the keepalive interval.
func (c *ConnManager) closeUnused() {
	c.connMutex.Lock()
	defer c.connMutex.Unlock()

	for uri, conn := range c.connections {
		if time.Since(conn.lastTimeAccess) > c.keepAlive {
			conn.client.Close()
			delete(c.connections, uri)
			log.Debugf("[%s] Closed unused connection: %s", pluginName, uri.Addr())
		}
	}
}

// closeAll closes all existed connections.
func (c *ConnManager) closeAll() {
	c.connMutex.Lock()
	for uri, conn := range c.connections {
		conn.client.Close()
		delete(c.connections, uri)
	}
	c.connMutex.Unlock()
}

// housekeeper repeatedly checks for unused connections and closes them.
func (c *ConnManager) housekeeper(ctx context.Context, interval time.Duration) {
	ticker := time.NewTicker(interval)

	for {
		select {
		case <-ctx.Done():
			ticker.Stop()
			c.closeAll()

			return
		case <-ticker.C:
			c.closeUnused()
		}
	}
}

// create creates a new connection with given credentials.
func (c *ConnManager) create(uri uri.URI) (*MyConn, error) {
	c.connMutex.Lock()
	defer c.connMutex.Unlock()

	if _, ok := c.connections[uri]; ok {
		// Should never happen.
		panic("connection already exists")
	}

	config := mysql.NewConfig()
	config.User = uri.User()
	config.Passwd = uri.Password()
	config.Net = uri.Scheme()
	config.Addr = uri.Addr()
	config.Timeout = c.connectTimeout
	config.ReadTimeout = c.callTimeout
	config.InterpolateParams = true

	connector, err := mysql.NewConnector(config)

	if err != nil {
		return nil, err
	}

	client := sql.OpenDB(connector)

	c.connections[uri] = &MyConn{
		client:         client,
		lastTimeAccess: time.Now(),
	}

	log.Debugf("[%s] Created new connection: %s", pluginName, uri.Addr())

	return c.connections[uri], nil
}

// get returns a connection with given uri if it exists and also updates lastTimeAccess, otherwise returns nil.
func (c *ConnManager) get(uri uri.URI) *MyConn {
	c.connMutex.Lock()
	defer c.connMutex.Unlock()

	if conn, ok := c.connections[uri]; ok {
		conn.updateAccessTime()
		return conn
	}

	return nil
}

// GetConnection returns an existing connection or creates a new one.
func (c *ConnManager) GetConnection(uri uri.URI) (conn *MyConn, err error) {
	c.Lock()
	defer c.Unlock()

	conn = c.get(uri)

	if conn == nil {
		conn, err = c.create(uri)
	}

	if err != nil {
		err = zbxerr.ErrorConnectionFailed.Wrap(err)
	}

	return
}
