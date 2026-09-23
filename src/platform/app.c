// The shipped app: native engine, no interpreter, no boot ROM, no ROM code. On first launch it
// takes the user's ROM (argument or file dialog), checks its SHA1 against the two known games,
// builds the cycle table from the original bytes, zeroes the code bytes and writes the result to
// the per-platform cache; later launches load the cache and never see the ROM again. Saves are
// the game's own SRAM image in that cache.
//
// usage: oracles-native [ROM] [--game ages|seasons] [--cache DIR] [--frames N]
//   --frames N exits after N frames (tests run the app under SDL_VIDEO_DRIVER=dummy).
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "core/gb.h"
#include "core/sha1.h"
#include "hw/render.h"
#include "platform/setup.h"
#include "game/game.h"
#include "assets/assets.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SCALE 4
#define AUDIO_TARGET_BYTES (APU_SAMPLE_RATE / 10 * 4)
#define ASSETS_VERSION 1

static const struct { const char *sha1, *game; } known[] = {
  {"880374fb978b18af4aa529e2e32f7ffb4d7dd2f4", "ages"},
  {"ba1268290fb2b1b70505d2d7b5825fc8a4816a4b", "seasons"},
};

static bool write_file(const char *path, const void *data, size_t n) {
  FILE *f = fopen(path, "wb");
  if (!f) return false;
  bool ok = fwrite(data, 1, n, f) == n;
  fclose(f);
  return ok;
}

static const char *game_of(const uint8_t *rom, size_t n) {
  char hex[41];
  sha1_hex(rom, n, hex);
  for (size_t i = 0; i < sizeof known / sizeof known[0]; i++)
    if (strcmp(hex, known[i].sha1) == 0) return known[i].game;
  fprintf(stderr, "ROM sha1 %s is not Oracle of Ages or Seasons (USA)\n", hex);
  return NULL;
}

// The cache holds, per game, the ROM image with its code bytes zeroed, the cycle table built
// from the original bytes, and a manifest naming both.
static bool extract(const char *rom_path, const char *cache, char *game_out) {
  size_t n;
  uint8_t *rom = oracles_read_file(rom_path, &n);
  if (!rom) { fprintf(stderr, "cannot read %s\n", rom_path); return false; }
  const char *game = game_of(rom, n);
  if (!game) { free(rom); return false; }
  char hex[41];
  sha1_hex(rom, n, hex);
  uint8_t *tab = cyctab_alloc(rom, n);
  size_t zeroed = assets_zero_code(rom, n);
  char dir[1024], path[1200];
  snprintf(dir, sizeof dir, "%s%s", cache, game);
  SDL_CreateDirectory(dir);
  snprintf(path, sizeof path, "%s/rom.bin", dir);
  bool ok = write_file(path, rom, n);
  snprintf(path, sizeof path, "%s/cyctab.bin", dir);
  ok = ok && write_file(path, tab, n);
  char manifest[512];
  snprintf(manifest, sizeof manifest, "oracles-assets %d\ngame %s\nrom_sha1 %s\nsize %zu\ncode_bytes_zeroed %zu\n", ASSETS_VERSION, game, hex, n, zeroed);
  snprintf(path, sizeof path, "%s/manifest.txt", dir);
  ok = ok && write_file(path, manifest, strlen(manifest));
  if (ok) fprintf(stderr, "extracted %s: %zu code bytes zeroed, cache %s\n", game, zeroed, dir);
  else fprintf(stderr, "cannot write the cache in %s\n", dir);
  strcpy(game_out, game);
  free(rom); free(tab);
  return ok;
}

