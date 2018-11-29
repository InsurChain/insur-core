package dao

import (
	"time"
	"errors"
	"pnt/db/model"
	"pnt/db/cache/funcs"
	"github.com/Sirupsen/logrus"
)

const (
	//TIMEINTERVAL time interval, 0 means never expires.
	TIMEINTERVAL = 0 *time.Second
)

/*
	basic node impl 
*/

//BasicNodeImpl BasicNode interface
type BasicNodeImpl struct {
	cache		*funcs.LocalCache
	log			*logrus.Logger
}

//NewBasicNodeImpl new basic node impl
func NewBasicNodeImpl(l *logrus.Logger, f *funcs.LocalCache) *BasicNodeImpl {
	return &BasicNodeImpl {
		cache: f,
		log: l,
	}
}

//CreateTable to create table
func (b *BasicNodeImpl) CreateTable(mo model.DBInterface) {
	basicNode := mo.(*model.BasicNode)
	b.cache.CacheTable(basicNode.ModelName())
	b.log.Infoln("create table ", basicNode.ModelName())
}

//AddItem add item
func (b *BasicNodeImpl) AddItem(mo model.DBInterface) {
	basicNode := mo.(*model.BasicNode)
	b.cache.CacheTable(basicNode.ModelName()).NotFoundAdd(basicNode.NodeInfo.NodeID, TIMEINTERVAL, basicNode)
	b.log.Infof("add item %s success", basicNode.NodeInfo.NodeID)
}

//UpdateItem update item: del first, then add item
func (b *BasicNodeImpl) UpdateItem(mo model.DBInterface) error {
	basicNode := mo.(*model.BasicNode)
	_, err := b.cache.CacheTable(basicNode.ModelName()).Delete(basicNode.NodeInfo.NodeID); if err != nil {
		b.log.Errorf("update item %s error: %s", basicNode.NodeInfo.NodeID, err.Error())
		return err
	}
	b.AddItem(basicNode)
	b.log.Infof("update item %s success", basicNode.NodeInfo.NodeID)
	return nil
}

//GetItem get items
func (b *BasicNodeImpl) GetItem(nodeID ...string) (mb map[string]*model.BasicNode) {
    bbn := new(model.BasicNode)	
	if len(nodeID) == 0 {
		b.cache.CacheTable(bbn.ModelName()).Foreach(
			func(key interface{},item *funcs.CacheItem) {
				v, _ := item.Data().(*model.BasicNode)
				k, _ := key.(string)
				mb[k] = v
			})
		// delete myself
		delete(mb, "selfnode")
	}else {
		for _, id := range nodeID {
			v, err := b.cache.CacheTable(bbn.ModelName()).Value(id); if err != nil {
				b.log.Errorf("Get item %s error: %s", id, err.Error())
				continue
			}
			mb[id] = v.Data().(*model.BasicNode)
		}
	}
	b.log.Debugln("Get items: ", mb)
	return 
}

//DelItem delete item
func (b *BasicNodeImpl) DelItem(nodeID string) error {
	_, err := b.cache.CacheTable(new(model.BasicNode).ModelName()).Delete(nodeID); if err != nil {
		b.log.Errorf("Delete item %s error: %s", nodeID, err.Error())
		return err
	}
	return nil
}

//AddSelfItem add self item
func (b *BasicNodeImpl) AddSelfItem(mo model.DBInterface) {
	basicNode := mo.(*model.BasicNode)
	b.cache.CacheTable(basicNode.ModelName()).NotFoundAdd("selfnode", TIMEINTERVAL, basicNode)
	b.log.Infof("add self item %s success", basicNode.NodeInfo.NodeID)	
}

//GetSelfItem get self item
func (b *BasicNodeImpl) GetSelfItem() (*model.BasicNode, error) {
	mb := b.GetItem("selfnode")
	if len(mb) == 0 {
		return nil, errors.New("Not found any keys calls: selfnode")
	}
	return mb["selfnode"], nil
}

//UpdateSelfItem update self item
func (b *BasicNodeImpl) UpdateSelfItem(mo model.DBInterface) error {
	basicNode := mo.(*model.BasicNode)
	_, err := b.cache.CacheTable(basicNode.ModelName()).Delete("selfnode"); if err != nil {
		b.log.Errorf("update item %s error: %s", "selfnode", err.Error())
		return err
	}
	b.AddSelfItem(basicNode)
	b.log.Infof("update item %s success", "selfnode")
	return nil
}





