#!/bin/bash

BIN_DIR="$(pwd)/fs/bin"
MAIN="$BIN_DIR/server"
FILE_STORE_NAME="fs_store"
FILE_STORE="$BIN_DIR/$FILE_STORE_NAME"

if [ ! -e "$FILE_STORE" ]; then
  echo "Error, file store $FILE_STORE does not exist. It seems that you do not run init.sh or check init.sh"
  exit 1
fi

if [ -n "$(ps -d | grep server)" ]; then
  ps -d | grep server | awk '{print $1}' | xargs kill
fi

if [ -e "$MAIN" ]; then
  export FILE_STORE=$FILE_STORE
  $MAIN
else
  echo "Error, no executable file $MAIN. It seems that you do not run buld.sh"
  exit 1
fi
