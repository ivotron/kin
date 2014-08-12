package kin

import (
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestDefaultOptions(t *testing.T) {
	o := NewOptions()

	assert.Equal(t, o.Coordinator, SingleClient)
	assert.Equal(t, o.ObjectDbType, Memory)
	assert.Equal(t, o.MetaDbType, Memory)
	assert.Equal(t, o.ClientSync, AtCommit)
	assert.Equal(t, o.IdGenerationMethod, SHA1)
	assert.Equal(t, o.RadosMetaDbPoolName, "data")
	assert.Equal(t, o.RadosObjDbPoolName, "data")
}
