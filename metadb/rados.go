package metadb

type RadosMetaDB struct {
	PoolName string
}

func NewRadosMetaDB(o Options) *RadosMetaDB {
	return &RadosMetaDB{PoolName: o.RadosMetaDbPoolName}
}

func (db RadosMetaDB) Create(p Version, seed string, m lockMode, key string) (v Version, err error) {
	return
}
func (db RadosMetaDB) Checkout(id string) (version Version, err error) {
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
func (db RadosMetaDB) MakeHEAD(v Version) (err error) {
	return
}
func (db RadosMetaDB) Commit(v Version) (err error) {
	return
}
func (db RadosMetaDB) Add(v Version, oid string) (err error) {
	return
}
func (db RadosMetaDB) Remove(v Version, oid string) (err error) {
	return
}
func (db RadosMetaDB) GetLockCount(v Version, lockKey string) (err error) {
	return
}
