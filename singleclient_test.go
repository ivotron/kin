package kin

import (
	"io/ioutil"
	"os"
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestInitRepo(t *testing.T) {
	_, err := os.Getwd()
	assert.Nil(t, err)
	path, err := ioutil.TempDir("", "kin")
	assert.Nil(t, os.Chdir(path))

	assert.Nil(t, Init())

	_, err = os.Stat(".git/")
	assert.Nil(t, err)
}
