package kin

type KinError struct {
	Msg string
}

func (e KinError) Error() string {
	return "diversion: " + e.Msg
}
