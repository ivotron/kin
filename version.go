package diversion

import (
	"container/list"
)

type status int

// status
const (
	Staged status = iota
	Committed
)

type Version struct {
	Status  status
	ID      string
	Objects list.List
}
