package kin

type KinError struct {
	Msg string
}

func (e KinError) Error() string {
	return "kin: " + e.Msg
}
