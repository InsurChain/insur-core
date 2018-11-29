package model

import (
	//"fmt"
	"math/rand"
	"sort"
	"sync"
	"time"
)

var onceNode sync.Once

//
const SpeedStandard float64 = 30.0
const MinSpeed float64 = 20.0

//NodeSortedPool singleton
var nspInstance *NodeSortedPool

func GetNodeInstance() *NodeSortedPool {
	onceNode.Do(func() {
		nspInstance = &NodeSortedPool{}
	})
	return nspInstance
}

//NodeSorted NodeSorted
type NodeSorted struct {
	NodeId string
	Speed  float64
	IsWork bool
}

//NodeSorted:按照Speed排序,重写方法
type SortedBySpeed []NodeSorted

//NodeSortedPool NodeSortedPool
type NodeSortedPool struct {
	SortedSlice []NodeSorted
	m           sync.Mutex
}

func (a SortedBySpeed) Len() int {
	return len(a)
}

func (a SortedBySpeed) Swap(i, j int) {
	a[i], a[j] = a[j], a[i]
}

func (a SortedBySpeed) Less(i, j int) bool {
	return a[i].Speed > a[j].Speed
}

//ADD ADD
func (node *NodeSortedPool) ADD(nodeId string, rest ...interface{}) {
	node.m.Lock()
	defer node.m.Unlock()
	var speed float64
	for _, value := range rest {
		switch value := value.(type) {
		case float64:
			speed = value
		}
	}
	n := NodeSorted{nodeId, speed, true}
	node.SortedSlice = append(node.SortedSlice, n)
	//fmt.Printf("nodeCache:%v\n",node)

}

//SelectNode SelectNode
func (node *NodeSortedPool) SelectNode() string {
	rand.Seed(time.Now().Unix()) // set time seeds
	return node.SortedSlice[rand.Intn(len(node.SortedSlice))].NodeId
}

//SelectNode SelectNode
func (node *NodeSortedPool) SelectNodeByLen(m int) string {
	rand.Seed(time.Now().Unix()) // set time seeds
	return node.SortedSlice[rand.Intn(m)].NodeId
}

//GetPercentage use to get percentage by speed
func (node *NodeSortedPool) GetPercentage() float64 {
	len1 := len(node.SortedSlice)
	percentage := 0.0
	tmpPer := 40
	if len1 < 30 {
		percentage = 1.0
	} else {
		for {
			if tmpPer != 100 && node.SortedSlice[int(len1*tmpPer/100)].Speed > SpeedStandard {
				tmpPer += 10
				continue
			} else {
				percentage = float64(tmpPer) / 100
				break
			}

		}
	}
	return percentage
}

//DEL DEL
func (node *NodeSortedPool) DEL(nodeId string) {
	node.m.Lock()
	defer node.m.Unlock()
	for k, _ := range node.SortedSlice {
		if node.SortedSlice[k].NodeId == nodeId {
			node.SortedSlice = append(node.SortedSlice[:k], node.SortedSlice[k+1:]...)
		}
	}
}

//Update  Update
func (node *NodeSortedPool) Update() {
	//TODO
}

//CacheSort CacheSort
func (node *NodeSortedPool) CacheSort() {
	node.m.Lock()
	defer node.m.Unlock()
	len1 := len(node.SortedSlice)
	if len1 > 1 {
		for i := 0; i < len1; i++ {
			if i == len1-1 && node.SortedSlice[i-1].Speed < MinSpeed {
				node.SortedSlice = node.SortedSlice[0 : i-1]
				break
			} else if node.SortedSlice[i].Speed < MinSpeed {
				node.SortedSlice = append(node.SortedSlice[:i], node.SortedSlice[i+1:]...)
			}
		}
	}
	sort.Sort(SortedBySpeed(node.SortedSlice))
	//fmt.Println("node Sorted Pool:::",node.SortedSlice)
}

//Rush Rush
func (node *NodeSortedPool) Rush() {
	//TODO
}
