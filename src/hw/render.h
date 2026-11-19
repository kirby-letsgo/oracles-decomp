#ifndef RENDER_H
#define RENDER_H
#include <stdint.h>
#include "core/gb.h"
// A frontend supplies storage for the last completed LCD picture on its emulation thread.
// Pass NULL for both arguments when the game ends. Movie samples keep their own timing.
void gb_set_display(const GB *gb, GBSample *out);
void gb_blank_display(const GB *gb);
void gb_publish_display(const GB *gb);
void framebuffer_to_rgb(const uint16_t *fb, uint8_t *rgb);

// The registers each line of the current frame was drawn with, kept beside the emulator (the GB
// struct's layout is fixed by the recorded boot states) and copied into each frame's sample.
struct GB;
typedef struct {
  uint8_t scx[144], scy[144], lcdc[144], wx[144], wy[144], drawn[144];
  uint8_t vram[2][2][8192], bg_pal[2][64], oam[2][160];   // at the start of the last two LCD frames
  uint8_t current;                           // which of the two the frame being drawn uses
  // the engine's off-screen sprites: built with the OAM buffer, taken by the OAM DMA that copies
  // exactly that buffer, then kept per LCD frame like oam
  OffscreenSprites offscreen_pending, offscreen_dma, offscreen[2];
  uint8_t pending_oam[160];
  // a copy of the game standing in a neighbouring room (wide/npcs.c): the list holds every sprite
  // of the room's own objects, on the screen or not, and none of Link's
  bool offscreen_room_objects;
  // objects of the room being left that the game stopped drawing during this screen transition
  // (one bit per object slot), still drawn past the screen's edges until it ends
  uint64_t offscreen_hidden;
  // enemies the room's object list placed at a random spot: a copy leaves them out, since Link
  // entering would find them elsewhere
  uint64_t offscreen_random;
} RenderLineRegs;
RenderLineRegs *render_line_regs(const struct GB *gb);
// The LCD went off: the picture is blank (white) until lines are drawn again.
void render_lines_blank(const struct GB *gb);
#endif
