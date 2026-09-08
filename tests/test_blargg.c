#include "unit.h"
#include "core/gb.h"

static char serial_buf[4096];
static size_t serial_len;
static void capture(void *ctx, uint8_t b) {
  (void)ctx;
  if (serial_len + 1 < sizeof serial_buf) serial_buf[serial_len++] = (char)b;
  serial_buf[serial_len] = 0;
}

static void run_blargg(const char *name, int max_frames) {
  char path[512];
  snprintf(path, sizeof path, "%s/%s", TEST_ROM_DIR, name);
  size_t n;
  uint8_t *rom = read_file(path, &n);
  if (!rom) SKIP(path);
  GB *gb = calloc(1, sizeof *gb);
  gb_init(gb);
  ASSERT(gb_load_rom(gb, rom, n));
  gb->serial_out = capture;
  serial_len = 0;
  serial_buf[0] = 0;
  gb_reset(gb);
  for (int f = 0; f < max_frames; f++) {
    gb_run_frame(gb);
    if (strstr(serial_buf, "Passed") || strstr(serial_buf, "Failed")) break;
  }
  if (!strstr(serial_buf, "Passed")) {
    fprintf(stderr, "%s output:\n%s\n", name, serial_buf);
    ASSERT(0);
  }
  printf("ok %s\n", name);
  free(gb);
  free(rom);
}

int main(void) {
  const char *cpu[] = {"cpu_01-special.gb", "cpu_02-interrupts.gb", "cpu_03-op sp,hl.gb",
    "cpu_04-op r,imm.gb", "cpu_05-op rp.gb", "cpu_06-ld r,r.gb", "cpu_07-jr,jp,call,ret,rst.gb",
    "cpu_08-misc instrs.gb", "cpu_09-op r,r.gb", "cpu_10-bit ops.gb", "cpu_11-op a,(hl).gb"};
  for (size_t i = 0; i < sizeof cpu / sizeof cpu[0]; i++) run_blargg(cpu[i], 4000);
  run_blargg("instr_timing.gb", 1000);
  run_blargg("mem_timing.gb", 1000);
  return 0;
}
