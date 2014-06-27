package metadb

type lockMode int

const (
	Exclusive lockMode = 0
	Shared
)

type MetaDB interface {
	Create(v string, seed string, mode lockMode, key string) (newV string, err error)

	// retrieves the metadata of the given version id
	Checkout(id string) (version string, err error)

	// inits a new db. Might fail if the db has been initialized previously
	Init() (err error)

	// opens the db.
	Open() (err error)

	// closes the db.
	Close() (err error)

	// whether the db is empty.
	IsEmpty()

	// makes the given version the head of the repo.
	MakeHEAD(v string) (err error)

	// changes the status of a version stored at the backed to COMMITTED
	Commit(v string) (err error)

	// adds an object to a version.
	Add(v string, oid string) (err error)

	// removes an object from the given version.
	Remove(v string, oid string) (err error)

	// for versions created with ::SHARED_LOCK, returns the number of locks placed on it.
	GetLockCount(v string, lockKey string) (err error)
}
