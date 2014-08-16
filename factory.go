package kin

func Init() (err error) {
	repo, err := OpenRepository()

	if err != nil {
		return
	}

	return repo.Init()
}

func Add(args []string) (err error) {
	repo, err := OpenRepository()

	if err != nil {
		return
	}

	if repo.GetStatus() == Committed {
		return KinError{"Create a staged commit by checking out one first."}
	}

	return repo.Add(args)
}

func Checkout(id string) (err error) {
	repo, err := OpenRepository()

	if err != nil {
		return
	}

	if repo.GetStatus() == Staged {
		return KinError{"Commit staged changes first."}
	}

	return repo.Checkout(id)
}

func Commit() (err error) {
	repo, err := OpenRepository()

	if err != nil {
		return
	}

	if repo.GetStatus() == Committed {
		return KinError{"Nothing to commit; create a staged commit first."}
	}

	return repo.Commit()
}

func Remove(args []string) (err error) {
	repo, err := OpenRepository()

	if err != nil {
		return
	}

	if repo.GetStatus() == Committed {
		return KinError{"Create a staged commit by checking out one first."}
	}

	return repo.Remove(args)
}

func Diff(args []string) (diffoutput string, err error) {
	repo, err := OpenRepository()

	if err != nil {
		return
	}

	return repo.Diff(args)
}

func OpenRepository() (repo Repository, err error) {
	conf := NewOptions()
	return NewRepositoryWithOptions(conf)
}

func NewRepositoryWithOptions(conf Options) (repo Repository, err error) {
	var odb Backend
	switch conf.ObjectDbType {
	case Posix:
		if odb, err = NewPosixBackend(conf); err != nil {
			return
		}
	default:
		return nil, KinError{"unknown object backend " + string(conf.ObjectDbType)}
	}

	var mdb Backend
	switch conf.MetaDbType {
	case Git:
		if mdb, err = NewGitBackend(conf); err != nil {
			return
		}
	default:
		return nil, KinError{"unknown meta backend " + string(conf.MetaDbType)}
	}

	switch conf.Coordinator {

	case SingleClient:
		if repo, err = NewSingleClientCoordinator(conf, mdb, odb); err != nil {
			return nil, err
		} else {
			return repo, nil
		}

	default:
		return nil, KinError{"unknown coordination type " + string(conf.Coordinator)}
	}
}
