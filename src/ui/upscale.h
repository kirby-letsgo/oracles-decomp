#pragma once
// The pixel-art scalers behind the XBRZ and HQX screen filters, wrapped so filter.c stays C
// (xBRZ is C++) and so both take the same picture in and out.
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// src: src_w x src_h RGB24, already colour-corrected. dst: (src_w*scale) x (src_h*scale) RGB24.
// `factor` is how far the scaler itself enlarges the picture; the rest of the way to `scale` is
// whole pixels, so a window scale that is a multiple of the factor stays on the game's pixel grid.
// The factor is clamped to what each scaler supports (xBRZ 2-6, hqx 2-3) and never exceeds `scale`.
void ui_xbrz_scale(const uint8_t *src, int src_w, int src_h, int factor, int scale, uint8_t *dst);
void ui_hqx_scale(const uint8_t *src, int src_w, int src_h, int factor, int scale, uint8_t *dst);

#ifdef __cplusplus
}
#endif
