package dao

import (
	"github.com/Sirupsen/logrus"
	"github.com/errors"
	"pnt/db/cache/funcs"
	"pnt/db/model"
	"pnt/utils"
)

//RepeaterNodeImpl repeater node interface
type RepeaterNodeImpl struct {
	cache *funcs.LocalCache
	log   *logrus.Logger
}

//NewRepeaterNodeImpl new repeater node impl
func NewRepeaterNodeImpl(l *logrus.Logger, f *funcs.LocalCache) *RepeaterNodeImpl {
	return &RepeaterNodeImpl{
		cache: f,
		log:   l,
	}
}

//CreateTable to create table
func (b *RepeaterNodeImpl) CreateTable(mo model.DBInterface) {
	repeaters := mo.(*model.Repeaters)
	b.cache.CacheTable(repeaters.ModelName())
	b.log.Infoln("create table ", repeaters.ModelName())
}

//AddItem add item, overall handle
func (b *RepeaterNodeImpl) AddItem(mo model.DBInterface) {
	repeaters := mo.(*model.Repeaters)
	b.cache.CacheTable(repeaters.ModelName()).NotFoundAdd(SELFNODE, TIMEINTERVAL, repeaters)
	b.log.Infof("add repeaters item %s success", SELFNODE)
}

//UpdateItem update item: del first, then add item, overall handle
func (b *RepeaterNodeImpl) UpdateItem(mo model.DBInterface) error {
	repeaters := mo.(*model.Repeaters)
	_, err := b.cache.CacheTable(repeaters.ModelName()).Delete(SELFNODE)
	if err != nil {
		b.log.Errorf("update repeater item %s error: %s", SELFNODE, err.Error())
		return err
	}
	b.AddItem(repeaters)
	b.log.Infof("update repeater item %s success", SELFNODE)
	return nil
}

//GetItem get items
func (b *RepeaterNodeImpl) GetItem(nodeID ...string) (mb map[string]*model.BasicNode) {
	mb = make(map[string]*model.BasicNode)

	if len(nodeID) == 0 {
		rList, err := b.cache.CacheTable(new(model.Repeaters).ModelName()).Value(SELFNODE)
		if err != nil {
			b.log.Errorln("repeater get item error: ", err.Error())
			return
		}
		for e := rList.Data().(*model.Repeaters).NodeList.Front(); e != nil; e = e.Next() {
			bp, err := b.cache.CacheTable(new(model.BasicNode).ModelName()).Value(e.Value)
			if err != nil {
				b.log.Errorln("repeater get basicnode item error: ", err.Error())
				continue
			}
			mb[bp.Data().(*model.BasicNode).NodeInfo.NodeID] = bp.Data().(*model.BasicNode)
		}
	}
	b.log.Debugln("Get repeater items: ", mb)
	return
}

//DelItem delete item
func (b *RepeaterNodeImpl) DelItem(nodeID string) error {
	rl, err := b.cache.CacheTable(new(model.Repeaters).ModelName()).Value(SELFNODE)
	if err != nil {
		b.log.Errorln("<DelRepeaterItem>:::repeater get item error: ", err.Error())
		return err
	}
	if ok, e := utils.ListContains(rl.Data().(model.Repeaters).NodeList, nodeID); ok {
		rl.Data().(model.Repeaters).NodeList.Remove(e)
	}
	return nil
}

//AddOneRepeater AddOneRepeater
func (b *RepeaterNodeImpl) AddOneRepeater(nodeID string) error {
	if nodeID == "" {
		return errors.New("nodeId is empty")
	}
	rl, err := b.cache.CacheTable(new(model.Repeaters).ModelName()).Value(SELFNODE)
	if err != nil {
		if err == funcs.ErrKeyNotFound {
			b.AddItem(model.NewRepeaters())
			rl, err = b.cache.CacheTable(new(model.Repeaters).ModelName()).Value(SELFNODE)
		} else {
			b.log.Errorln("<AddOneRepeater>:::repeater get item error: ", err.Error())
			return err
		}
	}
	// push back
	id := rl.Data().(*model.Repeaters).NodeList.PushBack(nodeID).Value.(string)
	b.log.Infof("<AddOneRepeater>:::add repeater %s success.", id)
	return nil
}

func (b *RepeaterNodeImpl) InitServerRepeater() error {
	b.CreateTable(new(model.Repeaters))
	if err := b.AddOneRepeater(SELFNODE); err != nil {
		return err
	}
	return nil
}
   
