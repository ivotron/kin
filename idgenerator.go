package kin

import (
	"crypto/sha1"
	"encoding/hex"
	"io"
	"os/user"
	"strconv"

	"github.com/dchest/uniuri"
)

type KinIDGenerationError struct {
	Msg string
}

func (e KinIDGenerationError) Error() string {
	return "kin: " + e.Msg
}

func generateSequential(parent string) (id string, err error) {
	if i, err := strconv.Atoi(parent); err != nil {
		return "", err
	} else {
		id = strconv.Itoa(i + 1)
	}

	return
}
func generateSHA1(parent string) (id string, err error) {
	id = ""

	usr, err := user.Current()
	if err != nil {
		return
	}

	// add entropy
	s := uniuri.New()

	// maybe s would be enough, but just in case, we add more
	payload := parent + s + usr.Uid + usr.Gid + usr.Username + usr.Name + usr.HomeDir

	h := sha1.New()

	if cnt, err := io.WriteString(h, payload); cnt != len(payload) {
		return "", KinIDGenerationError{"can't write payload for SHA1"}
	} else if err != nil {
		return "", err
	}

	id = hex.EncodeToString(h.Sum(nil))

	return
}

// Generate creates a version identifier using the specified method
func GenerateID(parent string, method VersionIdGenerationMethod) (id string, err error) {
	id = ""

	switch method {
	case Sequential:
		id, err = generateSequential(parent)
	case SHA1:
		id, err = generateSHA1(parent)
	default:
		err = KinIDGenerationError{"unknown generation method " + string(method)}
	}

	return
}
