package version

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
	Parent  string
	Id      string
	Objects list.List
}
