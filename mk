#!/usr/bin/env bash

function deps {
echo "Fetching dependencies to $GOPATH..."
printf "   (00/03)\r"
  go get -u github.com/stretchr/testify
printf "## (01/03)\r"
  go get -u github.com/libgit2/git2go
printf "## (02/03)\r"
  go get -u github.com/spf13/cobra
printf "## (03/03)\r"
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
