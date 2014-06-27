package metadb

import "diversion/opts"

type MemMetaDB struct {
}

func NewMemMetaDB(o opts.Options) (db *MemMetaDB, err error) {
	return &MemMetaDB{}, nil
}

func (db MemMetaDB) Create(p string, seed string, m lockMode, key string) (v string, err error) {
	return
}
func (db MemMetaDB) Checkout(id string) (version string, err error) {
	return
}
func (db MemMetaDB) Init() (err error) {
	return
}
func (db MemMetaDB) Open() (err error) {
	return
}
func (db MemMetaDB) Close() (err error) {
	return
}
func (db MemMetaDB) IsEmpty() {
	return
}
func (db MemMetaDB) MakeHEAD(v string) (err error) {
	return
}
func (db MemMetaDB) Commit(v string) (err error) {
	return
}
func (db MemMetaDB) Add(v string, oid string) (err error) {
	return
}
func (db MemMetaDB) Remove(v string, oid string) (err error) {
	return
}
func (db MemMetaDB) GetLockCount(v string, lockKey string) (err error) {
	return
}
