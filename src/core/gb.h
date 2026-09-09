#ifndef GB_H
#define GB_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "apu.h"

enum {
  R_P1 = 0x00, R_SB = 0x01, R_SC = 0x02, R_DIV = 0x04, R_TIMA = 0x05, R_TMA = 0x06, R_TAC = 0x07,
  R_IF = 0x0f, R_NR52 = 0x26,
  R_LCDC = 0x40, R_STAT = 0x41, R_SCY = 0x42, R_SCX = 0x43, R_LY = 0x44, R_LYC = 0x45, R_DMA = 0x46,
  R_BGP = 0x47, R_OBP0 = 0x48, R_OBP1 = 0x49, R_WY = 0x4a, R_WX = 0x4b, R_KEY1 = 0x4d, R_VBK = 0x4f,
  R_BOOT = 0x50, R_HDMA1 = 0x51, R_HDMA2 = 0x52, R_HDMA3 = 0x53, R_HDMA4 = 0x54, R_HDMA5 = 0x55,
  R_RP = 0x56, R_BCPS = 0x68, R_BCPD = 0x69, R_OCPS = 0x6a, R_OCPD = 0x6b, R_OPRI = 0x6c, R_SVBK = 0x70,
};

enum { INT_VBLANK = 1, INT_STAT = 2, INT_TIMER = 4, INT_SERIAL = 8, INT_JOYPAD = 16 };
enum { JOY_A = 1, JOY_B = 2, JOY_SELECT = 4, JOY_START = 8, JOY_RIGHT = 16, JOY_LEFT = 32, JOY_UP = 64, JOY_DOWN = 128 };
enum { MBC_NONE = 0, MBC_1 = 1, MBC_5 = 5 };

#define FZ 0x80
#define FN 0x40
#define FH 0x20
#define FC 0x10
#define FRAME_CYCLES 70224
#define GRID_OFFSET -8
extern int64_t gb_grid_offset;
#define GRID_FRAME(c) (((c) - gb_grid_offset) / FRAME_CYCLES)
#define GRID_END(f) ((f) * FRAME_CYCLES + gb_grid_offset)
#define BOOT_CYCLES 0
#define FB_W 160
#define FB_H 144

typedef struct {
  uint64_t frame;
  uint8_t wram[8][4096], hram[127], vram[2][8192], oam[160], io[128], bg_pal[64], ob_pal[64], ie, ram_bank;
  uint32_t rom_bank;
  uint16_t framebuffer[FB_W * FB_H];
  bool joy_latched, joy_read;
} GBSample;

typedef struct GB {
  uint8_t a, f, b, c, d, e, h, l;
  uint16_t sp, pc;
  bool ime, ime_delay, halted, halt_bug, hung;
  bool double_speed, speed_armed;

  const uint8_t *rom;
  size_t rom_size;
  int mbc;
  uint32_t rom_bank;
  uint8_t ram_bank;
  bool ram_enabled;
  uint8_t eram[32768];
  size_t eram_size;
  const uint8_t *boot;
  size_t boot_size;
  bool boot_mapped;

  uint8_t vram[2][8192];
  uint8_t wram[8][4096];
  uint8_t oam[160];
  uint8_t hram[127];
  uint8_t io[128];
  uint8_t ie;
  uint8_t bg_pal[64], ob_pal[64];

  uint16_t div_counter;
  int tima_reload;

  int ppu_dot, mode3_end;
  uint8_t ppu_mode;
  bool stat_line, first_line_after_enable;
  int window_line;
  uint16_t framebuffer[FB_W * FB_H];
  bool frame_ready;
  uint64_t frame_count;

  uint16_t hdma_src, hdma_dst;
  uint8_t hdma_len;
  bool hdma_active, hdma_chunk_pending;

  uint8_t joy;
  bool init_ram_loaded;
  bool joy_latched;
  void (*serial_out)(void *ctx, uint8_t byte);
  void *serial_ctx;

  Apu apu;
  uint64_t cycles, mcycles;
  bool joy_read, vblank_if_raised, hooks_checked, hooks_ok;
  uint16_t hook_pc;
  uint32_t ime_writes, sp_loads;
  uint8_t (*input_at)(void *ctx, uint64_t frame);
  void *input_ctx;
  uint64_t next_sample_at;
  int sample_head, sample_count;
  GBSample *samples;
  uint64_t sample_overflow;
  const GBSample *sample;
  void (*frame_cb)(struct GB *gb, const GBSample *sample, void *ctx);
  void *frame_ctx;
} GB;

void gb_init(GB *gb);
bool gb_load_rom(GB *gb, const uint8_t *rom, size_t size);
void gb_set_boot_rom(GB *gb, const uint8_t *boot, size_t size);
void gb_reset(GB *gb);
void gb_tick(GB *gb);
void gb_step(GB *gb);
void cpu_dispatch_interrupt(GB *gb);
void gb_run_cycles(GB *gb, uint64_t target);
uint64_t gb_run_frame(GB *gb);
uint8_t gb_input_now(GB *gb);
void gb_run_until_vblank(GB *gb);
#define GB_W0_SKIP_END 0x2c0
#define GB_SOUND_BANK 2
#define GB_SOUND_BANK_LO 0xf80
#define GB_HRAM_MUSIC_LO 0x58
#define GB_HRAM_MUSIC_HI 0x73
uint64_t gb_state_hash(const GB *gb);
uint64_t gb_frame_hash(const GB *gb);

void timer_tick(GB *gb);
void timer_write_div(GB *gb);
void gb_speed_switch(GB *gb);
void timer_write_tac(GB *gb, uint8_t v);
void ppu_tick(GB *gb, int dots);
void ppu_write_lcdc(GB *gb, uint8_t v);
void ppu_update_stat(GB *gb);
static inline uint8_t ppu_ly_read(const GB *gb) { return (gb->io[R_LY] == 153 && gb->ppu_dot >= 4) ? 0 : gb->io[R_LY]; }
void render_scanline(GB *gb);
#endif
