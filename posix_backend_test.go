package kin

import (
	"io/ioutil"
	"os"
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestPosixBackendInit(t *testing.T) {
	_, err := os.Getwd()
	assert.Nil(t, err)
	path, err := ioutil.TempDir("", "kin")
	assert.Nil(t, os.Chdir(path))

	opts := NewOptions()
	assert.NotNil(t, opts)

	// check other-than-git refdb backend
	opts.MetaDbType = Memory
	backend, err := NewPosixBackend(opts)
	assert.NotNil(t, backend)
	assert.Nil(t, err)
	assert.False(t, backend.redirect)
	assert.Equal(t, backend.base_path, ".")
	assert.True(t, backend.IsInitialized())
	assert.Nil(t, backend.Init())
	assert.True(t, backend.IsInitialized())

	// check with git refdb backend
	opts.MetaDbType = Git
	backend, err = NewPosixBackend(opts)
	assert.NotNil(t, backend)
	assert.Nil(t, err)
	assert.True(t, backend.redirect)
	assert.Equal(t, backend.base_path, ".git/kin/objects")
	assert.Equal(t, backend.link_path, ".")
	assert.False(t, backend.IsInitialized())

	err = os.Mkdir(".git", 0770)
	assert.Nil(t, err)
	_, err = os.Stat(".git/")
	assert.Nil(t, err)

	assert.Nil(t, backend.Init())
	assert.True(t, backend.IsInitialized())
}
