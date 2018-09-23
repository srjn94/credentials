#include "sha1.h"
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"

#define ROTATE_LEFT(x, k) \
  (((x) << (k) % 32) | ((x) >> (32 - (k) % 32)))

#define FLIP_ENDIAN(x) \
  ((ROTATE_LEFT((x), 24) & 0xff00ff00) | (ROTATE_LEFT((x), 8) & 0x00ff00ff))

#define TO_TEMPLATE(TO, FROM, p, size) \
  if ((p)->mode == FROM) { \
    for (size_t i = 0; i < size; i++) { \
      (p)->data.words[i] = FLIP_ENDIAN((p)->data.words[i]); \
    } \
    (p)->mode = TO; \
  } 

#define TO_BYTES(p, size) TO_TEMPLATE(BYTES, WORDS, p, size)
#define TO_WORDS(p, size) TO_TEMPLATE(WORDS, BYTES, p, size)

void sha1_mode_print(enum sha1_mode mode) {
  if (mode == BYTES) printf("BYTES");
  if (mode == WORDS) printf("WORDS");
}

void sha1_block_init(struct sha1_block *bp) {
  bzero(&bp->data, sizeof(bp->data));
  bp->mode = BYTES;
}

void sha1_block_clear(struct sha1_block *bp) {
  sha1_block_init(bp);
  bzero(&bp->data, sizeof(bp->data));
}

void sha1_hash_init(struct sha1_hash *hp) {
  static struct sha1_hash h = {
    {.words = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0}},
    WORDS
  };
  memcpy(hp, &h, sizeof(*hp));
}

void sha1_block_tobytes(struct sha1_block *bp) { TO_BYTES(bp, SHA1_BLOCK_WORDS) }
void sha1_block_towords(struct sha1_block *bp) { TO_WORDS(bp, SHA1_BLOCK_WORDS) }
void sha1_hash_tobytes(struct sha1_hash *hp) { TO_BYTES(hp, SHA1_HASH_WORDS) }
void sha1_hash_towords(struct sha1_hash *hp) { TO_WORDS(hp, SHA1_HASH_WORDS) }

void sha1_hash_print(const struct sha1_hash *hp) {
  if (hp->mode==WORDS) print_words((const uint32_t*)hp->data.words,SHA1_HASH_WORDS);
  if (hp->mode==BYTES) print_bytes((const uint8_t*)hp->data.bytes,SHA1_HASH_BYTES);
}

void sha1_hash_println(const struct sha1_hash *hp) {
  sha1_hash_print(hp);
  putchar('\n');
}

size_t sha1_bufpos(const struct sha1_session *sp) {
  return sp->bytes_read % SHA1_BLOCK_BYTES; 
}

size_t sha1_bufspace(const struct sha1_session *sp) {
  return SHA1_BLOCK_BYTES - sha1_bufpos(sp);
}

void sha1_init(struct sha1_session *sp) {
  sha1_hash_init(&sp->hash);
  sha1_block_init(&sp->blockbuf);
  sp->bytes_read = 0;
}

static void sha1_step(struct sha1_hash *hp, const struct sha1_block *bp) {
  static uint32_t (*f[])(uint32_t,uint32_t,uint32_t) = {choose, parity, majority, parity};
  static uint32_t k[] = {0x5a827999, 0x6ed9eba1, 0x8f1bbcdc, 0xca62c1d6};
  uint32_t a, b, c, d, e, temp, w[80];
  
  sha1_hash_towords(hp);
  a = hp->data.words[0];
  b = hp->data.words[1];
  c = hp->data.words[2];
  d = hp->data.words[3];
  e = hp->data.words[4];
  
  for (size_t i = 0; i < 80; i++) {
    if (i < 16) {
      temp = w[i] = FLIP_ENDIAN(bp->data.words[i]);
    } else {
      temp = w[i] = ROTATE_LEFT(w[i-3]^w[i-8]^w[i-14]^w[i-16], 1);
    }
    temp += ROTATE_LEFT(a, 5) + f[i/20](b,c,d) + e + k[i/20];
    e = d;
    d = c;
    c = ROTATE_LEFT(b, 30);
    b = a;
    a = temp;
  }
  
  hp->data.words[0] += a;
  hp->data.words[1] += b;
  hp->data.words[2] += c;
  hp->data.words[3] += d;
  hp->data.words[4] += e;
}

void sha1_digest(struct sha1_session *sp, const void *bytes, size_t n) {
  if (n > sha1_bufspace(sp)) {
    do {
      size_t pos = sha1_bufpos(sp);
      size_t space = sha1_bufspace(sp);
      memcpy(sp->blockbuf.data.bytes + pos, bytes, space);
      sha1_step(&sp->hash, &sp->blockbuf);
      n -= space;
      bytes += space;
      sp->bytes_read += space;
    } while (n >= SHA1_BLOCK_BYTES);
  }

  memcpy(&sp->blockbuf.data.bytes, bytes, n);
  sp->bytes_read += n;
}

ssize_t sha1_sdigest(struct sha1_session *sp, const char *str) {
  size_t len = strlen(str); //TODO enforce length requirement?
  sha1_digest(sp, (uint8_t*) str, len);
  return len;
}

ssize_t sha1_fdigest(struct sha1_session *sp, FILE *fp) {
  struct sha1_session temp;
  memcpy(&temp, sp, sizeof(*sp));
  
  ssize_t out = 0;
  char buf[BUFSIZ];
  while (!feof(fp)) {
    size_t bytes_read = fread(buf, sizeof(char), sizeof(buf), fp);
    if (ferror(fp)) {
      return -1;
    }
    sha1_digest(&temp, (uint8_t *) buf, bytes_read);
    out += bytes_read;
  }

  memcpy(sp, &temp, sizeof(temp));
  return out;
}

void sha1_hash(struct sha1_hash *hp, const struct sha1_session *sp) {
  memcpy(hp, &sp->hash, sizeof(sp->hash));
  struct sha1_block ult;
  sha1_block_clear(&ult);
  if (sha1_bufspace(sp) == SHA1_BLOCK_BYTES) {
    ult.data.bytes[0] = 0x80;
    sha1_step(hp, &sp->blockbuf);
  } else if (sha1_bufspace(sp) < sizeof(uint8_t) + sizeof(size_t)) {
    struct sha1_block penult;
    sha1_block_clear(&penult);
    memcpy(&penult.data, &sp->blockbuf.data, sha1_bufpos(sp));
    sha1_block_tobytes(&penult);
    penult.data.bytes[sha1_bufpos(sp)] = 0x80;
    sha1_step(hp, &penult);
  } else {
    memcpy(&ult.data, &sp->blockbuf.data, sha1_bufpos(sp));
    sha1_block_tobytes(&ult);
    ult.data.bytes[sha1_bufpos(sp)] = 0x80;
  }
  ult.data.words[SHA1_BLOCK_WORDS-1] = FLIP_ENDIAN(CHAR_BIT*sp->bytes_read);
  sha1_step(hp, &ult);
}

void sha1_print(const struct sha1_session *sp) {
  printf("Current hash: "); sha1_hash_print(&sp->hash); putchar('\n');
  printf("Total bytes digested: %zu\n", sp->bytes_read);
  printf("Bytes in blockbuf: %zu\n", sp->bytes_read % SHA1_BLOCK_BYTES);
}


