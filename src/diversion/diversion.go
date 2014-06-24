package diversion

import "fmt"

type DiversosError int

func (e DiversosError) Error() string {
	return fmt.Sprintf("versos: ret=%d", e)
}
