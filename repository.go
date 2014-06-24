package diversion

import "github.com/ivotron/diversion/metadb"
import "github.com/ivotron/diversion/coordination"

type Repository struct {
	metadb      metadb.MetaDB
	coordinator coordination.Coordinator
}

func NewRepository(o Options) (r *Repository, err error) {
	switch o.MetaDbType {
	case Memory:
		if (r.metadb, err) := metadb.NewMemMetaDB(o); err != nil {
			return
		}
	default:
		return nil, DiversosError("unknown metadata db type " + o.MetaDbType)
	}

	switch o.CoordinatorType {
	case SingleClient:
		if (r.metadb, err) := coordination.NewSingleClientCoordinator(o); err != nil {
			return
		}
	default:
		return nil, DiversosError("unknown coordinator type " + o.CoordinatorType)
	}

	return
}
