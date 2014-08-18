package kin

import (
	"io/ioutil"
	"os"
	"testing"

	"github.com/stretchr/testify/assert"
)

func createGitBackend(t *testing.T) *GitBackend {
	path, err := ioutil.TempDir("", "kin")
	assert.Nil(t, os.Chdir(path))

	opts := NewOptions()
	assert.NotNil(t, opts)
	backend, err := NewGitBackend(opts)
	assert.NotNil(t, backend)
	assert.Nil(t, err)
	return backend
}

func TestGitBackendInit(t *testing.T) {
	backend := createGitBackend(t)
	assert.NotNil(t, backend)

	_, err := os.Stat(".git/")
	assert.NotNil(t, err)

	assert.False(t, backend.IsInitialized())

	assert.Nil(t, backend.Init())

	assert.True(t, backend.IsInitialized())

	_, err = os.Stat(".git/")
	assert.Nil(t, err)
	_, err = os.Stat(".gitignore")
	assert.Nil(t, err)
}

func TestGitBackendOpen(t *testing.T) {
	backend := createGitBackend(t)
	assert.NotNil(t, backend)
	assert.NotNil(t, backend.Open())
	assert.Nil(t, backend.Init())
	backend, err := NewGitBackend(NewOptions())
	assert.NotNil(t, backend)
	assert.Nil(t, err)
	assert.Nil(t, backend.Open())
}

func TestGitBackendCheckout(t *testing.T) {
	backend := createGitBackend(t)
	assert.NotNil(t, backend)
	assert.Nil(t, backend.Init())
	_, _, err := backend.Checkout("master")
	assert.NotNil(t, err)
	_, _, err = backend.Checkout("HEAD")
	assert.Nil(t, err)
}
