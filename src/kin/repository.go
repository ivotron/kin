package diversion

import (
	"diversion/coordination"
	"diversion/metadb"
	"diversion/opts"
	"fmt"
)

type Repository interface {
	// creates a version based on a given one. The ID associated to the new version is assigned according to the
	// version ID generation technique (e.g. SHA1, sequential, etc.)
	Clone(parent string) (v string, err error)

	// marks version as committed. Once a version is committed, no more additions/removals to it are allowed.
	Commit(v string) (err error)

	// makes the given version the head of the repo. Returns the ID of the committed version if it successfully
	// made the given version the head, otherwise returns the ID of the current. The latter happens when another
	// commit was made the head
	MakeHead(v string) (id string, err error)

	// retrieves the id of the latest committed version.
	GetHeadId() (vid string, err error)

	// adds an object to a version.
	Add(v string, oid string) (err error)

	// removes an object from a version
	Remove(v string, oid string) (err error)

	// initializes an empty repository. fails if non-empty.
	InitRepository() (err error)

	// whether the repository is empty.
	IsRepositoryEmpty() (empty bool, err error)
}

func NewRepository(conf opts.Options) (repo Repository, err error) {
	var db metadb.MetaDB

	switch conf.MetaDbType {
	case opts.Memory:
		if db, err = metadb.NewMemMetaDB(conf); err != nil {
			return nil, err
		}

	default:
		return nil, DiversionError{fmt.Sprintf("unknown metadata db type %d", conf.MetaDbType)}

	}

	switch conf.CoordinatorType {

	case opts.SingleClient:
		if repo, err := coordination.NewSingleClientCoordinator(conf, db); err != nil {
			return nil, err
		} else {
			return repo, nil
		}

	default:
		return nil, DiversionError{fmt.Sprintf("unknown coordination type %d", conf.CoordinatorType)}
	}
}
