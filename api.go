package kin

type Status int

const (
	Committed Status = iota
	Staged
	StatusError
)

type Repository interface {
	// inits repository in current directory
	Init() error

	// returns status of repo
	GetStatus() Status

	// creates a staged commit based on a parent given one
	Checkout(parent string) error

	// marks version as committed. Once a version is committed, no more additions/removals to it are allowed.
	Commit() error

	// adds an object to the current staged commit
	Add(objrefs []string) error

	// removes an object from the current staged commit
	Remove(objrefs []string) error

	// retrieves the string representation of the diff for two given objects
	Diff(objrefs []string) (string, error)
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
