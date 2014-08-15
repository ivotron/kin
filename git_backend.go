package kin

import (
	"os"

	"github.com/libgit2/git2go"
)

type GitBackend struct {
	path string
}

func NewGitBackend(o Options) (b *GitBackend, err error) {
	return &GitBackend{path: o.GitPath}, nil
}

func (b GitBackend) Init() (err error) {
	_, err = git.InitRepository(".", false)
	return
}

func (b GitBackend) IsInitialized() bool {
	_, err := os.Stat(b.path + "/.git")
	return (err == nil)
}
