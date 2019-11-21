#!/bin/bash

BIN_DIR="bin"
FILE_STORE="$BIN_DIR/fs_store"
INIT_PROG="$BIN_DIR/initProg.o"

make init
if [ -e $FILE_STORE ]; then
  rm $FILE_STORE
fi

if [ ! -e $BIN_DIR ]; then
  mkdir $BIN_DIR
fi
touch $FILE_STORE
if [ -e $INIT_PROG ]; then
  export FILE_STORE=$FILE_STORE
  './'$INIT_PROG
else
  echo "Error, no executable file $INIT_PROG, check Makefile"
  exit 1
fi