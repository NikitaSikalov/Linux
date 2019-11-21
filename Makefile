init: src/initialize.c
	gcc src/initialize.c -o bin/initProg.o

build: src/main.c src/define.h src/fs_base.h src/operations.h
	gcc src/main.c -o bin/mainProg.o