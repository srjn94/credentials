all: sha1 base64

sha1: src/exec_sha1.c src/sha1.c src/utils.c
	gcc src/exec_sha1.c src/sha1.c src/utils.c -o bin/sha1

base64: src/exec_base64.c src/base64.c src/utils.c
	gcc src/exec_base64.c src/base64.c src/utils.c -o bin/base64

.PHONY: clean

clean:
	rm bin/*
