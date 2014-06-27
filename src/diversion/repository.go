package diversion

import (
	"diversion/coordination"
	"diversion/metadb"
	"diversion/opts"
	"fmt"
)

type Repository struct {
	db    metadb.MetaDB
	coord coordination.Coordinator
}

func NewRepository(conf opts.Options) (r *Repository, err error) {
	switch conf.MetaDbType {

	case opts.Memory:
		if r.db, err = metadb.NewMemMetaDB(conf); err != nil {
			return nil, err
		}

	default:
		return nil, DiversionError{fmt.Sprintf("unknown metadata db type %d", conf.MetaDbType)}

	}

	/*
		switch conf.CoordinatorType {

		case opts.SingleClient:
			if r.coord err := coordination.NewSingleClientCoordinator(conf); err != nil {
				return nil, err
			}

		default:
			return nil, DiversionError("unknown coordinator type " + conf.CoordinatorType)
		}
	*/

	return
}
