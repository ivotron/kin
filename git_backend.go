package kin

import (
	"io/ioutil"
	"os"
	"os/exec"
	"time"

	"github.com/libgit2/git2go"
)

type GitBackend struct {
	gitRepo *git.Repository
	path    string
}

func NewGitBackend(o Options) (b *GitBackend, err error) {
	return &GitBackend{path: o.GitPath, gitRepo: nil}, nil
}

func (b *GitBackend) seedRepo() (err error) {
	loc, err := time.LoadLocation("America/Hermosillo")
	if err != nil {
		return
	}

	sig := &git.Signature{
		Name:  "Rand Om Hacker",
		Email: "random@hacker.com",
		When:  time.Date(2013, 03, 06, 14, 30, 0, 0, loc),
	}

	treeBuilder, err := b.gitRepo.TreeBuilder()
	if err != nil {
		return
	}

	// add .gitignore file containing .staged
	gitIgnore := []byte(".staged\n.status")
	oid, err := b.gitRepo.CreateBlobFromBuffer(gitIgnore)
	if err != nil {
		return
	}
	if err = ioutil.WriteFile(b.path+"/.staged", []byte(oid.String()), 0644); err != nil {
		return
	}
	if err = ioutil.WriteFile(b.path+"/.status", []byte("Committed"), 0644); err != nil {
		return
	}
	err = treeBuilder.Insert(".gitignore", oid, git.FilemodeBlob)
	if err != nil {
		return
	}

	treeId, err := treeBuilder.Write()
	if err != nil {
		return
	}

	message := "Initial commit.\n"
	tree, err := b.gitRepo.LookupTree(treeId)
	if err != nil {
		return
	}

	_, err = b.gitRepo.CreateCommit("HEAD", sig, sig, message, tree)

	return b.gitRepo.CheckoutHead(&git.CheckoutOpts{Strategy: git.CheckoutForce})
}

func (b *GitBackend) Init() (err error) {
	b.gitRepo, err = git.InitRepository(b.path, false)
	if err != nil {
		return
	}
	err = b.seedRepo()
	return
}

func (b *GitBackend) Open() (err error) {
	b.gitRepo, err = git.OpenRepository(b.path)
	return
}

func (b *GitBackend) IsInitialized() bool {
	_, err := os.Stat(b.path + "/.git")
	return (err == nil)
}
func (b *GitBackend) GetStatus() (status Status, err error) {
	if _, err = os.Stat(b.path + "/.status"); err != nil {
		return
	}

	statusStr, err := ioutil.ReadFile(b.path + "/.status")
	if err != nil {
		return
	}

	switch string(statusStr) {
	case "Committed":
		return Committed, nil
	case "Staged":
		return Staged, nil
	default:
		return Committed, KinError{"Unexpected value of .status of repo: " + string(statusStr)}
	}
}

func (b *GitBackend) Checkout(ref string) (stagedId string, err error) {
	if b.gitRepo == nil {
		return "", KinError{"nil pointer to git repo"}
	}

	// checkout working tree (will include logic for branches and detached HEADs later) {
	cmd := exec.Command("git", "checkout", ref)

	if _, err = cmd.Output(); err != nil {
		return
	}

	// get an ID to associate the staged changes
	// {
	parentId, err := ioutil.ReadFile(b.path + "/.staged")
	if err != nil {
		return
	}
	if parentId == nil {
		return "", KinError{"This sucks"}
	}

	stagedId, err = GenerateID(string(parentId), SHA1)
	if err != nil {
		return
	}

	if err = ioutil.WriteFile(b.path+"/.staged", []byte(stagedId), 0644); err != nil {
		return
	}
	if err = ioutil.WriteFile(b.path+"/.status", []byte("Staged"), 0644); err != nil {
		return
	}
	// }

	return
}

func (b *GitBackend) Commit() (err error) {
	return KinError{"not yet"}
}

func (b *GitBackend) Add(oids []string) (err error) {
	if status, err := b.GetStatus(); status == Committed {
		return KinError{"can't add when status is committed. Create a staged commit first"}
	} else if err != nil {
		return err
	}
	return KinError{"not yet"}
}

func (b *GitBackend) Remove(oids []string) (err error) {
	if status, err := b.GetStatus(); status == Committed {
		return KinError{"can't remove when status is committed. Create a staged commit first"}
	} else if err != nil {
		return err
	}
	return KinError{"not yet"}
}

func (b *GitBackend) Diff(objrefs []string) (string, error) {
	return "", KinError{"not yet"}
}
