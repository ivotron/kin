package kin

type SingleClientCoordinator struct {
	repoName string
	mdb      Backend
	odb      Backend
}

func NewSingleClientCoordinator(o Options, mdb Backend, odb Backend) (c *SingleClientCoordinator, err error) {
	return &SingleClientCoordinator{repoName: o.RepositoryName, odb: odb, mdb: mdb}, nil
}

func (c SingleClientCoordinator) InitRepository() (err error) {
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

func (c SingleClientCoordinator) Checkout(parent string) (v string, err error) {
	return "", nil
}

func (c SingleClientCoordinator) Commit(v string) (err error) {
	return nil
}

func (c SingleClientCoordinator) MakeHead(v string) (id string, err error) {
	return "", nil
}

func (c SingleClientCoordinator) GetHeadId() (vid string, err error) {
	return "", nil
}

func (c SingleClientCoordinator) Add(v string, oid string) (err error) {
	return nil
}

func (c SingleClientCoordinator) Remove(v string, oid string) (err error) {
	return nil
}

func (c SingleClientCoordinator) IsRepositoryEmpty() (isEmpty bool, err error) {
	return true, nil
}
