#include "ui/xbrz_filter.h"
#include "xbrz.h"
#include <cstdlib>
#include <vector>

// The player picks the factor (XBRZ2/3/4) rather than the filter running all the way up to the
// window: the Game Boy picture is only 160x144, and scaling it 8x with xBRZ rounds the art into
// blobs -- small glyphs like the (R) in the Capcom logo lose their shape. 2x rounds off the jagged
// diagonals and leaves everything else as the artists drew it; 4x is there for anyone who wants
// the softer look. Whatever enlargement is left is done by repeating whole pixels.
static int clamp_factor(int factor, int scale) {
  if (factor < 2) factor = 2;
  if (factor > 6) factor = 6;                 // xBRZ scales by 2 to 6
  // Never scale past the window and sample back down -- that throws away what xBRZ just worked out.
  return factor > scale ? scale : factor;
}

// Buffers live between frames: the picture is the same size every time, so this allocates once.
static std::vector<uint32_t> in_argb, out_argb;

void ui_xbrz_scale(const uint8_t *src, int src_w, int src_h, int factor, int scale, uint8_t *dst) {
  factor = clamp_factor(factor, scale);
  const int mid_w = src_w * factor, mid_h = src_h * factor;
  in_argb.resize((size_t)src_w * src_h);
  out_argb.resize((size_t)mid_w * mid_h);

  for (size_t i = 0, n = in_argb.size(); i < n; i++)
    in_argb[i] = (uint32_t)src[i * 3] << 16 | (uint32_t)src[i * 3 + 1] << 8 | src[i * 3 + 2];

  xbrz::scale((size_t)factor, in_argb.data(), out_argb.data(), src_w, src_h, xbrz::ColorFormat::RGB);

  // Back to RGB24, enlarging by scale/factor on the way (a no-op when the two are equal).
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
