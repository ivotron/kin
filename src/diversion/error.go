package diversion

type DiversionError struct {
	Msg string
}

func (e DiversionError) Error() string {
	return "diversion: " + e.Msg
}
