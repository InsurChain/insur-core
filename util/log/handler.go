package log

import (
	"fmt"
	"github.com/Sirupsen/logrus"
	"log"
	"os"
	"conf"
	"sync"
)

/*
node  model
*/

//logStats logStats
type logStats struct {
	Levels logrus.Level
	Msg    string
}

var LevelList = []logrus.Level{logrus.ErrorLevel, logrus.WarnLevel, logrus.InfoLevel, logrus.DebugLevel}

//logStack logStack
type logStack struct {
	LogChan    chan *logStats //200
	CacheLines int
	RWMutex    *sync.RWMutex
}

var singleMutex sync.Mutex
var singleLogStack *logStack

//NewLogStack init log stack
func NewLogStack(cache int) *logStack {
	logStack := new(logStack)
	logStack.CacheLines = cache
	logStack.RWMutex = new(sync.RWMutex)
	logStack.LogChan = make(chan *logStats, cache)
	log.Printf("init log chan")
	singleLogStack = logStack
	return logStack
}

//GetLogStack  logstack single model
func GetLogStack(cache ...int) *logStack {
	singleMutex.Lock()
	defer singleMutex.Unlock()
	if singleLogStack == nil {
		lines := 500
		if len(cache) == 1 {
			lines = cache[0]
		}
		return NewLogStack(lines)
	}
	return singleLogStack
}

//Tail read log from cache
func (l *logStack) Tail(num ...int) {
	l.RWMutex.Lock()
	defer l.RWMutex.Unlock()
	if len(num) == 0 {
		log.Println("打印实时日志")
		for {
			select {
			case lss := <-l.LogChan:
				fmt.Println(string(lss.Msg))
			}
		}
	} else {
		log.Println("打印历史日志")
		for {
			select {
			case lss := <-l.LogChan:
				log.Println(string(lss.Msg))
			}
		}
	}
}

//Offer write log into cache
func (l *logStack) Offer(lss *logStats) {
	if len(l.LogChan) == l.CacheLines {
		defer l.RWMutex.RUnlock()
		l.RWMutex.RLock()
		<-l.LogChan
		// log.Println("丢弃", mm.Levels)
	} else {
		// log.Println(len(l.LogChan))
		l.LogChan <- lss
	}
}

//Close close log channel
func (l *logStack) Close() {
	close(l.LogChan)
}

/*
   file model
*/

//MuxWriter MuxWriter
type MuxWriter struct {
	sync.RWMutex
	fd *os.File
}

func (l *MuxWriter) Write(b []byte) (int, error) {
	l.Lock()
	defer l.Unlock()
	return l.fd.Write(b)
}

// Setfd set os.File in writer.
func (l *MuxWriter) Setfd(fd *os.File) {
	if l.fd != nil {
		_ = l.fd.Close()
	}
	l.fd = fd
}

//Close close file fd
func (l *MuxWriter) Close() {
	err := l.fd.Close()
	if err != nil {
		log.Println("Close log file error: ", err.Error())
	}
}

//Logger interface
type Logger interface {
	LoggerInit() error
	DryPool()
	WriteLog(level logrus.Level, msg string) error
	CreateLogFile() (*os.File, error)
}

type Hook struct {
	L Logger
}

//NewHook new hook
func NewHook(c *conf.Config) (h *Hook) {
	l := NewKerriLogger(c)
	err := l.LoggerInit()
	if err != nil {
		log.Println(err.Error())
		return nil
	}
	return &Hook{l}
}

var handlerMutex sync.Mutex
var logHandler *logrus.Logger

func getLogrusFormatter() *logrus.TextFormatter {
	f := new(logrus.TextFormatter)
	f.FullTimestamp = true
	f.TimestampFormat = "2006-01-02 15:04:05"
	f.DisableColors = true
	return f
}

