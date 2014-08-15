package kin

import (
	"os"
)

const git_path = ".git/kin/objects"

type PosixBackend struct {
	redirect  bool
	base_path string
	link_path string
}

func NewPosixBackend(o Options) (b *PosixBackend, err error) {
	if o.MetaDbType == Git {
		return &PosixBackend{redirect: true, base_path: git_path, link_path: "."}, nil
	} else {
		return &PosixBackend{redirect: false, base_path: ".", link_path: "."}, nil
	}
}

func (b PosixBackend) Init() (err error) {
	if b.base_path == git_path {
		return os.MkdirAll(git_path, 0770)
	}
	return nil
}

func (b PosixBackend) IsInitialized() bool {
	_, err := os.Stat(b.base_path)
	return (err == nil)
}
