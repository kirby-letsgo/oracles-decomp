#pragma once
// The hardware model: timer, PPU, APU, DMA and the IO register file, ticked by gb_tick. It knows
// no opcodes; both the interpreter and the native engine drive it through the same calls.
#include "core/gb.h"

uint8_t io_read(GB *gb, uint8_t r);
void io_write(GB *gb, uint8_t r, uint8_t v);
void oam_dma(GB *gb, uint8_t page);
void hdma5_write(GB *gb, uint8_t v);
void bus_hdma_chunk(GB *gb);
