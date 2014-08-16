package kin

import (
	"container/list"
)

type Version struct {
	Status  Status
	Parent  string
	Id      string
	Objects list.List
}
