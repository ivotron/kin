package metadb

type lockMode int

const (
	Exclusive lockMode = 0
	Shared
)

type MetaDB interface {
	Create(v Version, seed string, mode lockMode, key string) (newV Version, err error)

	// retrieves the metadata of the given version id. Returns Version::ERROR If the version is not
	// committed; Version::NOT_FOUND if the version doesn't exist in the db.
	Checkout(id string) (version Version, err error)

	// inits a new db. Might fail if the db has been initialized previously
	Init() (err error)

	// opens the db.
	Open() (err error)

	// closes the db.
	Close() (err error)

	// whether the db is empty.
	IsEmpty()

	// makes the given version the head of the repo.
	MakeHEAD(v Version) (err error)

	// changes the status of a version stored at the backed to Version::COMMITTED.
	Commit(v Version) (err error)

	// adds an object to a version.
	Add(v Version, oid string) (err error)

	// removes an object from the given version.
	Remove(v Version, oid string) (err error)

	// for versions created with ::SHARED_LOCK, returns the number of locks placed on it.
	GetLockCount(v Version, lockKey string) (err error)
}
