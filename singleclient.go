package kin

type SingleClientCoordinator struct {
	repoName string
}

func NewSingleClientCoordinator(o Options) (c *SingleClientCoordinator, err error) {
	return &SingleClientCoordinator{repoName: o.RepositoryName}, nil
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

func (c SingleClientCoordinator) InitRepository() (err error) {
	return nil
}

func (c SingleClientCoordinator) IsRepositoryEmpty() (isEmpty bool, err error) {
	return true, nil
}
