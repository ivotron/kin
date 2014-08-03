package version

import (
	"strconv"

	"github.com/ivotron/kin/opts"
)

type KinIDGenerationError struct {
	Msg string
}

func (e KinIDGenerationError) Error() string {
	return "kin: " + e.Msg
}

// Generate creates a version identifier based on the provided method
func Generate(parent string, method opts.VersionIdGenerationMethod) (string, error) {
	switch method {
	case opts.Sequential:
		if i, err := strconv.Atoi(parent); err != nil {
			return "", err
		} else {
			return strconv.Itoa(i + 1), nil
		}

	default:
		return "", KinIDGenerationError{"unknown generation method " + string(method)}
	}
}
