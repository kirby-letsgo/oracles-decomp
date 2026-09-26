#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "core/gb.h"
#include "platform/png.h"
#include "hw/render.h"
#include "platform/setup.h"
#include "platform/window.h"
#include "hooks/hooks.h"
#include "rt/fibers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint8_t live_joy;

static uint8_t *read_all(const char *path, size_t *size);

static uint8_t *rec_buf;
static uint64_t rec_len, rec_cap, rec_resume;
static const char *rec_path;
static GB *rec_gb;

// A snapshot of the machine next to the recording (FILE.inputs.state), taken whenever the inputs
// are written: resuming loads it instead of replaying every recorded frame. It ends with the
// number of input frames it goes with; a snapshot from another build or another length is ignored.
static void rec_state_path(char *out, size_t n) { snprintf(out, n, "%s.state", rec_path); }

static void rec_state_write(void) {
  char path[1024];
  rec_state_path(path, sizeof path);
  if (!oracles_save_boot_state(rec_gb, path)) return;
  FILE *f = fopen(path, "ab");
  if (!f) return;
  uint64_t len = rec_len;
  fwrite(&len, sizeof len, 1, f);
  fclose(f);
}

static bool rec_state_load(void) {
  char path[1024];
  rec_state_path(path, sizeof path);
  FILE *f = fopen(path, "rb");
  if (!f) return false;
  uint64_t len = 0;
  bool ok = fseek(f, -(long)sizeof len, SEEK_END) == 0 && fread(&len, sizeof len, 1, f) == 1;
  fclose(f);
  if (!ok || len != rec_len) return false;
  return oracles_load_boot_state(rec_gb, path);
}

static void rec_push(uint8_t joy) {
  if (rec_len == rec_cap) { rec_cap = rec_cap ? rec_cap * 2 : 1 << 16; rec_buf = realloc(rec_buf, rec_cap); }
  rec_buf[rec_len++] = joy;
}

static void rec_load_existing(void) {
  size_t n;
  uint8_t *d = read_all(rec_path, &n);
  if (!d) return;
  if (n < 8 || memcmp(d, "ORIN", 4)) { fprintf(stderr, "%s is not an .inputs file\n", rec_path); exit(2); }
  uint32_t count;
  memcpy(&count, d + 4, 4);
  for (uint32_t i = 0; i < count && 8 + i < n; i++) rec_push(d[8 + i]);
  rec_resume = rec_len;
  free(d);
  if (rec_state_load()) {
    fprintf(stderr, "resuming at frame %llu from the saved snapshot\n", (unsigned long long)rec_resume);
    return;
  }
  fprintf(stderr, "resuming: replaying %llu recorded frames first\n", (unsigned long long)rec_resume);
}

static void rec_set(uint64_t frame, uint8_t joy) {
  while (rec_len <= frame) rec_push(0);
  rec_buf[frame] = joy;
}

// The core indexes inputs by its own frame counter (boot frames included), so the recording is
// keyed by that and not by the render loop's iteration count.
static uint8_t live_input(void *ctx, uint64_t frame) {
  (void)ctx;
  if (frame < rec_resume) return rec_buf[frame];
  if (rec_path) rec_set(frame, live_joy);
  return live_joy;
}

static void rec_write(void) {
  FILE *f = fopen(rec_path, "wb");
  if (!f) return;
  uint32_t n = (uint32_t)rec_len;
  fwrite("ORIN", 1, 4, f);
  fwrite(&n, 4, 1, f);
  fwrite(rec_buf, 1, rec_len, f);
  fclose(f);
  rec_state_write();
}

// Save states (Cmd+S / Cmd+R): one slot next to the ROM (FILE.savestate), or next to the recording
// while recording. A state ends with the core frame it was taken at; loading one during a
// recording cuts the recorded inputs back to that frame, so the movie still replays from power-on.
static void savestate_path(const char *rom_path, char *out, size_t n) {
  if (rec_path) { snprintf(out, n, "%s.savestate", rec_path); return; }
  snprintf(out, n, "%s", rom_path);
  char *dot = strrchr(out, '.');
  if (dot) *dot = 0;
  strncat(out, ".savestate", n - strlen(out) - 1);
}

static bool savestate_write(GB *gb, const char *path) {
  if (!oracles_save_boot_state(gb, path)) return false;
  FILE *f = fopen(path, "ab");
  if (!f) return false;
  uint64_t frame = GRID_FRAME(gb->cycles);
  bool ok = fwrite(&frame, sizeof frame, 1, f) == 1;
  fclose(f);
  return ok;
}

static bool savestate_read(GB *gb, const char *path) {
  FILE *f = fopen(path, "rb");
  if (!f) return false;
  uint64_t frame = 0;
  bool ok = fseek(f, -(long)sizeof frame, SEEK_END) == 0 && fread(&frame, sizeof frame, 1, f) == 1;
  fclose(f);
  if (!ok || (rec_path && frame > rec_len)) return false;
  if (!oracles_load_boot_state(gb, path)) return false;
  fibers_reset(gb);
  if (rec_path) rec_len = frame;
  return true;
}

