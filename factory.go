package kin

func Init() (err error) {
	repo, err := NewRepository()

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

	return repo.Add(args)
}

func Checkout(id []string) (err error) {
	if len(id) > 1 {
		return KinError{"Only one reference allowed."}
	}

	repo, err := OpenRepository()

	if err != nil {
		return
	}

	if len(id) == 0 {
		_, _, err = repo.Checkout("HEAD")
	} else {
		_, _, err = repo.Checkout(id[0])
	}
	return
}

func Commit() (err error) {
	repo, err := OpenRepository()

	if err != nil {
		return
	}

	return repo.Commit()
}

func Remove(args []string) (err error) {
	repo, err := OpenRepository()

	if err != nil {
		return
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
	if repo, err = NewRepository(); err != nil {
		return
	}
	repo.Open()
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
