package model

import (
	"container/list"
)

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
	PROVIDER  	//2
	REPEATERS	//4
	MINER		//8
)

// func NewResourceInstance(panic chan error, funcs func(c conf.Config)...) {
// 	for _, f := range funcs {

// 	}
// }

//DBInterface model  interface
type DBInterface interface {
	ModelName() string
}


//Repeaters Repeaters
type Repeaters struct {
	NodeList 		*list.List		`json:"node_list"`
}

//NewRepeaters new repeaters list
func NewRepeaters() *Repeaters {
	return &Repeaters{
		NodeList: list.New(),
	}
}

//ModelName return model name
func (r *Repeaters) ModelName() string {
	return "repeater_nodes"
}

//BandWidthProvider BandWidthProvider
type BandWidthProvider struct {
	NodeList 		*list.List 		`json:"node_list"`
}

//BandWidthItem BandWidthItem
type BandWidthItem struct {
	nodeID		string 
	bandWidth	float64
}

//NewBandwidthItem NewBandwidthItem
func NewBandwidthItem(b *BasicNode) *BandWidthItem {
	return &BandWidthItem{
		nodeID: b.NodeInfo.NodeID,
		bandWidth: b.Hardware.BandWidth,
	}
}

//GetID return bandwidth id
func (b *BandWidthItem) GetID() string {
	return b.nodeID
}

//GetBandwidth return bandwidth 
func (b *BandWidthItem) GetBandwidth() float64 {
	return b.bandWidth
}

//NewBandWidthProvider return new bandwitdh provider
func NewBandWidthProvider() *BandWidthProvider {
	return &BandWidthProvider {
		NodeList: list.New(),
	}
}

//ModelName return bandwidth provider model name
func (r *BandWidthProvider) ModelName() string {
	return "bandWidth_provider"
}   
