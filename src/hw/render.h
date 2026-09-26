#ifndef RENDER_H
#define RENDER_H
#include <stdint.h>
void framebuffer_to_rgb(const uint16_t *fb, uint8_t *rgb);

// The registers each line of the current frame was drawn with, kept beside the emulator (the GB
// struct's layout is fixed by the recorded boot states) and copied into each frame's sample.
struct GB;
typedef struct {
  uint8_t scx[144], scy[144], lcdc[144], wx[144], wy[144], drawn[144];
  uint8_t vram[2][2][8192], bg_pal[2][64], oam[2][160];   // at the start of the last two LCD frames
  uint8_t current;                           // which of the two the frame being drawn uses
} RenderLineRegs;
RenderLineRegs *render_line_regs(const struct GB *gb);
// The LCD went off: the picture is blank (white) until lines are drawn again.
void render_lines_blank(const struct GB *gb);
#endif
