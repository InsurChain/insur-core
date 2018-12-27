package policy

import (
	// "math/rand"
    // cache "pnt/db/cache"
	// "sort"
	// "time"
	"pnt/db/model"
	"pnt/db"
)

type TunnProtocol int

const (
	//PROXYFULL 全量节点
	PROXYFULL			= iota + 1
	//PROXYKEEP 会话保持
	PROXYKEEP
	//PROXYMIX 单会话混杂节点
	PROXYMIX 
)

//NodeSelected NodeSelected
type NodeSelected struct {
	strategy int
	protocol TunnProtocol
}

//NewSelected NewSelected
func NewSelected(strategy int, protocol TunnProtocol) *NodeSelected {
	return &NodeSelected{
		strategy: strategy,
		protocol: protocol,
	}
}

//Getstrategy return strategy 
func (n *NodeSelected) Getstrategy() int {
	return n.strategy
}

//GetSuitableNode get suitable nodes
func (n *NodeSelected) GetSuitableNode() map[string]*model.BasicNode {
	switch n.strategy {
	case PROXYFULL:
		return db.GetManager().BasicNodeDao().GetItem()
	case PROXYKEEP:
		return db.GetManager().BandWidthProviderDao().GetOneSuitableProvider()
	case PROXYMIX:
		return db.GetManager().BandWidthProviderDao().GetOneSuitableProvider()
	default:
		return db.GetManager().BandWidthProviderDao().GetOneSuitableProvider()
	}
}
