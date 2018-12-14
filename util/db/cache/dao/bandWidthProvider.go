package dao

import (
	"pnt/db/model"
	"pnt/db/cache/funcs"
	"github.com/Sirupsen/logrus"
	"pnt/utils"
	"math/rand"
)

//BandWidthProviderImpl bandwidth provider interface
type BandWidthProviderImpl struct {
	cache		*funcs.LocalCache
	log			*logrus.Logger
}

//NewBandWidthProviderImpl new basic node impl
func NewBandWidthProviderImpl(l *logrus.Logger, f *funcs.LocalCache) *BandWidthProviderImpl {
	return &BandWidthProviderImpl {
		cache: f,
		log: l,
	}
}

//CreateTable to create table
func (b *BandWidthProviderImpl) CreateTable(mo model.DBInterface) {
	bandwidthP := mo.(*model.BandWidthProvider)
	b.cache.CacheTable(bandwidthP.ModelName())
	b.log.Infoln("create table ", bandwidthP.ModelName())
}

//AddItem add item, overall handle
func (b *BandWidthProviderImpl) AddItem(mo model.DBInterface) {
	//bandwidth is needed
	bandwidthP := mo.(*model.BandWidthProvider)
	if bandwidthP.NodeList.Len() == 0 {
	}
	b.cache.CacheTable(bandwidthP.ModelName()).NotFoundAdd(SELFNODE, TIMEINTERVAL, bandwidthP)
	b.log.Infof("add bandwidth provider item %s success", SELFNODE)
}

//UpdateItem update item: del first, then add item, overall handle
func (b *BandWidthProviderImpl) UpdateItem(mo model.DBInterface) error {
	bandwidthP := mo.(*model.BandWidthProvider)
	_, err := b.cache.CacheTable(bandwidthP.ModelName()).Delete(SELFNODE); if err != nil {
		b.log.Errorf("update bandwidth provider item %s error: %s", SELFNODE, err.Error())
		return err
	}
	b.AddItem(bandwidthP)
	b.log.Infof("update provider item %s success", SELFNODE)
	return nil
}

//GetItem get items
func (b *BandWidthProviderImpl) GetItem(nodeID ...string) (mb map[string]*model.BasicNode) {
	mb = make(map[string]*model.BasicNode)
	if len(nodeID) == 0 {
		l, err := b.cache.CacheTable(new(model.BandWidthProvider).ModelName()).Value(SELFNODE); if err != nil {
			b.log.Errorln("provider get item error: ", err.Error())
			return mb
		}
		for _, r := range l.Data().([]string) {
			bp, err := b.cache.CacheTable(new(model.BasicNode).ModelName()).Value(r); if err != nil {
				b.log.Errorln("provider get basicnode item error: ", err.Error())
				continue
			}
			mb[r] = bp.Data().(*model.BasicNode)
		}	
	}else {
		for _, id := range nodeID {
			bn, err := b.cache.CacheTable(new(model.BasicNode).ModelName()).Value(id); if err != nil {
				b.log.Errorln("one provider get basicnode item error: ", err.Error())
			}
			mb[id] = bn.Data().(*model.BasicNode)
		}
	}
	b.log.Debugln("Get provider items: ", mb)
	return
}

//DelItem delete item
func (b *BandWidthProviderImpl) DelItem(nodeID string) error {
	rl, err := b.cache.CacheTable(new(model.BandWidthProvider).ModelName()).Value(SELFNODE); if err != nil {
		b.log.Errorln("<DelProviderItem>:::provider get item error: ", err.Error())	
		return err
	}
	if ok, e := utils.ListContains(rl.Data().(model.BandWidthProvider).NodeList, nodeID); ok {
		rl.Data().(model.BandWidthProvider).NodeList.Remove(e)
	}
	return nil
}

//AddOneBandProvider add provider into provider list
func (b *BandWidthProviderImpl) AddOneBandProvider(bn *model.BasicNode) error {
	bi := model.NewBandwidthItem(bn)
	rl, err := b.cache.CacheTable(new(model.BandWidthProvider).ModelName()).Value(SELFNODE);
	if err != nil {
		if err == funcs.ErrKeyNotFound {
			b.AddItem(model.NewBandWidthProvider())
			rl, err = b.cache.CacheTable(new(model.BandWidthProvider).ModelName()).Value(SELFNODE)
		}else {
			b.log.Errorln("<AddOneBandProvider>:::provider get item: ", err.Error())	
			return err
		}
	}
	utils.ListInsert(rl.Data().(*model.BandWidthProvider).NodeList, bi)
	b.log.Infof("add provider %s success.", bi.GetID())
	return nil
}

//GetOneSuitableProvider return a most suitable provider
//Front 50% randam
func (b *BandWidthProviderImpl) GetOneSuitableProvider() map[string]*model.BasicNode {
	rl, err := b.cache.CacheTable(new(model.BandWidthProvider).ModelName()).Value(SELFNODE); if err != nil {
		b.log.Errorln("<GetOneSuitableProvider>:::provider get item error: ", err.Error())	
		return nil
	}

	l := rl.Data().(*model.BandWidthProvider).NodeList 
	x := rand.Intn(l.Len())
	i := 0
	for e := l.Front(); e != nil; e = e.Next() {
		if i == x {
			return b.GetItem(e.Value.(*model.BandWidthItem).GetID())
		}
		i++
	}
	return nil
}






