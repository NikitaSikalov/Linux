#!/bin/bash

BIN_DIR="$(pwd)/bin"
MAIN="$BIN_DIR/server"
FILE_STORE_NAME="fs_store"
FILE_STORE="$BIN_DIR/$FILE_STORE_NAME"

if [ ! -e $FILE_STORE ]; then
  echo "Error, file store $FILE_STORE does not exist. It seems that you do not run init.sh or check init.sh"
  exit 1
fi

ps -d | grep server | awk '{print $1}' | xargs kill
if [ -e $MAIN ]; then
  export FILE_STORE=$FILE_STORE
  $MAIN
else
  echo "Error, no executable file $MAKE, check Makefile"
  exit 1
fi
