#ifndef SHA1_H
#define SHA1_H
#include <stddef.h>
void sha1(const unsigned char *data, size_t len, unsigned char out[20]);
void sha1_hex(const unsigned char *data, size_t len, char out[41]);
#endif
