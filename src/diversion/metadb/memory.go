package diversion

type MemMetaDB struct {
}

func NewMemMetaDB(o Options) *MemMetaDB {
	return &MemMetaDB{PoolName: o.MemMetaDbPoolName}
}

func (db MemMetaDB) Create(p Version, seed string, m lockMode, key string) (v Version, err error) {
	return
}
func (db MemMetaDB) Checkout(id string) (version Version, err error) {
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
func (db MemMetaDB) MakeHEAD(v Version) (err error) {
	return
}
func (db MemMetaDB) Commit(v Version) (err error) {
	return
}
func (db MemMetaDB) Add(v Version, oid string) (err error) {
	return
}
func (db MemMetaDB) Remove(v Version, oid string) (err error) {
	return
}
func (db MemMetaDB) GetLockCount(v Version, lockKey string) (err error) {
	return
}
