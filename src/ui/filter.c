#include "ui/filter.h"
#include <math.h>
#include <string.h>

// A common approximation of the GBC LCD's response: channels mix a little and the image is
// compressed towards a warm mid-tone.
void ui_gbc_colour(const uint8_t in[3], uint8_t out[3]) {
  int r = in[0], g = in[1], b = in[2];
  int R = (r * 26 + g * 4 + b * 2) / 32, G = (g * 24 + b * 8) / 32, B = (r * 6 + g * 4 + b * 22) / 32;
  out[0] = (uint8_t)(R * 7 / 8 + 24);
  out[1] = (uint8_t)(G * 7 / 8 + 24);
  out[2] = (uint8_t)(B * 7 / 8 + 20);
}

static void source_pixel(const uint8_t *src, bool gbc, int x, int y, uint8_t out[3]) {
  const uint8_t *p = src + (y * UI_W + x) * 3;
  if (gbc) ui_gbc_colour(p, out);
  else memcpy(out, p, 3);
}

static uint8_t shade(uint8_t v, int num, int den) { return (uint8_t)(v * num / den); }

void ui_filter(const uint8_t *src, bool gbc, ScreenFilter filter, int scale, uint8_t *dst) {
  int w = UI_W * scale, h = UI_H * scale;
  if (filter != FILTER_CRT) {
    for (int y = 0; y < UI_H; y++)
      for (int x = 0; x < UI_W; x++) {
        uint8_t px[3];
        source_pixel(src, gbc, x, y, px);
        for (int sy = 0; sy < scale; sy++)
          for (int sx = 0; sx < scale; sx++) {
            uint8_t *d = dst + ((y * scale + sy) * w + x * scale + sx) * 3;
            int num = 1, den = 1;
            if (scale >= 2 && filter == FILTER_SCANLINES && sy == scale - 1) { num = 1; den = 2; }
            if (scale >= 3 && filter == FILTER_LCD && (sy == scale - 1 || sx == scale - 1)) { num = 3; den = 4; }
            for (int k = 0; k < 3; k++) d[k] = shade(px[k], num, den);
          }
      }
    return;
  }
  // CRT: barrel-distort the screen, brighten the centre of each scanline, add a soft vignette
  const float curve = 0.06f;
  for (int y = 0; y < h; y++) {
    float ny = (y + 0.5f) / h * 2 - 1;
    for (int x = 0; x < w; x++) {
      float nx = (x + 0.5f) / w * 2 - 1;
      float r2 = nx * nx + ny * ny, k = 1 + curve * r2;
      float ux = nx * k, uy = ny * k;
      uint8_t *d = dst + (y * w + x) * 3;
      if (ux < -1 || ux > 1 || uy < -1 || uy > 1) { d[0] = d[1] = d[2] = 0; continue; }
      float fx = (ux + 1) / 2 * UI_W, fy = (uy + 1) / 2 * UI_H;
      int sx = (int)fx, sy = (int)fy;
      if (sx >= UI_W) sx = UI_W - 1;
      if (sy >= UI_H) sy = UI_H - 1;
      uint8_t px[3], nb[3];
      source_pixel(src, gbc, sx, sy, px);
      source_pixel(src, gbc, sx + 1 < UI_W ? sx + 1 : sx, sy, nb);
      float within = fy - sy;                             // 0..1 down the source row
      float line = 0.55f + 0.45f * sinf(within * 3.14159265f);
      float vignette = 1 - 0.35f * r2 * r2;
      for (int c = 0; c < 3; c++) {
        float v = (px[c] * 0.8f + nb[c] * 0.2f) * line * vignette * 1.12f;
        d[c] = (uint8_t)(v > 255 ? 255 : v);
      }
    }
  }
}
