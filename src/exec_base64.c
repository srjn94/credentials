#include "base64.h"
#include "utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define INITIAL_BUFSIZE 2

bool dflag = false;

int encode_word(char *arg) {
  size_t n_ascii = strlen(arg);
  size_t n_base64 = base64_encode_size(n_ascii);
  char buf[n_base64 + 1];
  base64_encode(buf, arg, n_ascii);
  printf("%s\n", buf);
  return 0;
}

int decode_word(char *arg) {
  size_t n_base64 = strlen(arg);
  size_t n_ascii = base64_decode_size(n_base64);
  char buf[n_ascii + 1];
  base64_decode(buf, arg);
  printf("%s\n", buf);
  return 0;
}

int job_on_stream(FILE *fp, int(*job)(char*)) {
  char *str = strread(fp);
  if (str == NULL) {
    return -1;
  }
  int status = job(str);
  free(str);
  return status;
}

int encode_stream(FILE *fp) {
  return job_on_stream(fp, encode_word);
}

int decode_stream(FILE *fp) {
  return job_on_stream(fp, decode_word);
}

void encode_words(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    encode_word(argv[i]);
  }
}

void decode_words(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    decode_word(argv[i]);
  }
}

int main(int argc, char *argv[]) {
  int c;
  while ((c = getopt(argc, argv, "hd")) != -1) {
    switch (c) {
      case 'h':
        putchar('\n');
        printf("%s: encode/decode args (or from stdin if no args) to base64.\n",
          argv[0]);
        printf("usage: %s [-d] [arg1 arg2 ...]\n", argv[0]);
        printf("-d: decode (default encode)\n");
        putchar('\n');
        return 0;
      case 'd':
        dflag = true;
        break;
      default: return EBADOPTS;
    }
  }
  
  clear_opts(&argc, &argv, &optind);
  
  if (argc == 1) {
    if (dflag && decode_stream(stdin) == -1) {
      fprintf(stderr, "%s: could not decode from base64\n", argv[0]);
      return EBADSTREAM;
    } else if (encode_stream(stdin) == -1) {
      fprintf(stderr, "%s: could not encode to base64\n", argv[0]);
      return EBADSTREAM;
    }
  } else if (dflag) {
    decode_words(argc, argv);
  } else {
    encode_words(argc, argv);
  }

  return 0;

}
