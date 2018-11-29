package model

import (
	// "pnt/conf"
)

//Handler resouce handler
type Handler interface {
	Add()
	Del()
	Put()
	Updata()
	Rush()
	CacheSort()
}

/*
	判断角色  
	> 8 矿工  
	7 p2p所有角色	 
	4~5 中继器  
	2~3 一般服务角色   
	1 纯消费角色
*/
const (
	CONSUMER = 1 << iota
	PROVIDER
	REPEATERS
	MINER
)

// func NewResourceInstance(panic chan error, funcs func(c conf.Config)...) {
// 	for _, f := range funcs {

// 	}
// }

//DBInterface model  interface
type DBInterface interface {
	ModelName() string
}