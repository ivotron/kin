package kin

import (
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestIdGeneration(t *testing.T) {
	id, err := GenerateID("1", Sequential)
	assert.Equal(t, id, "2")
	assert.Nil(t, err)

	id, err = GenerateID("foo", Sequential)
	assert.NotNil(t, err)

	id, err = GenerateID("a", SHA1)
	assert.Nil(t, err)

	id2, err := GenerateID("a", SHA1)
	assert.Nil(t, err)

	assert.NotEqual(t, id, id2)
}
