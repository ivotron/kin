package kin

import (
	"io/ioutil"
	"os"
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestGitBackendInit(t *testing.T) {
	_, err := os.Getwd()
	assert.Nil(t, err)
	path, err := ioutil.TempDir("", "kin")
	assert.Nil(t, os.Chdir(path))

	opts := NewOptions()
	assert.NotNil(t, opts)
	backend, err := NewGitBackend(opts)
	assert.NotNil(t, backend)
	assert.Nil(t, err)

	_, err = os.Stat(".git/")
	assert.NotNil(t, err)

	err = backend.Init()
	assert.Nil(t, err)

	_, err = os.Stat(".git/")
	assert.Nil(t, err)
}
