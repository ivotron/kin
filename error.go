package diversion

import "fmt"

type DiversosError string

func (e DiversosError) Error() string {
	return fmt.Sprintf("diversion: %s", e)
}
