package kin

import (
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestIdGeneration(t *testing.T) {
	id, err := Generate("1", Sequential)
	assert.Equal(t, id, "2")
	assert.Nil(t, err)

	id, err = Generate("foo", Sequential)
	assert.NotNil(t, err)
}
