#include "unit.h"
#include "core/sha1.h"
#include "core/hash.h"

static void sha1_abc(void) {
  char hex[41];
  sha1_hex((const unsigned char *)"abc", 3, hex);
  ASSERT(strcmp(hex, "a9993e364706816aba3e25717850c26c9cd0d89d") == 0);
}
static void sha1_empty(void) {
  char hex[41];
  sha1_hex((const unsigned char *)"", 0, hex);
  ASSERT(strcmp(hex, "da39a3ee5e6b4b0d3255bfef95601890afd80709") == 0);
}
static void fnv_known(void) {
  ASSERT(fnv1a64_update(FNV1A64_INIT, (const unsigned char *)"a", 1) == 0xaf63dc4c8601ec8cULL);
}
int main(void) { RUN(sha1_abc); RUN(sha1_empty); RUN(fnv_known); return 0; }
