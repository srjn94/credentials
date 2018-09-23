#include "utils.h"
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>

void clear_opts(int *p_argc, char **p_argv[], int *p_optind) {
  (*p_argv)[*p_optind - 1] = (*p_argv)[0]; \
  *p_argv += *p_optind - 1; \
  *p_argc -= *p_optind - 1;
  *p_optind = 1;
}

void print_words(const uint32_t words[const], size_t n) {
  for (size_t i = 0; i < n; i++) {
    printf("%08x", words[i]);
  }
}

void print_bytes(const uint8_t bytes[const], size_t n) {
  for (size_t i = 0; i < n; i++) {
    printf("%02x", bytes[i]);
  }
}

void print_bits(const void *p, size_t n) {
  uint8_t *bp = (uint8_t *) p;
  size_t count = 0;
  while (true) {
    uint8_t byte = *(bp++);
    for (size_t i = 0; i < CHAR_BIT; i++) {
      if (count++ == n) {
        return;
      }
      putchar(0x80 & byte ? '1' : '0');
      byte <<= 1;
    }
    if (count < n) putchar(' ');
  }
}

uint32_t choose(uint32_t b, uint32_t c, uint32_t d) {
  return (b & c) | ((~b) & d);
}

uint32_t parity(uint32_t b, uint32_t c, uint32_t d) {
  return b ^ c ^ d;
}

uint32_t majority(uint32_t b, uint32_t c, uint32_t d) {
  return (b & c) | (b & d) | (c & d);
}