//InitLogger NewLogger
func InitLogger(c *conf.Config) *logrus.Logger {
	loger := logrus.New()
	loger.SetFormatter(getLogrusFormatter())
	loger.SetLevel(c.LogLevel)
	loger.AddHook(NewHook(c))
	if logHandler == nil {
		logHandler = loger
	}
	return loger
}

//GetLogHandler single  model
func GetLogHandler() *logrus.Logger {
	handlerMutex.Lock()
	defer handlerMutex.Unlock()
	if logHandler == nil {
		log.Println("log handler is nil")
		return nil
	}
	return logHandler
}

//Fire Fire
func (h *Hook) Fire(entry *logrus.Entry) (err error) {
	message, err := entry.String()
	if err != nil {
		log.Println("Read entry append error: ", err.Error())
		return err
	}
	switch entry.Level {
	case logrus.PanicLevel:
		fallthrough
	case logrus.FatalLevel:
		fallthrough
	case logrus.ErrorLevel:
		return h.L.WriteLog(logrus.ErrorLevel, message)
	case logrus.WarnLevel:
		return h.L.WriteLog(logrus.WarnLevel, message)
	case logrus.InfoLevel:
		return h.L.WriteLog(logrus.InfoLevel, message)
	case logrus.DebugLevel:
		return h.L.WriteLog(logrus.DebugLevel, message)
	default:
		return nil
	}
}

// Levels 实现Hook的Levels接口
func (h *Hook) Levels() []logrus.Level {
	return logrus.AllLevels
}

const (
	CacheModel = iota
	FileModel
)

type KerriLogWriter struct {
	Model      int          `json:"model"`
	Level      logrus.Level `json:"level"`
	CacheLines int          `json:"cache_lines"`
	FilePath   string       `json:"file_path"`
	mw         *MuxWriter
}

//NewKerriLogger [0]model [1]lines [2]filepath
func NewKerriLogger(c *conf.Config) Logger {
	kw := new(KerriLogWriter)
	kw.Model = c.LogModel
	kw.Level = c.LogLevel
	kw.CacheLines = c.CacheLines
	kw.FilePath = c.LogPath
	return kw
}

//LoggerInit LoggerInit
func (kw *KerriLogWriter) LoggerInit() error {
LOOP:
	switch kw.Model {
	case CacheModel:
		NewLogStack(kw.CacheLines)
	case FileModel:
		kw.mw = new(MuxWriter)
		fd, err := kw.CreateLogFile()
		if err != nil {
			return err
		}
		kw.mw.Setfd(fd)
	default:
		kw.Model = CacheModel
		goto LOOP
	}
	return nil
}

//WriteLog WriteLog
func (kw *KerriLogWriter) WriteLog(level logrus.Level, msg string) error {
	if level > kw.Level {
		return nil
	}
	switch kw.Model {
	case CacheModel:
		lss := &logStats{
			Levels: level,
			Msg:    msg,
		}
		GetLogStack().Offer(lss)
	case FileModel:
		kw.DoFile(level, msg)
	default:
		return nil
	}
	return nil
}

//DoFile write log into file
func (kw *KerriLogWriter) DoFile(level logrus.Level, msg string) error {
	_, err := os.Lstat(kw.FilePath)
	if err != nil {
		return err
	}
	// log.SetOutput(kw.mw.fd)
	if _, err = kw.mw.Write([]byte(msg)); err != nil {
		log.Panic("log write error: ", err.Error())
		return err
	}
	return nil
}

//DryPool DryPool
func (kw *KerriLogWriter) DryPool() {
	switch kw.Model {
	case CacheModel:
		GetLogStack().Close()
	case FileModel:
		kw.mw.Close()
	}
}

//CreateLogFile CreateLogFile
func (kw *KerriLogWriter) CreateLogFile() (*os.File, error) {
	// Open the log file
	fd, err := os.OpenFile(kw.FilePath, os.O_WRONLY|os.O_APPEND|os.O_CREATE, 0660)
	return fd, err
}
   
