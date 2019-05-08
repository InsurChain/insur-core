package model

import (
	"fmt"
	"sync"
)

type PluginsCachePool struct {
	PluginCache map[string][]string
	m           sync.Mutex
}

var oncePlugin sync.Once

var pcpInstance *PluginsCachePool

func GetPluginsInstance() *PluginsCachePool {
	oncePlugin.Do(func() {
		pcpInstance = &PluginsCachePool{}
	})
	return pcpInstance
}

//ADD ADD :nodeID  protocal
func (node *PluginsCachePool) ADD(nodeId string, rest ...interface{}) {
	node.m.Lock()
	defer node.m.Unlock()
	if len(node.PluginCache) == 0 {
		node.PluginCache = make(map[string][]string, 8)
	}
	plugin := ""
	for _, value := range rest {
		switch value := value.(type) {
		case string:
			plugin = value
		}
	}

	node.PluginCache["http"] = append(node.PluginCache["http"], nodeId)
	node.PluginCache["https"] = append(node.PluginCache["https"], nodeId)
	node.PluginCache["tcp"] = append(node.PluginCache["tcp"], nodeId)

	if plugin != "http" && plugin != "https" && plugin != "tcp" {
		node.PluginCache[plugin] = append(node.PluginCache[plugin], nodeId)
	}
	fmt.Printf("PluginCache:%v\n", node.PluginCache)
}

func (node *PluginsCachePool) DEL(nodeId string) {
	node.m.Lock()
	defer node.m.Unlock()
	for k, v := range node.PluginCache {
		for i, s := range v {
			if nodeId == s {
				node.PluginCache[k] = append(node.PluginCache[k][:i], node.PluginCache[k][i+1:]...)
				break
			}

		}
	}
}

//Update  Update
func (node *PluginsCachePool) Update() {
	//TODO
}

//CacheSort CacheSort
func (node *PluginsCachePool) CacheSort() {
	//TODO
}

//Rush Rush

func (node *PluginsCachePool) Rush() {
	//TODO
}
