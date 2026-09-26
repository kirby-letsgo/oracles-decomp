#pragma once
// SHA-256 (FIPS 180-4), for comparing synced files with the server's hashes.
#include <stddef.h>
#include <stdint.h>

void sha256(const uint8_t *data, size_t n, uint8_t out[32]);
void sha256_hex(const uint8_t *data, size_t n, char out[65]);
