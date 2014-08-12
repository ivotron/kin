package kin

import (
	"io/ioutil"
	"runtime"

	"testing"
	"time"

	"github.com/libgit2/git2go"
	"github.com/stretchr/testify/assert"
)

func createTestRepo(t *testing.T) *git.Repository {
	// figure out where we can create the test repo
	path, err := ioutil.TempDir("", "git2go")
	checkFatal(t, err)
	repo, err := git.InitRepository(path, false)
	checkFatal(t, err)

	return repo
}

func seedTestRepo(t *testing.T, repo *git.Repository) *git.Oid {
	loc, err := time.LoadLocation("Europe/Berlin")
	checkFatal(t, err)
	sig := &git.Signature{
		Name:  "Rand Om Hacker",
		Email: "random@hacker.com",
		When:  time.Date(2013, 03, 06, 14, 30, 0, 0, loc),
	}
	treeBuilder, err := repo.TreeBuilder()
	checkFatal(t, err)
	oid, err := git.NewOid("de9f2c7fd25e1b3afad3e85a0bd17d9b100db4b3")
	checkFatal(t, err)
	err = treeBuilder.Insert("foo", oid, git.FilemodeBlob)
	checkFatal(t, err)
	treeId, err := treeBuilder.Write()
	checkFatal(t, err)
	message := "This is a commit\n"
	tree, err := repo.LookupTree(treeId)
	checkFatal(t, err)
	commitId, err := repo.CreateCommit("HEAD", sig, sig, message, tree)
	checkFatal(t, err)
	return commitId
}

func TestCreateRepo(t *testing.T) {
	repo := createTestRepo(t)
	commitId := seedTestRepo(t, repo)

	assert.NotNil(t, commitId)
}

func checkFatal(t *testing.T, err error) {
	if err == nil {
		return
	}
	// The failure happens at wherever we were called, not here
	_, file, line, ok := runtime.Caller(1)
	if !ok {
		t.Fatal()
	}
	t.Fatalf("Fail at %v:%v; %v", file, line, err)
}

// kin
// kin status
// kin init
// kin add objects/
