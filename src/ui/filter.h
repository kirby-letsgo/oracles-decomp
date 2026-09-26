#pragma once
// Screen filters, applied on the CPU at the window's integer scale so every effect lines up with
// the game's pixels: sharp, scanlines, an LCD pixel grid, and a CRT look (scanlines, a slight
// curve, glow and vignette). GBC colour correction mimics the handheld's paler, warmer screen.
#include "ui/settings.h"

// src: UI_W x UI_H RGB24. dst: (UI_W*scale) x (UI_H*scale) RGB24. scale >= 1.
void ui_filter(const uint8_t *src, bool gbc_colours, ScreenFilter filter, int scale, uint8_t *dst);
void ui_gbc_colour(const uint8_t in[3], uint8_t out[3]);
