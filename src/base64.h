#ifndef _base64_h_
#define _base64_h_

#include <stdbool.h>
#include <unistd.h>

bool isbase64(const char *str);
size_t base64_encode_size(size_t n);
size_t base64_decode_size(size_t n);
void base64_encode(char *output, const void *input, size_t n);
void base64_decode(void *output, const char *input);

#endif
