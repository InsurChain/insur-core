package dao

import (
	"pnt/db/model"
)

//Dao Dao
type Dao interface {
	CreateTable(model.DBInterface) 
	AddItem(model.DBInterface)
	UpdateItem(model.DBInterface) error
	DelItem(nodeID string) error
	GetItem(nodeID ...string) map[string]*model.BasicNode
}

//BasicNodeDao BasicNodeInfoDao
type BasicNodeDao interface {
	Dao
	//deal self node, search key "selfnode"
	AddSelfItem(model.DBInterface)
	GetSelfItem() (*model.BasicNode, error)
	UpdateSelfItem(model.DBInterface) error
}

