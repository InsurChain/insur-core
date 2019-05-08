/*
 * Simple caching library with expiration capabilities
 *     Copyright (c) 2012, Radu Ioan Fericean
 *                   2013-2017, Christian Muehlhaeuser <muesli@gmail.com>
 *
 *   For license see LICENSE.txt
 */

package funcs

import (
	"sync"
)

//LocalCache local cache 
type LocalCache struct {
	Cache	map[string]*CacheTable
	sync.RWMutex
}

// CacheTable returns the existing cache table with given name or creates a new one
// if the table does not exist yet.
func (l *LocalCache) CacheTable (table string) *CacheTable {
	l.RLock()
	t, ok := l.Cache[table]
	l.RUnlock()
	if !ok {
		l.Lock()
		t, ok = l.Cache[table]
		if !ok {
			t = &CacheTable{
				name:  table,
				items: make(map[interface{}]*CacheItem),
			}
			l.Cache[table] = t
		}
		l.Unlock()
	}
	return t
}

var defaultLocalCache *LocalCache
var singleInstance sync.Mutex

//NewCacheManager NewCacheManager
func NewCacheManager() *LocalCache {
	l := new(LocalCache)
	l.Cache = make(map[string]*CacheTable)
	return l
}

//GetCacheInstance get cache single instance
func GetCacheInstance() *LocalCache {
	singleInstance.Lock()
	defer singleInstance.Unlock()
	if defaultLocalCache != nil {
		return defaultLocalCache
	}
	return NewCacheManager()
}
