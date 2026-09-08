#ifndef PNG_H
#define PNG_H
#include <stdbool.h>
#include <stdint.h>
bool png_write_rgb(const char *path, const uint8_t *rgb, int w, int h);
#endif
