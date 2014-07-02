package metadb

import (
	"diversion/version"
)

type lockMode int

const (
	ExclusiveLock lockMode = 0
	SharedLock
)

type MetaDB interface {
	// Create a new version based on given parent
	Clone(p string, seed string, mode lockMode, key string) (v string, err error)

	// retrieves the metadata of the given version id
	Checkout(id string) (version version.Version, err error)

	// inits a new db. Might fail if the db has been initialized previously
	Init() (err error)

	// opens the db.
	Open() (err error)

	// closes the db.
	Close() (err error)

	// whether the db is empty.
	IsEmpty() (bool, error)

	// retrieves the id of the latest committed version.
	GetHeadId() (vid string, err error)

	// makes the given version the head of the repo.
	MakeHead(v string) (err error)

	// changes the status of a version stored at the backed to COMMITTED
	Commit(v string) (err error)

	// adds an object to a version.
	Add(v string, oid string) (err error)

	// removes an object from the given version.
	Remove(v string, oid string) (err error)

	// for versions created with SharedLock, returns the number of locks placed on it.
	GetLockCount(v string, lockKey string) (err error)
}
