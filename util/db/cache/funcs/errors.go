package funcs

import (
	"errors"
)

var (
	// ErrKeyNotFound gets returned when a specific key couldn't be found
	ErrKeyNotFound = errors.New("Key not found in cache")
	// ErrKeyNotFoundOrLoadable gets returned when a specific key couldn't be
	// found and loading via the data-loader callback also failed
	ErrKeyNotFoundOrLoadable = errors.New("Key not found and could not be loaded into cache")
)
   
