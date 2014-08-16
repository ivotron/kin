package kin

type SingleClientCoordinator struct {
	repoName string
	mdb      Backend
	odb      Backend
}

func NewSingleClientCoordinator(o Options, mdb Backend, odb Backend) (c *SingleClientCoordinator, err error) {
	return &SingleClientCoordinator{repoName: o.RepositoryName, odb: odb, mdb: mdb}, nil
}

func (c SingleClientCoordinator) Init() (err error) {
	if c.mdb.IsInitialized() {
		return KinError{"Meta backend already initialized"}
	}
	if c.odb.IsInitialized() {
		return KinError{"Object backend already initialized"}
	}

	if err = c.mdb.Init(); err != nil {
		return
	}

	if err = c.odb.Init(); err != nil {
		return
	}

	return
}

func (c SingleClientCoordinator) GetStatus() Status {
	return StatusError
}

func (c SingleClientCoordinator) Checkout(parent string) (err error) {
	return KinError{"not yet"}
}

func (c SingleClientCoordinator) Commit() (err error) {
	return KinError{"not yet"}
}

func (c SingleClientCoordinator) Add(oids []string) (err error) {
	return KinError{"not yet"}
}

func (c SingleClientCoordinator) Remove(oids []string) (err error) {
	return KinError{"not yet"}
}
func (c SingleClientCoordinator) Diff(objrefs []string) (string, error) {
	return "", KinError{"not yet"}
}
