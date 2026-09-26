// OAM DMA and the HDMA/GDMA engines.
#include "core/bus.h"
#include "hw/hw.h"
#include <stdio.h>

int dbg_log_dma;

void oam_dma(GB *gb, uint8_t page) {
  if (dbg_log_dma) printf("  OURS OAM DMA write %02x at %llu mc %llu\n", page, (unsigned long long)((gb->cycles % FRAME_CYCLES) / 2), (unsigned long long)gb->mcycles);
  uint16_t src = (uint16_t)page << 8;
  for (int i = 0; i < 160; i++) gb->oam[i] = bus_read(gb, src + i);
}

uint64_t dbg_dma_bytes, dbg_dma_calls, dbg_hblank_chunks;
static void hdma_copy(GB *gb, int bytes) {
  dbg_dma_bytes += bytes; dbg_dma_calls++;
  for (int i = 0; i < bytes; i++) {
    gb->vram[gb->io[R_VBK] & 1][(gb->hdma_dst + i) & 0x1fff] = bus_read(gb, gb->hdma_src + i);
  }
  gb->hdma_src += bytes;
  gb->hdma_dst += bytes;
}

void bus_hdma_chunk(GB *gb) {
  if (!gb->hdma_active) return;
  dbg_hblank_chunks++;
  hdma_copy(gb, 16);
  int stall = gb->double_speed ? 16 : 8;
  for (int i = 0; i < stall; i++) gb_tick(gb);
  if (--gb->hdma_len == 0) gb->hdma_active = false;
}

uint64_t dbg_hdma5_writes, dbg_hdma5_hblank_lcdoff, dbg_hdma5_hblank;
void hdma5_write(GB *gb, uint8_t v) {
  if (dbg_log_dma) printf("  OURS HDMA5 write %02x at %llu mc %llu (src %04x dst %04x)\n", v, (unsigned long long)((gb->cycles % FRAME_CYCLES) / 2), (unsigned long long)gb->mcycles, (gb->io[R_HDMA1] << 8) | gb->io[R_HDMA2], (gb->io[R_HDMA3] << 8) | gb->io[R_HDMA4]);
  dbg_hdma5_writes++; if (v & 0x80) { dbg_hdma5_hblank++; if (!(gb->io[R_LCDC] & 0x80)) dbg_hdma5_hblank_lcdoff++; }
  if (gb->hdma_active && !(v & 0x80)) { gb->hdma_active = false; gb->hdma_len = (v & 0x7f) + 1; return; }
  gb->hdma_src = ((gb->io[R_HDMA1] << 8) | gb->io[R_HDMA2]) & 0xfff0;
  gb->hdma_dst = (((gb->io[R_HDMA3] << 8) | gb->io[R_HDMA4]) & 0x1ff0);
  gb->hdma_len = (v & 0x7f) + 1;
  if (v & 0x80) { gb->hdma_active = true; return; }
  int bytes = gb->hdma_len * 16;
  hdma_copy(gb, bytes);
  int stall = gb->hdma_len * (gb->double_speed ? 16 : 8) + 1;
  for (int i = 0; i < stall; i++) gb_tick(gb);
  gb->hdma_len = 0;
  gb->hdma_active = false;
  gb->irq_delay = true;
}
