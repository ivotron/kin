package kin

type Status int

const (
	Committed Status = iota
	Staged
)

type Backend interface {
	// inits backend in current directory
	Init() error

	// opens the backend
	Open() error

	// whether the backend has been initialized
	IsInitialized() bool

	// returns status of backend
	GetStatus() (Status, error)

	// creates a staged commit based on a given one
	Checkout(ref string) (stagedCommit string, err error)

	// marks version as committed. Once a version is committed, no more additions/removals to
	// it are allowed.
	Commit() error

	// adds an object to the current staged commit
	Add(oids []string) error

	// removes an object from the current staged commit
	Remove(oids []string) error

	// retrieves the string representation of the diff for two given objects
	Diff(oids []string) (string, error)
}

// TODO as we go along adding functionality, consider breaking up the Backend interface
// (e.g. making ODB and MDB interfaces embed Backend in it)
//   - CheckoutObjects() would be in ODB
type Repository Backend

type KinError struct {
	Msg string
}

func (e KinError) Error() string {
	return "kin: " + e.Msg
}
