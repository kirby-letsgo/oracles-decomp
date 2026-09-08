#ifndef UNIT_H
#define UNIT_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ASSERT(c) do { if (!(c)) { \
  fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); exit(1); } } while (0)
#define ASSERT_EQ(a, b) do { long long _a = (long long)(a), _b = (long long)(b); if (_a != _b) { \
  fprintf(stderr, "FAIL %s:%d: %s == %s (got %lld, want %lld)\n", __FILE__, __LINE__, #a, #b, _a, _b); \
  exit(1); } } while (0)
#define RUN(fn) do { fn(); printf("ok %s\n", #fn); } while (0)
#define SKIP(msg) do { printf("skip: %s\n", msg); exit(77); } while (0)

static inline unsigned char *read_file(const char *path, size_t *size) {
  FILE *f = fopen(path, "rb");
  if (!f) return NULL;
  fseek(f, 0, SEEK_END);
  long n = ftell(f);
  fseek(f, 0, SEEK_SET);
  unsigned char *buf = malloc(n);
  if (fread(buf, 1, n, f) != (size_t)n) { free(buf); fclose(f); return NULL; }
  fclose(f);
  *size = n;
  return buf;
}
#endif
