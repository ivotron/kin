package metadb

import (
	"testing"

	"kin/opts"

	"github.com/stretchr/testify/assert"
)

func TestRadosMetaDB(t *testing.T) {
	o := opts.NewOptions()
	o.MetaDbType = opts.Rados
	db := NewRadosMetaDB(o)
	assert.Equal(t, db.PoolName, "data")
}