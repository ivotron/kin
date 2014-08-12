package version

import (
	"testing"

	"github.com/ivotron/kin/opts"

	"github.com/stretchr/testify/assert"
)

func TestIdGeneration(t *testing.T) {
	id, err := Generate("1", opts.Sequential)
	assert.Equal(t, id, "2")
	assert.Nil(t, err)

	id, err = Generate("foo", opts.Sequential)
	assert.NotNil(t, err)
}
