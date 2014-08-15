package kin

func OpenRepository() (repo Repository, err error) {
	conf := NewOptions()
	return NewRepositoryWithOptions(conf)
}

func InitRepository() (repo Repository, err error) {
	if repo, err = NewRepository(); err != nil {
		return nil, err
	}
	err = repo.InitRepository()
	return
}

func NewRepository() (repo Repository, err error) {
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
