package model

import (
	//"fmt"
	"sync"
)

//LevelJudge  LevelJudge
type LevelJudge struct {
	Level      rune
	TotalScore int
	NodeId     string
	IsWork     bool
}

//LevelJudgePool Singleton
var ljpInstance *LevelJudgePool
var onceLevel sync.Once

func GetLevelInstance() *LevelJudgePool {
	onceLevel.Do(func() {
		ljpInstance = &LevelJudgePool{}
	})
	return ljpInstance
}

//LevelJudgePool LevelJudgePool
type LevelJudgePool struct {
	LevelSlice []LevelJudge
	m          sync.Mutex
}

//ADD  ADD
func (l *LevelJudgePool) ADD(nodeId string, rest ...interface{}) {
	l.m.Lock()
	defer l.m.Unlock()
	var score int
	var level rune
	for _, value := range rest {
		switch value := value.(type) {
		case int:
			score = value
		case rune:
			level = value
		}
	}
	levelNode := LevelJudge{level, score, nodeId, true}
	l.LevelSlice = append(l.LevelSlice, levelNode)
}

//DEL DEL
func (l *LevelJudgePool) DEL(nodeId string) {
	l.m.Lock()
	defer l.m.Unlock()
	for k := range l.LevelSlice {
		if l.LevelSlice[k].NodeId == nodeId {
			l.LevelSlice = append(l.LevelSlice[:k], l.LevelSlice[k+1:]...)
			break
		}
	}
}

//Update Update
func (l *LevelJudgePool) Update() {
	//TODO
}

//Rush Rush
func (l *LevelJudgePool) Rush() {
	//TODO
}

//CacheSort CacheSort
func (l *LevelJudgePool) CacheSort() {
	//TODO
}
