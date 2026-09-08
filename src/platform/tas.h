#ifndef TAS_H
#define TAS_H
#include <stdbool.h>
#include <stdint.h>
typedef struct Tas {
  uint8_t *inputs;
  uint32_t count;
} Tas;
bool tas_load(Tas *t, const char *path);
void tas_free(Tas *t);
uint8_t tas_input_at(const Tas *t, uint64_t frame);
#endif
