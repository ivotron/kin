package coordination

type Coordinator interface {
	// commits.
	Commit(v Version) (err error)

	// makes the given version the head of the repo. Fails if head in the refdb is.
	MakeHEAD(Version v) (err error)

	// open the meta db.
	OpenMetaDB() (err error)

	// retrieves the id of the latest committed version.
	GetHeadId() (vid string, err error)

	// returns an object so that a user can operate against it using the object store API.
	GetObjectId(vid string, oid string) (versioned_oid string, err error)

	// checks out a version.
	Checkout(commitId string) (err error)

	// creates a version based on a given one.
	Create(parent Version) (err error)

	// adds an object to a version.
	Add(v Version, oid string) (err error)

	// removes an object from a version
	Remove(v Version, oid string) (err error)

	// initializes an empty repo. fails if non-empty.
	InitRepository() (err error)

	// whether repo is empty.
	IsRepositoryEmpty() (err error)

	// shuts down. No more operations can be done after this is invoked.
	Shutdown() (err error)
}
