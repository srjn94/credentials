#ifndef _sha1_h_
#define _sha1_h_

#include <stdio.h>
#include <stdlib.h>

#define SHA1_BLOCK_BYTES 64
#define SHA1_HASH_BYTES 20
#define SHA1_BLOCK_WORDS SHA1_BLOCK_BYTES / sizeof(uint32_t)
#define SHA1_HASH_WORDS SHA1_HASH_BYTES / sizeof(uint32_t)

/* BYTES = big endian, WORDS = little endian (on my computer) */
enum sha1_mode {BYTES, WORDS};

/* data that can be divided into bytes or 4-byte words */
union sha1_block_data {
  uint8_t bytes[SHA1_BLOCK_BYTES];
  uint32_t words[SHA1_BLOCK_WORDS];
};

union sha1_hash_data {
  uint8_t bytes[SHA1_HASH_BYTES];
  uint32_t words[SHA1_HASH_WORDS];
};

/* 64-byte block: a buffer for digestion */
struct sha1_block {
  union sha1_block_data data;
  enum sha1_mode mode;
};
void sha1_block_init(struct sha1_block *bp);
void sha1_block_clear(struct sha1_block *bp);
void sha1_block_tobytes(struct sha1_block *bp);
void sha1_block_towords(struct sha1_block *bp);

/* 20-byte output hash */
struct sha1_hash {
  union sha1_hash_data data;
  enum sha1_mode mode;
};
void sha1_hash_init(struct sha1_hash *hp);
void sha1_hash_tobytes(struct sha1_hash *hp);
void sha1_hash_towords(struct sha1_hash *hp);
void sha1_hash_print(const struct sha1_hash *hp);
void sha1_hash_println(const struct sha1_hash *hp);

/* Holds the hash so far, and the number of bytes digested so far */
struct sha1_session {
  struct sha1_hash hash; // the hash so far
  struct sha1_block blockbuf;
  size_t bytes_read;
};

size_t sha1_bufpos(const struct sha1_session *sp);
size_t sha1_bufspace(const struct sha1_session *sp);
void sha1_init(struct sha1_session *sp);
void sha1_digest(struct sha1_session *sp, const void *bytes, size_t n);
ssize_t sha1_sdigest(struct sha1_session *sp, const char *str);
ssize_t sha1_fdigest(struct sha1_session *sp, FILE *fp);
void sha1_hash(struct sha1_hash *hp, const struct sha1_session *sp);
void sha1_print(const struct sha1_session *sp);

#endif
