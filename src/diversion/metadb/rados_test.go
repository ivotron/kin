package diversion

import "testing"
import "github.com/stretchr/testify/assert"

func TestRadosMetaDB(t *testing.T) {
	o := NewOptions()
	o.MetaDbType = Rados
	db := NewRadosMetaDB(o)
	assert.Equal(t, db.PoolName, "data")
}
