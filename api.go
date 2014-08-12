package kin

type Repository interface {
	// creates a version based on a given one. The ID associated to the new version is assigned according to the
	// version ID generation technique (e.g. SHA1, sequential, etc.)
	Checkout(parent string) (v string, err error)

	// marks version as committed. Once a version is committed, no more additions/removals to it are allowed.
	Commit(v string) (err error)

	// makes the given version the head of the repo. Returns the ID of the committed version if it successfully
	// made it the head, otherwise returns the ID of the current one. The latter happens as a result of a race
	// condition: another commit was made the head between the clone/commit operation
	MakeHead(v string) (id string, err error)

	// retrieves the id of the latest committed version.
	GetHeadId() (vid string, err error)

	// adds an object to a version.
	Add(v string, oid string) (err error)

	// removes an object from a version
	Remove(v string, oid string) (err error)

	// initializes an empty repository. fails if non-empty.
	InitRepository() (err error)

	// whether the repository is empty.
	IsRepositoryEmpty() (empty bool, err error)
}
