#pragma once
// On-screen controls for touch screens. Everything is measured in game pixels (one game pixel is
// `scale` screen pixels), so the controls are drawn on the same grid as the game.
#include "ui/ui.h"
#include "ui/controls.h"

typedef struct { int x, y, w, h; } UiRect;

typedef struct {
  int screen_w, screen_h;   // screen pixels
  int scale;                // screen pixels per game pixel
  int w, h;                 // the screen in game pixels, rounded up
  bool overlay, portrait;
  UiRect game;              // the picture, in game pixels (rounded out when fill scales it)
  int pic_w;                // its width in game pixels
  UiRect game_px;           // the picture on the screen, in screen pixels
  UiRect dpad;              // the four directions
  UiRect button[ACTIONS];   // A, B, Start, Select, Pause, Fast, Item X/Y; w == 0 when not shown
} TouchLayout;

// safe: the part of the screen not under notches or system bars, in screen pixels.
// Without the overlay the game is centred at the largest whole scale (fill: the largest scale). In portrait the game sits at
// the top and the controls fill the space below; in landscape they sit at its sides.
// pic_w: the picture's width in game pixels (160, or 256 in widescreen).
void touch_layout(TouchLayout *l, int screen_w, int screen_h, UiRect safe, bool overlay, bool item_buttons, bool fill, int pic_w);
// The actions (1u << Action) a finger at (x, y) presses, in game pixels. The D-pad gives diagonals.
uint32_t touch_hit(const TouchLayout *l, float x, float y);
// Where a finger presses the action, in game pixels; false when the layout does not show it.
bool touch_point(const TouchLayout *l, Action a, float *x, float *y);
// Draws the controls into an RGBA image of l->w x l->h, transparent elsewhere; held actions are lit.
void touch_draw(const TouchLayout *l, const UiFont *font, const UiTheme *t, uint32_t held, uint8_t *rgba);
