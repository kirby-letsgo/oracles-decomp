#include "ui/filter.h"
#include "ui/upscale.h"
#include <math.h>
#include <string.h>

// The widest picture ui_filter is given: the widescreen 256 (WIDE_W), against the plain UI_W of 160.
// Kept here rather than included so the UI stays independent of the widescreen renderer; a wider
// picture than this simply falls through to the plain path below.
#define FILTER_MAX_SRC_W 256

// A common approximation of the GBC LCD's response: channels mix a little and the image is
// compressed towards a warm mid-tone.
void ui_gbc_colour(const uint8_t in[3], uint8_t out[3]) {
  int r = in[0], g = in[1], b = in[2];
  int R = (r * 26 + g * 4 + b * 2) / 32, G = (g * 24 + b * 8) / 32, B = (r * 6 + g * 4 + b * 22) / 32;
  out[0] = (uint8_t)(R * 7 / 8 + 24);
  out[1] = (uint8_t)(G * 7 / 8 + 24);
  out[2] = (uint8_t)(B * 7 / 8 + 20);
}

static void source_pixel(const uint8_t *src, int src_w, bool gbc, int x, int y, uint8_t out[3]) {
  const uint8_t *p = src + (y * src_w + x) * 3;
  if (gbc) ui_gbc_colour(p, out);
  else memcpy(out, p, 3);
}

static uint8_t shade(uint8_t v, int num, int den) { return (uint8_t)(v * num / den); }

// The source with the Game Boy Color screen's colours already applied, so a filter that reads a
// pixel more than once does not convert it again on every read.
static const uint8_t *corrected_source(const uint8_t *src, int src_w) {
  static uint8_t buf[FILTER_MAX_SRC_W * UI_H * 3];
  for (int i = 0; i < src_w * UI_H; i++) ui_gbc_colour(src + i * 3, buf + i * 3);
  return buf;
}

// The CRT scanline's brightness, 0.55 + 0.45 sin(pi t) across one source row, read with a lerp
// between neighbouring entries. The curve is smooth enough that this agrees with the sinf it
// replaces to far past the precision a byte can hold, and a sinf per output pixel was most of what
// the filter cost.
#define CRT_LINE_TAB 2048
static float crt_line[CRT_LINE_TAB + 1];

static void crt_line_init(void) {
  if (crt_line[0] != 0.0f) return;
  for (int i = 0; i <= CRT_LINE_TAB; i++)
    crt_line[i] = 0.55f + 0.45f * sinf((float)i / CRT_LINE_TAB * 3.14159265f);
}

// How far the pixel-art scalers enlarge the picture themselves; the rest of the way to the window
// is whole pixels. 3 rounds the diagonals off without softening small shapes -- see upscale.cpp.
#define UPSCALE_FACTOR 3

static bool is_upscaler(ScreenFilter f) { return f == FILTER_XBRZ || f == FILTER_HQX; }

void ui_filter(const uint8_t *src, int src_w, bool gbc, ScreenFilter filter, int scale, uint8_t *dst) {
  int w = src_w * scale, h = UI_H * scale;
  if (is_upscaler(filter) && src_w <= FILTER_MAX_SRC_W) {
    // The scalers read whole pixels, so the colour correction has to happen before them rather than
    // after: they decide which pixels count as equal, and it is the shown colours they should compare.
    const uint8_t *in = gbc ? corrected_source(src, src_w) : src;
    if (filter == FILTER_XBRZ) ui_xbrz_scale(in, src_w, UI_H, UPSCALE_FACTOR, scale, dst);
    else ui_hqx_scale(in, src_w, UI_H, UPSCALE_FACTOR, scale, dst);
    return;
  }
  if (filter != FILTER_CRT) {
    for (int y = 0; y < UI_H; y++)
      for (int x = 0; x < src_w; x++) {
        uint8_t px[3];
        source_pixel(src, src_w, gbc, x, y, px);
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
  // CRT: barrel-distort the screen, brighten the centre of each scanline, add a soft vignette.
  // A full-screen window is over two million output pixels, so everything that does not have to be
  // redone for each of them is lifted out: the two divisions become one reciprocal apiece, the
  // screen's colour correction is done once over the 160x144 source instead of twice per output
  // pixel, and the scanline curve comes from crt_line rather than a sinf each.
  crt_line_init();
  const uint8_t *csrc = src;
  bool convert = gbc;
  if (gbc && src_w <= FILTER_MAX_SRC_W) { csrc = corrected_source(src, src_w); convert = false; }
  const float curve = 0.06f;
  const float inv_w = 2.0f / w, inv_h = 2.0f / h;
  const float to_src_x = src_w * 0.5f, to_src_y = UI_H * 0.5f;
  for (int y = 0; y < h; y++) {
    float ny = (y + 0.5f) * inv_h - 1.0f, ny2 = ny * ny;
    uint8_t *row = dst + (size_t)y * w * 3;
    // The source is 160 or 256 wide against an output many times that, so a dozen or so pixels in a
    // row usually come from the same two source pixels. Their blend is held until one of them
    // changes; only the scanline and vignette still vary from pixel to pixel.
    int held_sx = -1, held_sy = -1;
    float blend[3] = {0, 0, 0};
    for (int x = 0; x < w; x++) {
      float nx = (x + 0.5f) * inv_w - 1.0f;
      float r2 = nx * nx + ny2, k = 1 + curve * r2;
      float ux = nx * k, uy = ny * k;
      uint8_t *d = row + x * 3;
      if (ux < -1 || ux > 1 || uy < -1 || uy > 1) { d[0] = d[1] = d[2] = 0; continue; }
      float fx = (ux + 1) * to_src_x, fy = (uy + 1) * to_src_y;
      int sx = (int)fx, sy = (int)fy;
      if (sx >= src_w) sx = src_w - 1;
      if (sy >= UI_H) sy = UI_H - 1;
      if (sx != held_sx || sy != held_sy) {
        held_sx = sx;
        held_sy = sy;
        // The two source pixels are neighbours on one row, so the second is the next three bytes
        // unless this is the last column.
        uint8_t px[3], nb[3];
        const uint8_t *p0, *p1;
        if (convert) {                                    // only a picture too wide to pre-correct
          source_pixel(csrc, src_w, true, sx, sy, px);
          source_pixel(csrc, src_w, true, sx + 1 < src_w ? sx + 1 : sx, sy, nb);
          p0 = px;
          p1 = nb;
        } else {
          p0 = csrc + ((size_t)sy * src_w + sx) * 3;
          p1 = p0 + (sx + 1 < src_w ? 3 : 0);
        }
        for (int c = 0; c < 3; c++) blend[c] = p0[c] * 0.8f + p1[c] * 0.2f;
      }
      // Where this lands down the source row, which is past its end only on the very last line.
      float t = (fy - sy) * CRT_LINE_TAB;
      if (t < 0) t = 0;
      if (t > CRT_LINE_TAB) t = CRT_LINE_TAB;
      int ti = (int)t;
      if (ti >= CRT_LINE_TAB) ti = CRT_LINE_TAB - 1;
      float line = crt_line[ti] + (crt_line[ti + 1] - crt_line[ti]) * (t - ti);
      float gain = line * (1 - 0.35f * r2 * r2) * 1.12f;
      for (int c = 0; c < 3; c++) {
        float v = blend[c] * gain;
        d[c] = (uint8_t)(v > 255 ? 255 : v);
      }
    }
  }
}
