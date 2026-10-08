#pragma once
// The xBRZ pixel-art scaler behind the XBRZ screen filters, wrapped so filter.c stays C (xBRZ is C++).
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// src: src_w x src_h RGB24, already colour-corrected. dst: (src_w*scale) x (src_h*scale) RGB24.
// `factor` is the strength the player chose (2, 3 or 4); anything beyond it is a whole-pixel
// enlargement, so a window scale that is a multiple of the factor stays on the game's pixel grid.
void ui_xbrz_scale(const uint8_t *src, int src_w, int src_h, int factor, int scale, uint8_t *dst);

#ifdef __cplusplus
}
#endif
