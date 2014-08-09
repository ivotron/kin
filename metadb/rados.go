package metadb

import "github.com/ivotron/kin/opts"

type RadosMetaDB struct {
	PoolName string
}

func NewRadosMetaDB(o opts.Options) *RadosMetaDB {
	return &RadosMetaDB{PoolName: o.RadosMetaDbPoolName}
}

func (db RadosMetaDB) Create(p string, seed string, m LockMode, key string) (v string, err error) {
	return
}
func (db RadosMetaDB) Checkout(id string) (version string, err error) {
	return
}
func (db RadosMetaDB) Init() (err error) {
	return
}
func (db RadosMetaDB) Open() (err error) {
	return
}
func (db RadosMetaDB) Close() (err error) {
	return
}
func (db RadosMetaDB) IsEmpty() {
	return
}
func (db RadosMetaDB) MakeHEAD(v string) (err error) {
	return
}
func (db RadosMetaDB) Commit(v string) (err error) {
	return
}
func (db RadosMetaDB) Add(v string, oid string) (err error) {
	return
}
func (db RadosMetaDB) Remove(v string, oid string) (err error) {
	return
}
func (db RadosMetaDB) GetLockCount(v string, lockKey string) (err error) {
	return
}
