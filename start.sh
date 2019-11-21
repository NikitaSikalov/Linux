#!/bin/bash

BIN_DIR="bin"
MAIN="$BIN_DIR/mainProg.o"
FILE_STORE_NAME="fs_store"
FILE_STORE="$BIN_DIR/$FILE_STORE_NAME"

if [ ! -e $FILE_STORE ]; then
  echo "Error, file store $FILE_STORE does not exist. It seems that you do not run init.sh or check init.sh"
  exit 1
fi

make build
if [ -e $MAIN ]; then
  export FILE_STORE=$FILE_STORE_NAME
  './'$MAIN
else
  echo "Error, no executable file $MAKE, check Makefile"
  exit 1
fi
