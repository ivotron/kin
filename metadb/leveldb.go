package metadb

import (
	"github.com/ivotron/kin/opts"
	"github.com/ivotron/kin/version"

	"github.com/syndtr/goleveldb/leveldb"
)

type LevelDBMetaDB struct {
}

func NewLevelDBMetaDB(o opts.Options) (db *LevelDBMetaDB, err error) {
	db, err := leveldb.OpenFile("path/to/db", nil)
	return &LevelDBMetaDB{}, nil
}

func (db LevelDBMetaDB) Clone(p string, c string, m LockMode, key string) (string, error) {
	return
}
func (db LevelDBMetaDB) Checkout(id string) (version version.Version, err error) {
	return
}
func (db LevelDBMetaDB) Init() (err error) {
	return
}
func (db LevelDBMetaDB) Open() (err error) {
	return
}
func (db LevelDBMetaDB) Close() (err error) {
	return
}
func (db LevelDBMetaDB) IsEmpty() (bool, error) {
	return true, nil
}
func (db LevelDBMetaDB) MakeHead(v string) (id string, err error) {
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
func (db LevelDBMetaDB) GetHeadId() (vid string, err error) {
	return
}
func (db LevelDBMetaDB) Commit(v string) (err error) {
	return
}
func (db LevelDBMetaDB) Add(v string, oid string) (err error) {
	return
}
func (db LevelDBMetaDB) Remove(v string, oid string) (err error) {
	return
}
func (db LevelDBMetaDB) GetLockCount(v string, lockKey string) (err error) {
	return
}
