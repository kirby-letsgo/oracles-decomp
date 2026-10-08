#include "ui/upscale.h"
#include "xbrz.h"
extern "C" {
#include "hqx.h"
}
#include <vector>

// Both scalers run at a small factor and the rest of the enlargement repeats whole pixels, rather
// than scaling all the way up to the window: the Game Boy picture is only 160x144, and taking it 8x
// through xBRZ rounds the art into blobs -- small glyphs like the (R) in the Capcom logo lose their
// shape. At 3x the jagged diagonals are rounded off and everything else is left as the artists drew
// it, and it is several times less work per frame.

// Buffers live between frames: the picture is the same size every time, so this allocates once.
static std::vector<uint32_t> in_argb, out_argb;

static int clamp_factor(int factor, int scale, int most) {
  if (factor < 2) factor = 2;
  if (factor > most) factor = most;
  // Never scale past the window and sample back down -- that throws away what the scaler worked out.
  return factor > scale ? scale : factor;
}

static void to_argb(const uint8_t *src, size_t n) {
  in_argb.resize(n);
  for (size_t i = 0; i < n; i++)
    in_argb[i] = (uint32_t)src[i * 3] << 16 | (uint32_t)src[i * 3 + 1] << 8 | src[i * 3 + 2];
}

// Back to RGB24, enlarging by scale/factor on the way (a straight copy when the two are equal).
static void from_argb(int src_w, int src_h, int factor, int scale, uint8_t *dst) {
  const int mid_w = src_w * factor;
  const int dst_w = src_w * scale, dst_h = src_h * scale;
  for (int y = 0; y < dst_h; y++) {
    const uint32_t *row = out_argb.data() + (size_t)(y * factor / scale) * mid_w;
    uint8_t *out = dst + (size_t)y * dst_w * 3;
    for (int x = 0; x < dst_w; x++) {
      const uint32_t px = row[x * factor / scale];
      out[x * 3] = (uint8_t)(px >> 16);
      out[x * 3 + 1] = (uint8_t)(px >> 8);
      out[x * 3 + 2] = (uint8_t)px;
    }
  }
}

void ui_xbrz_scale(const uint8_t *src, int src_w, int src_h, int factor, int scale, uint8_t *dst) {
  factor = clamp_factor(factor, scale, 6);                  // xBRZ scales by 2 to 6
  to_argb(src, (size_t)src_w * src_h);
  out_argb.resize((size_t)src_w * factor * src_h * factor);
  xbrz::scale((size_t)factor, in_argb.data(), out_argb.data(), src_w, src_h, xbrz::ColorFormat::RGB);
  from_argb(src_w, src_h, factor, scale, dst);
}

void ui_hqx_scale(const uint8_t *src, int src_w, int src_h, int factor, int scale, uint8_t *dst) {
  factor = clamp_factor(factor, scale, 3);                  // hq2x and hq3x are the vendored ones
  to_argb(src, (size_t)src_w * src_h);
  out_argb.resize((size_t)src_w * factor * src_h * factor);
  if (factor == 2) hq2x_32(in_argb.data(), out_argb.data(), src_w, src_h);
  else hq3x_32(in_argb.data(), out_argb.data(), src_w, src_h);
  from_argb(src_w, src_h, factor, scale, dst);
}
