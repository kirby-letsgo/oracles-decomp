#ifndef HASH_H
#define HASH_H
#include <stddef.h>
#include <stdint.h>
#define FNV1A64_INIT 0xcbf29ce484222325ULL
uint64_t fnv1a64_update(uint64_t h, const unsigned char *data, size_t len);
#endif
