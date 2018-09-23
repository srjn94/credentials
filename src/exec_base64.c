#include "base64.h"
#include "utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

bool dflag = false;

void encode_words(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    size_t n_ascii = strlen(argv[i]);
    size_t n_base64 = base64_encode_size(n_ascii);
    char buf[n_base64 + 1];
    base64_encode(buf, argv[i], n_ascii);
    printf("%s\n", buf);
  }
}

void decode_words(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    size_t n_base64 = strlen(argv[i]);
    size_t n_ascii = base64_decode_size(n_base64);
    char buf[n_ascii + 1];
    base64_decode(buf, argv[i]);
    printf("%s\n", buf);
    //fprintf(stderr, "%s: %s is not a valid base64 string\n", argv[0], argv[i]);
  }
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    fprintf(stderr, "usage: %s [-d] arg1 [arg2 ...]\n", argv[0]);
    return ENOARGS;
  }

  int c;
  while ((c = getopt(argc, argv, "d")) != -1) {
    switch (c) {
      case 'd':
        dflag = true;
        break;
      default: return EBADOPTS;
    }
  }
  
  clear_opts(&argc, &argv, &optind);

  if (dflag) {
    decode_words(argc, argv);
  } else {
    encode_words(argc, argv);
  }

  return 0;

}
