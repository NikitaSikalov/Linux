#!/bin/bash

BIN_DIR="$(pwd)/fs/bin"
FILE_STORE_NAME="fs_store"
FILE_STORE="$BIN_DIR/$FILE_STORE_NAME"
INIT_PROG="$BIN_DIR/init"

if [ -e $FILE_STORE ]; then
  rm $FILE_STORE
fi

touch $FILE_STORE
if [ -e $INIT_PROG ]; then
  export FILE_STORE=$FILE_STORE
  $INIT_PROG
else
  echo "Error, no executable file $INIT_PROG, check Makefile"
  exit 1
fi