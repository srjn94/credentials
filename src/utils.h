#ifndef _utils_h_
#define _utils_h_

#include <stdio.h>
#include <stdlib.h>

#define ENOARGS -1
#define EBADOPTS -2
#define EBADOPTCOMB -3
#define EBADSTREAM -4

void clear_opts(int *p_argc, char **p_argv[], int *p_optind);
void print_words(const uint32_t words[const], size_t n);
void print_bytes(const uint8_t bytes[const], size_t n);
void print_bits(const void *p, size_t nbytes);
uint32_t choose(uint32_t b, uint32_t c, uint32_t d);
uint32_t parity(uint32_t b, uint32_t c, uint32_t d);
uint32_t majority(uint32_t b, uint32_t c, uint32_t d);
char *strread(FILE *fp);

#endif
