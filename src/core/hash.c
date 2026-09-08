#include "hash.h"
uint64_t fnv1a64_update(uint64_t h, const unsigned char *data, size_t len) {
  for (size_t i = 0; i < len; i++) { h ^= data[i]; h *= 0x100000001b3ULL; }
  return h;
}
