package kin

import (
	"os"
)

const git_path = ".git/kin/objects"

type PosixBackend struct {
	redirect  bool
	base_path string
	link_path string

	// after staging a commit, if this is turned on, every object is read-only unless it's
	// unlocked. If an object is to be written, it has to be explicitly unlocked. The main
	// reason for this is to avoid performance hits on non-CoW filesystems, since an unlock
	// triggers a copy
	lock_by_default bool
}

func NewPosixBackend(o Options) (b *PosixBackend, err error) {
	if o.MetaDbType == Git {
		return &PosixBackend{redirect: true, base_path: git_path, link_path: ".", lock_by_default: true},
			nil
	} else {
		return &PosixBackend{redirect: false, base_path: ".", link_path: ".", lock_by_default: false}, nil
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
func (b PosixBackend) GetStatus() Status {
	return Committed
}
func (b PosixBackend) Commit() (err error) {
	return KinError{"not yet"}
}
func (b PosixBackend) Checkout(parent string) (stagedCommit string, err error) {
	return "", KinError{"not supported"}
}
func (b PosixBackend) Add(oids []string) (err error) {
	return KinError{"not yet"}
}
func (b PosixBackend) Remove(oids []string) (err error) {
	return KinError{"not yet"}
}
func (b PosixBackend) Diff(objrefs []string) (string, error) {
	return "", KinError{"not yet"}
}
func (b PosixBackend) Open() error {
	return KinError{"not yet"}
}
