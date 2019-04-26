package db

import (
	"pnt/conf"
	"pnt/db/cache"
	"pnt/db/dao"
	"sync"
)

//Manager Manager
type Manager interface {
	BasicNodeDao() dao.BasicNodeDao
	// dao.BasicNodeInfoDao
}

var defaultManager Manager 
var singleInstance sync.Mutex

//CreateManager create db manager
func CreateManager(c *conf.Config) error {
	var err error
	switch c.DBModel{
	case "store":
		fallthrough
	case "cache":
		fallthrough
	default:
		if defaultManager == nil {
			singleInstance.Lock()
			defer singleInstance.Unlock()
			defaultManager, err = cache.CreateCacheManager(c); if err != nil {
				return err
			}
		}
	}
	return nil
}

//GetManager get manager
func GetManager() Manager {
	return defaultManager
}




   
