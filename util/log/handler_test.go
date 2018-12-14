package log

import (
	"github.com/Sirupsen/logrus"
	"os"
	"pnt/conf"
	"testing"
)

func TestCacheModel(t *testing.T) {
	c := &conf.Config{
		LogModel:   0,
		LogLevel:   logrus.DebugLevel,
		CacheLines: 300,
	}
	log := InitLogger(c)
	go GetLogStack().Tail()
	for i := 0; i < 100; i++ {
		log.WithFields(logrus.Fields{
			"xxxx": "ddddd",
		}).Debug("this is a test msg")
	}
}

func TestFileModel(t *testing.T) {
	c := &conf.Config{
		LogModel:   1,
		LogLevel:   logrus.DebugLevel,
		CacheLines: 300,
		LogPath:    "/tmp/kerri.log",
	}
	log := InitLogger(c)
	log.WithFields(logrus.Fields{
		"xxxx": "ddddd",
	}).Debug("this is a test msg")
}

var config struct {
	Key string
	Val string
}

func TestLog(t *testing.T) {
	//_ := &conf.Config{
	//	LogModel:   0,
	//	LogLevel:   logrus.DebugLevel,
	//	CacheLines: 300,
	//}
	log := logrus.New()
	log.SetFormatter(&logrus.JSONFormatter{})
	log.SetReportCaller(true)
	f, _ := os.OpenFile("t.log", os.O_CREATE|os.O_WRONLY, 0666)
	log.SetOutput(f)
	config.Key = "123-Key"
	config.Val = "123-Val"
	log.WithFields(logrus.Fields{"key": "111"}).Info("hahah")
	log.Info("666")

}
