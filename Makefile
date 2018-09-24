all: bin/sha1 bin/base64

bin/sha1: src/exec_sha1.c src/sha1.c src/utils.c
	gcc -g -O0 src/exec_sha1.c src/sha1.c src/utils.c -o bin/sha1

bin/base64: src/exec_base64.c src/base64.c src/utils.c
	gcc -g -O0 src/exec_base64.c src/base64.c src/utils.c -o bin/base64

.PHONY: clean

clean:
	rm bin/*
