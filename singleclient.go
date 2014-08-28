package kin

type SingleClientCoordinator struct {
	repoName string
	mdb      Backend
	odb      Backend
}

func NewSingleClientCoordinator(o Options, mdb Backend, odb Backend) (c *SingleClientCoordinator, err error) {
	return &SingleClientCoordinator{repoName: o.RepositoryName, odb: odb, mdb: mdb}, nil
}

func (c SingleClientCoordinator) checkBackends(expected Status) error {
	if !c.mdb.IsInitialized() {
		return KinError{"Meta backend not initialized"}
	}
	if !c.odb.IsInitialized() {
		return KinError{"Object backend not initialized"}
	}

	if actual, err := c.mdb.GetStatus(); expected != actual {
		return KinError{"Wrong meta backend status; expecting the oposite."}
	} else if err != nil {
		return err
	}

	if actual, err := c.odb.GetStatus(); expected != actual {
		return KinError{"Wrong object backend status; expecting the oposite."}
	} else if err != nil {
		return err
	}

	return nil
}

func (c SingleClientCoordinator) IsInitialized() (isInitialized bool) {
	if c.mdb.IsInitialized() && c.odb.IsInitialized() {
		return true
	}
	return false
}

func (c SingleClientCoordinator) Open() (err error) {
	if err = c.mdb.Open(); err != nil {
		return
	}
	if err = c.odb.Open(); err != nil {
		return
	}

	return nil
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

func (c SingleClientCoordinator) GetStatus() (Status, error) {
	return c.mdb.GetStatus()
}

func (c SingleClientCoordinator) Checkout(parentCommit string) (stagedId string, err error) {
	stagedId = ""

	if err = c.checkBackends(Staged); err != nil {
		return
	}

	// make this atomic
	// {
	if stagedId, err = c.mdb.Checkout(parentCommit); err != nil {
		return
	}
	if _, err = c.odb.Checkout(stagedId); err != nil {
		return
	}
	// }

	return
}

func (c SingleClientCoordinator) Commit() (err error) {
	if err = c.checkBackends(Committed); err != nil {
		return
	}

	// make this atomic
	// {
	if err = c.odb.Commit(); err != nil {
		return
	}
	if err = c.mdb.Commit(); err != nil {
		return
	}
	// }

	return nil
}

func (c SingleClientCoordinator) Add(oids []string) (err error) {
	if err = c.checkBackends(Staged); err != nil {
		return
	}

	// make this atomic
	// {
	if err = c.odb.Add(oids); err != nil {
		return
	}
	if err = c.mdb.Add(oids); err != nil {
		return
	}
	// }

	return nil
}

func (c SingleClientCoordinator) Remove(oids []string) (err error) {
	if err = c.checkBackends(Staged); err != nil {
		return
	}

	// make this atomic
	// {
	if err = c.odb.Remove(oids); err != nil {
		return
	}
	if err = c.mdb.Remove(oids); err != nil {
		return
	}
	// }

	return nil
}

func (c SingleClientCoordinator) Diff(objrefs []string) (string, error) {
	return "", KinError{"not yet"}
}
