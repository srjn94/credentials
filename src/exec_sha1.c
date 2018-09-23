#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sha1.h"
#include "utils.h"

bool fflag = false;
bool vflag = false; 

int print_hashed_stream(FILE *fp) {
  struct sha1_session session;
  struct sha1_hash hash;
  sha1_init(&session);
  ssize_t nbytes = sha1_fdigest(&session, fp);
  if (nbytes == -1) {
    return -1;
  }  
  sha1_hash(&hash, &session);
  if (vflag) printf("(%zu bytes) -> ", (size_t) nbytes); 
  sha1_hash_println(&hash); 
  return 0;
}

int hash_files(int argc, char *argv[]) {
  bool success = true;
  for (int i = 1; i < argc; i++) {
    FILE *fp = fopen(argv[i], "r");
    if (fp == NULL) {
      fprintf(stderr, "%s: could not open %s: %s\n", argv[0], argv[i], strerror(errno));
    } else if (print_hashed_stream(fp) == -1) {
      fprintf(stderr, "%s: trouble reading %s: %s\n", argv[0], argv[i], strerror(errno));
    } else success = false;
    fclose(fp);
  }
  return success ? 0 : -1;
}

int hash_strings(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    struct sha1_session session;
    struct sha1_hash hash;
    sha1_init(&session);
    size_t len = sha1_sdigest(&session, argv[i]);
    sha1_hash(&hash, &session);
    if (vflag) {
      printf("%.10s", argv[i]);
      if (len > 10) printf("...");   
      printf(" -> "); 
    }
    sha1_hash_println(&hash);
  }
  return 0;
}

int main(int argc, char *argv[]) {
 
  int c;
  while ((c = getopt(argc, argv, "fv")) != -1) {
    switch (c) {
      case 'f':
        fflag = true;
        break;
      case 'v':
        vflag = true;
        break;
      default: return EBADOPTS;
    }
  }

  clear_opts(&argc, &argv, &optind);

  if (argc == 1) {
    if (fflag) {
      fprintf(stderr, "%s: cannot choose '-f' w/o argument(s)\n", argv[0]);
      return EBADOPTCOMB;
    }
    if (print_hashed_stream(stdin) == -1) {
      fprintf(stderr, "%s: could not read input\n", argv[0]);
    }
  }

  if (fflag) return hash_files(argc, argv);
  return hash_strings(argc, argv);

}
