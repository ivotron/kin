package metadb

import (
	"github.com/ivotron/kin/opts"
	"github.com/ivotron/kin/version"
)

type MemMetaDB struct {
}

func NewMemMetaDB(o opts.Options) (db *MemMetaDB, err error) {
	return &MemMetaDB{}, nil
}

func (db MemMetaDB) Clone(p string, c string, m LockMode, key string) (v string, err error) {
	return
}
func (db MemMetaDB) Checkout(id string) (version version.Version, err error) {
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
func (db MemMetaDB) IsEmpty() (bool, error) {
	return true, nil
}
func (db MemMetaDB) MakeHead(v string) (id string, err error) {
	// // this has to be atomic
	// // {
	// if id:= GetHeadId() != v.Parent() {
	//   // someone committed before us, return the id so client can do conflict detection
	//   return GetHeadId()
	// } else {
	//   MakeHead(v.Id)
	//   return v.Id()
	// }
	// // }
	return
}
func (db MemMetaDB) GetHeadId() (vid string, err error) {
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
