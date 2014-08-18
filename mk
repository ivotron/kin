#!/usr/bin/env bash

function deps {
echo "Fetching dependencies to $GOPATH..."
printf "   (00/04)\r"
  go get -u github.com/stretchr/testify
printf "## (01/04)\r"
  go get -u github.com/libgit2/git2go
printf "## (02/04)\r"
  go get -u github.com/spf13/cobra
printf "## (03/04)\r"
  go get -u github.com/dchest/uniuri
printf "## (04/04)\r"
printf "\n"
}

function build {
  go build ./...
}

function install {
  go install ./...
}

function test {
  go test ./...
}

$1
