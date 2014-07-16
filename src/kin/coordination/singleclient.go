package coordination

import (
	"diversion/metadb"
	"diversion/opts"
)

type SingleClientCoordinator struct {
	repoName string
	db       metadb.MetaDB
}

func NewSingleClientCoordinator(o opts.Options, mdb metadb.MetaDB) (c *SingleClientCoordinator, err error) {
	return &SingleClientCoordinator{db: mdb, repoName: o.RepositoryName}, nil
}

func (c SingleClientCoordinator) Clone(parent string) (v string, err error) {
	return c.db.Clone(parent, "", metadb.ExclusiveLock, "")
}

func (c SingleClientCoordinator) Commit(v string) (err error) {
	return c.db.Commit(v)
}

func (c SingleClientCoordinator) MakeHead(v string) (id string, err error) {
	return c.db.MakeHead(v)
}

func (c SingleClientCoordinator) GetHeadId() (vid string, err error) {
	return c.db.GetHeadId()
}

func (c SingleClientCoordinator) Add(v string, oid string) (err error) {
	return c.db.Add(v, oid)
}

func (c SingleClientCoordinator) Remove(v string, oid string) (err error) {
	return c.db.Remove(v, oid)
}

func (c SingleClientCoordinator) InitRepository() (err error) {
	return c.db.Init()
}

func (c SingleClientCoordinator) IsRepositoryEmpty() (isEmpty bool, err error) {
	return c.db.IsEmpty()
}