#define AUDIO_TARGET_BYTES (APU_SAMPLE_RATE / 10 * 4)

static uint8_t *read_all(const char *path, size_t *size) {
  FILE *f = fopen(path, "rb");
  if (!f) return NULL;
  fseek(f, 0, SEEK_END);
  long n = ftell(f);
  fseek(f, 0, SEEK_SET);
  uint8_t *buf = malloc(n);
  if (fread(buf, 1, n, f) != (size_t)n) { free(buf); fclose(f); return NULL; }
  fclose(f);
  *size = n;
  return buf;
}

static void sav_path(const char *rom_path, char *out, size_t n) {
  snprintf(out, n, "%s", rom_path);
  char *dot = strrchr(out, '.');
  if (dot) *dot = 0;
  strncat(out, ".sav", n - strlen(out) - 1);
}

static void load_sram(GB *gb, const char *path) {
  FILE *f = fopen(path, "rb");
  if (!f) return;
  size_t n = fread(gb->eram, 1, gb->eram_size, f);
  (void)n;
  fclose(f);
}

static void save_sram(GB *gb, const char *path) {
  if (gb->eram_size == 0) return;
  FILE *f = fopen(path, "wb");
  if (!f) return;
  fwrite(gb->eram, 1, gb->eram_size, f);
  fclose(f);
}

static uint8_t key_bit(SDL_Scancode sc) {
  switch (sc) {
  case SDL_SCANCODE_UP: return JOY_UP;
  case SDL_SCANCODE_DOWN: return JOY_DOWN;
  case SDL_SCANCODE_LEFT: return JOY_LEFT;
  case SDL_SCANCODE_RIGHT: return JOY_RIGHT;
  case SDL_SCANCODE_X: return JOY_A;
  case SDL_SCANCODE_Z: return JOY_B;
  case SDL_SCANCODE_RETURN:
  case SDL_SCANCODE_ESCAPE: return JOY_START;
  case SDL_SCANCODE_RSHIFT:
  case SDL_SCANCODE_BACKSPACE: return JOY_SELECT;
  default: return 0;
  }
}

static uint8_t pad_bit(int button) {
  switch (button) {
  case SDL_GAMEPAD_BUTTON_DPAD_UP: return JOY_UP;
  case SDL_GAMEPAD_BUTTON_DPAD_DOWN: return JOY_DOWN;
  case SDL_GAMEPAD_BUTTON_DPAD_LEFT: return JOY_LEFT;
  case SDL_GAMEPAD_BUTTON_DPAD_RIGHT: return JOY_RIGHT;
  case SDL_GAMEPAD_BUTTON_SOUTH: return JOY_A;
  case SDL_GAMEPAD_BUTTON_WEST: return JOY_B;
  case SDL_GAMEPAD_BUTTON_START: return JOY_START;
  case SDL_GAMEPAD_BUTTON_BACK: return JOY_SELECT;
  default: return 0;
  }
}

