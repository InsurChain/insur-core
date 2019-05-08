package cache

import (
	"log"
	"github.com/Sirupsen/logrus"
	"conf"
	"db/cache/funcs"
	"db/dao"
	cacheDao "db/cache/dao"
)

//Manager cache manager
type Manager struct {
	conf	*conf.Config
	log		*logrus.Logger
	cache   *funcs.LocalCache
}

//CreateCacheManager create cache manager
func CreateCacheManager(c *conf.Config) (*Manager, error) {
	m := new(Manager)
	m.conf = c
	m.log = log.GetLogHandler()
	m.cache = funcs.GetCacheInstance()
	m.log.Infoln("create cache manager success.")
	return m, nil
}

//LogManager log manager
func (m *Manager) LogManager() *logrus.Logger {
	return m.log
}

//CacheManager cache manager 
func (m *Manager) CacheManager() *funcs.LocalCache {
	return m.cache
}

//BasicNodeDao BasicNodeDao
func (m *Manager) BasicNodeDao() dao.BasicNodeDao {
	return cacheDao.NewBasicNodeImpl(m.log, m.cache)
}