static bool cached(const char *cache, const char *game) {
  char path[1200];
  snprintf(path, sizeof path, "%s%s/manifest.txt", cache, game);
  FILE *f = fopen(path, "r");
  if (!f) return false;
  int version = 0;
  bool ok = fscanf(f, "oracles-assets %d", &version) == 1 && version == ASSETS_VERSION;
  fclose(f);
  return ok;
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

static uint8_t live_joy;
static uint8_t live_input(void *ctx, uint64_t frame) { (void)ctx; (void)frame; return live_joy; }

static char picked[1200];
static int pick_state;
static void SDLCALL on_pick(void *userdata, const char *const *files, int filter) {
  (void)userdata; (void)filter;
  if (files && files[0]) { snprintf(picked, sizeof picked, "%s", files[0]); pick_state = 1; }
  else pick_state = -1;
}

static void load_sram(GB *gb, const char *path) {
  FILE *f = fopen(path, "rb");
  if (!f) return;
  size_t n = fread(gb->eram, 1, gb->eram_size, f);
  (void)n;
  fclose(f);
}

static void save_sram(GB *gb, const char *path) {
  if (gb->eram_size) write_file(path, gb->eram, gb->eram_size);
}

int main(int argc, char **argv) {
  const char *rom_arg = NULL, *game_arg = NULL, *cache_arg = NULL;
  uint64_t max_frames = 0;
  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "--game") && i + 1 < argc) game_arg = argv[++i];
    else if (!strcmp(argv[i], "--frames") && i + 1 < argc) max_frames = strtoull(argv[++i], NULL, 10);
    else if (!strcmp(argv[i], "--cache") && i + 1 < argc) cache_arg = argv[++i];
    else if (argv[i][0] != '-') rom_arg = argv[i];
    else { fprintf(stderr, "usage: oracles-native [ROM] [--game ages|seasons] [--cache DIR] [--frames N]\n"); return 2; }
  }
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) { fprintf(stderr, "%s\n", SDL_GetError()); return 2; }
  char cache[1024];
  if (cache_arg) snprintf(cache, sizeof cache, "%s%s", cache_arg, cache_arg[strlen(cache_arg) - 1] == '/' ? "" : "/");
  else { char *p = SDL_GetPrefPath("oracles-decomp", "oracles"); if (!p) { fprintf(stderr, "%s\n", SDL_GetError()); return 2; } snprintf(cache, sizeof cache, "%s", p); SDL_free(p); }
  SDL_CreateDirectory(cache);

  char game[16] = "";
  if (rom_arg) { if (!extract(rom_arg, cache, game)) return 2; }
  else if (game_arg && cached(cache, game_arg)) snprintf(game, sizeof game, "%s", game_arg);
  else if (!game_arg && cached(cache, "ages") && !cached(cache, "seasons")) strcpy(game, "ages");
  else if (!game_arg && cached(cache, "seasons") && !cached(cache, "ages")) strcpy(game, "seasons");
  else if (!game_arg && cached(cache, "ages") && cached(cache, "seasons")) strcpy(game, "ages");
  else {
    static const SDL_DialogFileFilter filters[] = {{"Game Boy Color ROM", "gbc;gb"}};
    SDL_ShowOpenFileDialog(on_pick, NULL, NULL, filters, 1, NULL, false);
    while (!pick_state) { SDL_Event ev; while (SDL_PollEvent(&ev)) if (ev.type == SDL_EVENT_QUIT) return 0; SDL_Delay(10); }
    if (pick_state < 0) return 0;
    if (!extract(picked, cache, game)) return 2;
  }

  char dir[1100], path[1300];
  snprintf(dir, sizeof dir, "%s%s", cache, game);
  size_t rom_size, tab_size;
  snprintf(path, sizeof path, "%s/rom.bin", dir);
  uint8_t *rom = oracles_read_file(path, &rom_size);
  snprintf(path, sizeof path, "%s/cyctab.bin", dir);
  uint8_t *tab = oracles_read_file(path, &tab_size);
  if (!rom || !tab || tab_size != rom_size) { fprintf(stderr, "cache in %s is incomplete; run again with the ROM path\n", dir); return 2; }
  GB *gb = calloc(1, sizeof *gb);
  gb_init(gb);
  if (!gb_load_rom(gb, rom, rom_size)) { fprintf(stderr, "cache in %s is not a ROM image\n", dir); return 2; }
  gb->cyctab = tab;
#ifndef NDEBUG
  gb->code_bits = assets_code_bits(rom, rom_size);
#endif
  gb_reset(gb);
  char sav[1300];
  snprintf(sav, sizeof sav, "%s/sram.sav", dir);
  load_sram(gb, sav);

  SDL_Window *win;
  SDL_Renderer *ren;
  char title[64];
  snprintf(title, sizeof title, "Oracle of %s", strcmp(game, "ages") == 0 ? "Ages" : "Seasons");
  if (!SDL_CreateWindowAndRenderer(title, FB_W * SCALE, FB_H * SCALE, 0, &win, &ren)) return 2;
  SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, FB_W, FB_H);
  SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
  SDL_AudioSpec spec = {SDL_AUDIO_S16, 2, APU_SAMPLE_RATE};
  SDL_AudioStream *audio = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
  if (audio) SDL_ResumeAudioStreamDevice(audio);

  static uint8_t rgb[FB_W * FB_H * 3];
  static int16_t samples[APU_RING * 2];
  gb->input_at = live_input;
  uint64_t frames = 0;
  bool running = true;
  bool muted = false;
  while (running && !gb->hung) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
      switch (ev.type) {
      case SDL_EVENT_QUIT: running = false; break;
      case SDL_EVENT_KEY_DOWN:
        if (ev.key.scancode == SDL_SCANCODE_M && !ev.key.repeat) {
          muted = !muted;
          if (audio) SDL_SetAudioStreamGain(audio, muted ? 0.0f : 1.0f);
        }
        else live_joy |= key_bit(ev.key.scancode);
        break;
      case SDL_EVENT_KEY_UP: live_joy &= ~key_bit(ev.key.scancode); break;
      case SDL_EVENT_GAMEPAD_ADDED: SDL_OpenGamepad(ev.gdevice.which); break;
      case SDL_EVENT_GAMEPAD_BUTTON_DOWN: live_joy |= pad_bit(ev.gbutton.button); break;
      case SDL_EVENT_GAMEPAD_BUTTON_UP: live_joy &= ~pad_bit(ev.gbutton.button); break;
      }
    }
    if (audio && SDL_GetAudioStreamQueued(audio) > AUDIO_TARGET_BYTES) { SDL_Delay(1); continue; }
    gb_run_frame(gb);
    frames++;
    uint32_t n = apu_read_samples(&gb->apu, samples, APU_RING);
    if (audio) SDL_PutAudioStreamData(audio, samples, n * 4);
    framebuffer_to_rgb(gb->sample->framebuffer, rgb);
    SDL_UpdateTexture(tex, NULL, rgb, FB_W * 3);
    SDL_RenderClear(ren);
    SDL_RenderTexture(ren, tex, NULL, NULL);
    SDL_RenderPresent(ren);
    if (frames % 600 == 0) save_sram(gb, sav);
    if (max_frames && frames >= max_frames) running = false;
  }
  if (gb->hung) fprintf(stderr, "the engine stopped (pc %04x)\n", gb->pc);
  else if (max_frames) fprintf(stderr, "ran %llu frames, state %016llx\n", (unsigned long long)frames, (unsigned long long)gb_state_hash(gb));
  save_sram(gb, sav);
  SDL_Quit();
  return gb->hung ? 1 : 0;
}
