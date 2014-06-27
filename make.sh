#!/usr/bin/env bash
set -e

cd "`dirname '$0'`"
SCRIPTPATH="`pwd`"
cd - > /dev/null

export GOPATH=$GOPATH:$SCRIPTPATH
export GOBIN=

function deps {
echo "Fetching dependencies to $SCRIPTPATH..."
printf "   (00/02)\r"
  go get github.com/noahdesu/go-rados
printf "#  (02/02)\r"
  go get github.com/stretchr/testify
printf "## (02/02)\r"
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
