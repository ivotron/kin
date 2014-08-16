package kin

type Status int

const (
	Committed Status = "Committed"
	Staged           = "Staged"
)

type Repository interface {
	// inits repository in current directory
	InitRepository() error

	// creates a version based on a given one. The ID associated to the new version is assigned according to the
	// version ID generation technique (e.g. SHA1, sequential, etc.)
	Checkout(parent string) (string, error)

	// marks version as committed. Once a version is committed, no more additions/removals to it are allowed.
	Commit(v string) error

	// makes the given version the head of the repo. Returns the ID of the committed version if it successfully
	// made it the head, otherwise returns the ID of the current one. The latter happens as a result of a race
	// condition: another commit was made the head between the clone/commit operation
	MakeHead(v string) (string, error)

	// retrieves the id of the latest committed version.
	GetHeadId() (string, error)

	// adds an object to a version.
	Add(args []string, oid string) error

	// removes an object from a version
	Remove(v string, oid string) error

	// whether the repository is empty.
	IsRepositoryEmpty() (bool, error)
}

type Backend interface {
	Init() error
	IsInitialized() bool
}

type KinError struct {
	Msg string
}

func (e KinError) Error() string {
	return "kin: " + e.Msg
}