int main(int argc, char **argv) {
  if (argc < 2) { fprintf(stderr, "usage: oracles ROM [BOOTROM] [--record FILE.inputs]\n"); return 2; }
  const char *init_ram = NULL;
  for (int i = 2; i + 1 < argc; i++) {
    if (!strcmp(argv[i], "--record")) { rec_path = argv[++i]; }
    else if (!strcmp(argv[i], "--init-ram")) { init_ram = argv[++i]; }
  }
  if (rec_path && !init_ram) init_ram = "tas/gbhawk-wram0.txt";
  if (rec_path) hook_mode = HOOK_MODE_OFF;
  size_t rom_size, boot_size;
  uint8_t *rom = read_all(argv[1], &rom_size);
  if (!rom) { fprintf(stderr, "cannot read %s\n", argv[1]); return 2; }
  GB *gb = calloc(1, sizeof *gb);
  gb_init(gb);
  if (!gb_load_rom(gb, rom, rom_size)) { fprintf(stderr, "unsupported ROM\n"); return 2; }
  if (argc > 2 && argv[2][0] != '-') {
    uint8_t *boot = read_all(argv[2], &boot_size);
    if (boot) { oracles_apply_agb_boot_patch(boot, boot_size); gb_set_boot_rom(gb, boot, boot_size); }
  } else if (rec_path) { fprintf(stderr, "--record needs the boot ROM argument to match the headless runner\n"); return 2; }
  if (init_ram && !oracles_load_init_ram(gb, init_ram)) { fprintf(stderr, "cannot read %s\n", init_ram); return 2; }
  gb_reset(gb);
  char sav[1024];
  sav_path(argv[1], sav, sizeof sav);
  if (!rec_path) load_sram(gb, sav);
  else { rec_gb = gb; rec_load_existing(); }

  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) { fprintf(stderr, "%s\n", SDL_GetError()); return 2; }
  SDL_Window *win;
  SDL_Renderer *ren;
  SDL_Texture *tex;
  if (!oracles_open_window("Oracles", NULL, &win, &ren, &tex)) { fprintf(stderr, "%s\n", SDL_GetError()); return 2; }
  SDL_SetRenderLogicalPresentation(ren, FB_W, FB_H, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
  SDL_AudioSpec spec = {SDL_AUDIO_S16, 2, APU_SAMPLE_RATE};
  SDL_AudioStream *audio = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
  if (audio) SDL_ResumeAudioStreamDevice(audio);

  static uint8_t rgb[FB_W * FB_H * 3];
  static int16_t samples[APU_RING * 2];
  bool running = true;
  gb->input_at = live_input;
  uint64_t frames = 0;
  bool muted = false;
  uint64_t title_reset = 0;
  while (running) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
      switch (ev.type) {
      case SDL_EVENT_QUIT: running = false; break;
      case SDL_EVENT_KEY_DOWN:
        if (oracles_window_key(win, &ev.key)) break;
        if ((ev.key.mod & SDL_KMOD_GUI) && (ev.key.scancode == SDL_SCANCODE_S || ev.key.scancode == SDL_SCANCODE_R)) {
          if (ev.key.repeat) break;
          char path[1024];
          savestate_path(argv[1], path, sizeof path);
          bool saving = ev.key.scancode == SDL_SCANCODE_S;
          bool ok = GRID_FRAME(gb->cycles) >= rec_resume && (saving ? savestate_write(gb, path) : savestate_read(gb, path));
          if (ok && !saving) live_joy = 0;
          SDL_SetWindowTitle(win, ok ? (saving ? "Oracles - state saved" : "Oracles - state loaded")
                                     : (saving ? "Oracles - could not save state" : "Oracles - no state to load"));
          title_reset = frames + 120;
          fprintf(stderr, "%s %s %s\n", saving ? "save state" : "load state", ok ? "ok:" : "failed:", path);
        }
        else if (ev.key.scancode == SDL_SCANCODE_M && !ev.key.repeat) {
          muted = !muted;
          if (audio) SDL_SetAudioStreamGain(audio, muted ? 0.0f : 1.0f);
        }
        else if (ev.key.scancode == SDL_SCANCODE_F12) {
          char path[256];
          snprintf(path, sizeof path, "out/screenshot_%llu.png", (unsigned long long)frames);
          framebuffer_to_rgb(gb->sample->framebuffer, rgb);
          png_write_rgb(path, rgb, FB_W, FB_H);
        } else live_joy |= key_bit(ev.key.scancode);
        break;
      case SDL_EVENT_KEY_UP: live_joy &= ~key_bit(ev.key.scancode); break;
      case SDL_EVENT_GAMEPAD_ADDED: SDL_OpenGamepad(ev.gdevice.which); break;
      case SDL_EVENT_GAMEPAD_BUTTON_DOWN: live_joy |= pad_bit(ev.gbutton.button); break;
      case SDL_EVENT_GAMEPAD_BUTTON_UP: live_joy &= ~pad_bit(ev.gbutton.button); break;
      }
    }
    bool fast_forward = GRID_FRAME(gb->cycles) < rec_resume;
    if (!fast_forward && audio && SDL_GetAudioStreamQueued(audio) > AUDIO_TARGET_BYTES) { SDL_Delay(1); continue; }
    gb_run_frame(gb);
    frames++;
    if (title_reset && frames >= title_reset) { SDL_SetWindowTitle(win, "Oracles"); title_reset = 0; }
    if (rec_path && !fast_forward && frames % 3600 == 0) rec_write();
    uint32_t n = apu_read_samples(&gb->apu, samples, APU_RING);
    if (fast_forward) {
      if (GRID_FRAME(gb->cycles) >= rec_resume) { live_joy = 0; fprintf(stderr, "resumed at frame %llu, recording live\n", (unsigned long long)rec_resume); }
      else if (frames % 600) continue;
    }
    if (audio && !fast_forward) SDL_PutAudioStreamData(audio, samples, n * 4);
    framebuffer_to_rgb(gb->sample->framebuffer, rgb);
    SDL_UpdateTexture(tex, NULL, rgb, FB_W * 3);
    SDL_RenderClear(ren);
    SDL_RenderTexture(ren, tex, NULL, NULL);
    SDL_RenderPresent(ren);
    if (!rec_path && frames % 600 == 0) save_sram(gb, sav);
  }
  if (rec_path) {
    rec_write();
    fprintf(stderr, "recorded %llu frames to %s, state %016llx\n", (unsigned long long)rec_len, rec_path,
            (unsigned long long)gb_state_hash(gb));
  }
  else save_sram(gb, sav);
  SDL_Quit();
  return 0;
}
