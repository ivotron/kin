package kin

import (
	"io/ioutil"
	"os"
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
	gitIgnore := []byte(".staged\n")
	oid, err := b.gitRepo.CreateBlobFromBuffer(gitIgnore)
	if err != nil {
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
func (b *GitBackend) GetStatus() Status {
	return Committed
}

func checkoutWalk(r *git.Repository, d string, t *git.Tree) (oids []string, err error) {

	for i := uint64(0); i < t.EntryCount(); i++ {
		te := t.EntryByIndex(i)
		if te == nil {
			return nil, KinError{"no entry in tree for given index "}
		}

		switch te.Type {

		case git.ObjectTree:
			subtree, err := r.LookupTree(te.Id)
			if err != nil {
				return nil, err
			}

			suboids, err := checkoutWalk(r, d+"/"+te.Name, subtree)
			if err != nil {
				return nil, err
			}
			oids = append(oids, suboids...)

		case git.ObjectBlob:
			oids = append(oids, d+"/"+te.Name)

		default:
			err = KinError{"unexpected object of type"}
		}
	}
	return
}

func (b *GitBackend) Checkout(ref string) (stagedId string, oids []string, err error) {
	if b.gitRepo == nil {
		return "", nil, KinError{"nil pointer to git repo"}
	}
	if ref != "HEAD" {
		return "", nil, KinError{"only support HEAD"}
	}
	// get objects inside commit {
	obj, err := b.gitRepo.RevparseSingle(ref)
	if err != nil {
		return
	}
	if obj.Type() != git.ObjectCommit {
		return "", nil, KinError{"error, expecing commit object for given reference"}
	}

	commit, err := b.gitRepo.LookupCommit(obj.Id())
	if err != nil {
		return
	}

	tree, err := commit.Tree()
	if err != nil {
		return
	}

	if oids, err = checkoutWalk(b.gitRepo, "", tree); err != nil {
		return
	}
	// }

	// get an ID to associate the staged changes {
	parentId := commit.ParentId(0)
	if parentId == nil {
		// if no parents (first commit), then assign a fictitious one
		parentId = git.NewOidFromBytes([]byte("12345678901234567890"))
	}
	stagedId, err = GenerateID(parentId.String(), SHA1)
	if err != nil {
		return
	}

	err = ioutil.WriteFile(b.path+"/.staged", []byte(stagedId), 0644)
	if err != nil {
		return
	}
	// }

	// checkout working tree (will include logic for branches and detached HEADs later) {
	err = b.gitRepo.CheckoutHead(&git.CheckoutOpts{Strategy: git.CheckoutForce})
	// }

	return
}

func (b *GitBackend) Commit() (err error) {
	return KinError{"not yet"}
}

func (b *GitBackend) CheckoutObjects(commit string, oids []string) (err error) {
	return KinError{"not supported"}
}

func (b *GitBackend) Add(oids []string) (err error) {
	if b.GetStatus() == Committed {
		return KinError{"can't add when status is committed. Create a staged commit first"}
	}
	return KinError{"not yet"}
}

func (b *GitBackend) Remove(oids []string) (err error) {
	if b.GetStatus() == Committed {
		return KinError{"can't remove when status is committed. Create a staged commit first"}
	}
	return KinError{"not yet"}
}

func (b *GitBackend) Diff(objrefs []string) (string, error) {
	return "", KinError{"not yet"}
}
