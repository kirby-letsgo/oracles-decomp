// The shipped app: native engine, no interpreter, no boot ROM, no ROM code. On first launch it
// takes the user's ROM (argument or file dialog), checks its SHA1 against the two known games,
// builds the cycle table from the original bytes, zeroes the code bytes and writes the result to
// the per-platform cache; later launches load the cache and never see the ROM again. Saves are
// the game's own SRAM image in that cache; Cmd+S / Cmd+R save and load one state slot there.
//
// usage: oracles-native [ROM] [--game ages|seasons [--file 1-3]] [--cache DIR] [--frames N]
//   --file N boots straight into save file N, as choosing it in the launcher does.
//   With neither a ROM nor --game it opens the launcher (the ROM file dialog on first launch).
//   --frames N exits after N frames (tests run the app under SDL_VIDEO_DRIVER=dummy).
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "core/gb.h"
#include "core/sha1.h"
#include "hw/render.h"
#include "platform/setup.h"
#include "platform/window.h"
#include "rt/fibers.h"
#include "game/game.h"
#include "assets/assets.h"
#include "ui/launcher.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static const char *const game_ids[UI_GAMES] = {"ages", "seasons"};

static bool pick_rom(char *game_out, const char *cache) {
  static const SDL_DialogFileFilter filters[] = {{"Game Boy Color ROM", "gbc;gb"}};
  pick_state = 0;
  SDL_ShowOpenFileDialog(on_pick, NULL, NULL, filters, 1, NULL, false);
  while (!pick_state) { SDL_Event ev; while (SDL_PollEvent(&ev)) if (ev.type == SDL_EVENT_QUIT) return false; SDL_Delay(10); }
  return pick_state > 0 && extract(picked, cache, game_out);
}

// Installed games, their save files and save state, each game's colours, and the font from whichever
// ROM image is cached.
static void launcher_load(Launcher *l, UiFont *font, const char *cache) {
  memset(l->games, 0, sizeof l->games);
  font->loaded = false;
  for (int g = 0; g < UI_GAMES; g++) {
    LauncherGame *lg = &l->games[g];
    ui_theme_default(&lg->theme, g == UI_GAME_AGES);
    if (!cached(cache, game_ids[g])) continue;
    lg->installed = true;
    char path[1300];
    size_t n;
    snprintf(path, sizeof path, "%s%s/sram.sav", cache, game_ids[g]);
    uint8_t *sram = oracles_read_file(path, &n);
    ui_read_files(sram, sram ? n : 0, g == UI_GAME_AGES, lg->files);
    free(sram);
    snprintf(path, sizeof path, "%s%s/savestate", cache, game_ids[g]);
    FILE *f = fopen(path, "rb");
    if (f) { lg->has_state = true; fclose(f); }
    snprintf(path, sizeof path, "%s%s/rom.bin", cache, game_ids[g]);
    uint8_t *rom = oracles_read_file(path, &n);
    ui_theme_load(&lg->theme, rom, rom ? n : 0, g == UI_GAME_AGES);
    if (!font->loaded) ui_font_load(font, rom, rom ? n : 0);
    free(rom);
  }
}

static bool menu_button(const SDL_Event *ev, UiButton *b) {
  if (ev->type == SDL_EVENT_KEY_DOWN) {
    switch (ev->key.scancode) {
    case SDL_SCANCODE_UP: *b = UI_UP; return true;
    case SDL_SCANCODE_DOWN: *b = UI_DOWN; return true;
    case SDL_SCANCODE_LEFT: *b = UI_LEFT; return true;
    case SDL_SCANCODE_RIGHT: *b = UI_RIGHT; return true;
    case SDL_SCANCODE_X: case SDL_SCANCODE_RETURN: case SDL_SCANCODE_SPACE: *b = UI_ACCEPT; return true;
    case SDL_SCANCODE_Z: case SDL_SCANCODE_ESCAPE: case SDL_SCANCODE_BACKSPACE: *b = UI_BACK; return true;
    default: return false;
    }
  }
  if (ev->type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) {
    switch (ev->gbutton.button) {
    case SDL_GAMEPAD_BUTTON_DPAD_UP: *b = UI_UP; return true;
    case SDL_GAMEPAD_BUTTON_DPAD_DOWN: *b = UI_DOWN; return true;
    case SDL_GAMEPAD_BUTTON_DPAD_LEFT: *b = UI_LEFT; return true;
    case SDL_GAMEPAD_BUTTON_DPAD_RIGHT: *b = UI_RIGHT; return true;
    case SDL_GAMEPAD_BUTTON_SOUTH: case SDL_GAMEPAD_BUTTON_START: *b = UI_ACCEPT; return true;
    case SDL_GAMEPAD_BUTTON_EAST: case SDL_GAMEPAD_BUTTON_WEST: *b = UI_BACK; return true;
    default: return false;
    }
  }
  return false;
}

// Runs the launcher until a game is chosen (true) or the player quits (false).
static bool run_launcher(SDL_Window *win, SDL_Renderer *ren, SDL_Texture *tex, const char *cache, char *game_out, int *file_out, bool *file_used) {
  static Launcher l;
  static UiFont font;
  static UiCanvas canvas;
  static uint8_t rgb[UI_W * UI_H * 3];
  launcher_load(&l, &font, cache);
  if (!font.loaded) {
    char game[16];
    if (!pick_rom(game, cache)) return false;
    launcher_load(&l, &font, cache);
  }
  launcher_init(&l, UI_GAME_SEASONS);
  for (;;) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_EVENT_QUIT) return false;
      if (ev.type == SDL_EVENT_KEY_DOWN && oracles_window_key(win, &ev.key)) continue;
      if (ev.type == SDL_EVENT_GAMEPAD_ADDED) SDL_OpenGamepad(ev.gdevice.which);
      UiButton b;
      if (!menu_button(&ev, &b) || (ev.type == SDL_EVENT_KEY_DOWN && ev.key.repeat)) continue;
      LaunchResult r = launcher_press(&l, b);
      if (r.action == LAUNCH_QUIT) return false;
      if (r.action == LAUNCH_ADD_ROM) {
        char game[16];
        if (pick_rom(game, cache)) { UiGame g = l.game; launcher_load(&l, &font, cache); launcher_init(&l, g); }
      }
      if (r.action == LAUNCH_PLAY) {
        strcpy(game_out, game_ids[r.game]);
        *file_out = r.file;
        *file_used = r.file >= 0 && l.games[r.game].files[r.file].valid;
        return true;
      }
    }
    launcher_draw(&l, &font, &canvas);
    ui_to_rgb(&canvas, rgb);
    SDL_UpdateTexture(tex, NULL, rgb, UI_W * 3);
    SDL_RenderClear(ren);
    SDL_RenderTexture(ren, tex, NULL, NULL);
    SDL_RenderPresent(ren);
    SDL_Delay(16);
  }
}

// Boots straight into a save file: presses Start until the file select screen is up, moves the
// cursor to the file, then A (the file) and A (the text-speed prompt). Driven by the game's own
// file-select state, so the same inputs a player would give; any key cancels it.
typedef enum { BOOT_OFF, BOOT_TO_FILE_SELECT, BOOT_MOVE, BOOT_SELECT, BOOT_TEXT_SPEED } BootPhase;
typedef struct { BootPhase phase; int file; bool open_file; uint64_t since; } BootDriver;

// wThreadStateBuffer+6/+7 ($c2e6/$c2e7) in both games: the intro stage and the title screen's state
#define INTRO_STAGE 0x2e6
#define INTRO_VAR 0x2e7
#define FS_MODE(gb) ((gb)->wram[0][wFileSelect_mode & 0xfff])
#define FS_MODE2(gb) ((gb)->wram[0][wFileSelect_mode2 & 0xfff])
#define FS_CURSOR(gb) ((gb)->wram[0][wFileSelect_cursorPos & 0xfff])

static uint8_t boot_input(GB *gb, BootDriver *d, uint64_t frame) {
  bool pulse = (frame / 6) % 2 == 0;
  bool choosing = FS_MODE(gb) == 1 && FS_MODE2(gb) == 1;
  if (frame - d->since > 3600) { d->phase = BOOT_OFF; return 0; }
  switch (d->phase) {
  case BOOT_OFF: return 0;
  case BOOT_TO_FILE_SELECT:
    // Start until the title screen fades out (intro stage 3, title state 3); none after that, so no
    // press lands on the file select screen's first input frame
    if (gb->wram[0][INTRO_STAGE] == 3 && gb->wram[0][INTRO_VAR] == 3) { d->phase = BOOT_MOVE; return 0; }
    return pulse ? JOY_START : 0;
  case BOOT_MOVE:
    if (!choosing) return 0;
    if (FS_CURSOR(gb) == d->file) { d->phase = d->open_file ? BOOT_SELECT : BOOT_OFF; return 0; }
    return pulse ? JOY_DOWN : 0;
  case BOOT_SELECT:
    if (FS_MODE2(gb) == 2) { d->phase = BOOT_TEXT_SPEED; return 0; }
    if (FS_MODE(gb) != 1) { d->phase = BOOT_OFF; return 0; }
    return pulse ? JOY_A : 0;
  case BOOT_TEXT_SPEED:
    if (FS_MODE2(gb) != 2) { d->phase = BOOT_OFF; return 0; }
    return pulse ? JOY_A : 0;
  }
  return 0;
}

// Cmd+S saves at the first frame whose threads are all parked at the top of their loops
// (threads_parked), usually the same frame; a load then resumes them in C.
#define NO_SAVE UINT64_MAX

static uint64_t show_status(SDL_Window *win, const char *title, const char *status, uint64_t frames) {
  char msg[96];
  snprintf(msg, sizeof msg, "%s - %s", title, status);
  SDL_SetWindowTitle(win, msg);
  return frames + 120;
}

int main(int argc, char **argv) {
  const char *rom_arg = NULL, *game_arg = NULL, *cache_arg = NULL;
  int file_arg = -1;
  uint64_t max_frames = 0;
  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "--game") && i + 1 < argc) game_arg = argv[++i];
    else if (!strcmp(argv[i], "--frames") && i + 1 < argc) max_frames = strtoull(argv[++i], NULL, 10);
    else if (!strcmp(argv[i], "--cache") && i + 1 < argc) cache_arg = argv[++i];
    else if (!strcmp(argv[i], "--file") && i + 1 < argc) file_arg = atoi(argv[++i]) - 1;
    else if (argv[i][0] != '-') rom_arg = argv[i];
    else { fprintf(stderr, "usage: oracles-native [ROM] [--game ages|seasons [--file 1-3]] [--cache DIR] [--frames N]\n"); return 2; }
  }
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) { fprintf(stderr, "%s\n", SDL_GetError()); return 2; }
  char cache[1024];
  if (cache_arg) snprintf(cache, sizeof cache, "%s%s", cache_arg, cache_arg[strlen(cache_arg) - 1] == '/' ? "" : "/");
  else { char *p = SDL_GetPrefPath("oracles-decomp", "oracles"); if (!p) { fprintf(stderr, "%s\n", SDL_GetError()); return 2; } snprintf(cache, sizeof cache, "%s", p); SDL_free(p); }
  SDL_CreateDirectory(cache);

  SDL_Window *win;
  SDL_Renderer *ren;
  SDL_Texture *tex;
  char window_size[1100];
  snprintf(window_size, sizeof window_size, "%swindow.txt", cache);
  if (!oracles_open_window("Oracles", window_size, &win, &ren, &tex)) { fprintf(stderr, "%s\n", SDL_GetError()); return 2; }

  char game[16] = "";
  int start_file = -1;
  bool start_file_used = false;
  if (rom_arg) { if (!extract(rom_arg, cache, game)) return 2; }
  else if (game_arg) {
    if (!cached(cache, game_arg)) { fprintf(stderr, "%s is not installed; run once with its ROM path\n", game_arg); return 2; }
    snprintf(game, sizeof game, "%s", game_arg);
    if (file_arg >= 0 && file_arg < UI_FILES) {
      Launcher probe;
      UiFont font;
      launcher_load(&probe, &font, cache);
      start_file = file_arg;
      start_file_used = probe.games[strcmp(game, "ages") == 0 ? UI_GAME_AGES : UI_GAME_SEASONS].files[file_arg].valid;
    }
  }
  else if (!run_launcher(win, ren, tex, cache, game, &start_file, &start_file_used)) { oracles_close_window(win, window_size); SDL_Quit(); return 0; }

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
  char sav[1300], state[1300];
  snprintf(sav, sizeof sav, "%s/sram.sav", dir);
  snprintf(state, sizeof state, "%s/savestate", dir);
  load_sram(gb, sav);

  char title[64];
  snprintf(title, sizeof title, "Oracle of %s", strcmp(game, "ages") == 0 ? "Ages" : "Seasons");
  SDL_SetWindowTitle(win, title);
  SDL_AudioSpec spec = {SDL_AUDIO_S16, 2, APU_SAMPLE_RATE};
  SDL_AudioStream *audio = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
  if (audio) SDL_ResumeAudioStreamDevice(audio);

  static uint8_t rgb[FB_W * FB_H * 3];
  static int16_t samples[APU_RING * 2];
  gb->input_at = live_input;
  uint64_t frames = 0;
  bool running = true;
  bool muted = false;
  uint64_t title_reset = 0, save_from = NO_SAVE;
  BootDriver boot = {start_file >= 0 ? BOOT_TO_FILE_SELECT : BOOT_OFF, start_file, start_file_used, 0};
  if (start_file == LAUNCH_STATE) {
    if (oracles_load_boot_state(gb, state)) fibers_reset(gb);
    else fprintf(stderr, "load state failed: %s\n", state);
  }
  while (running && !gb->hung) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
      switch (ev.type) {
      case SDL_EVENT_QUIT: running = false; break;
      case SDL_EVENT_KEY_DOWN:
        if (oracles_window_key(win, &ev.key)) break;
        if (boot.phase != BOOT_OFF) { boot.phase = BOOT_OFF; live_joy = 0; }
        if ((ev.key.mod & SDL_KMOD_GUI) && ev.key.scancode == SDL_SCANCODE_S && !ev.key.repeat) save_from = frames;
        else if ((ev.key.mod & SDL_KMOD_GUI) && ev.key.scancode == SDL_SCANCODE_R && !ev.key.repeat) {
          bool ok = oracles_load_boot_state(gb, state);
          if (ok) { fibers_reset(gb); live_joy = 0; save_from = NO_SAVE; }
          title_reset = show_status(win, title, ok ? "state loaded" : "no state to load", frames);
          fprintf(stderr, "load state %s %s\n", ok ? "ok:" : "failed:", state);
        }
        else if (ev.key.scancode == SDL_SCANCODE_M && !ev.key.repeat) {
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
    if (boot.phase != BOOT_OFF) {
      // fast-forward the logos and menus, silently
      for (int i = 0; i < 8 && boot.phase != BOOT_OFF; i++) { live_joy = boot_input(gb, &boot, frames); gb_run_frame(gb); frames++; }
      if (boot.phase == BOOT_OFF) { live_joy = 0; fprintf(stderr, "boot into file %d done at frame %llu\n", boot.file + 1, (unsigned long long)frames); }
      apu_read_samples(&gb->apu, samples, APU_RING);
    } else {
      if (audio && SDL_GetAudioStreamQueued(audio) > AUDIO_TARGET_BYTES) { SDL_Delay(1); continue; }
      gb_run_frame(gb);
      frames++;
    }
    if (save_from != NO_SAVE && threads_parked(gb)) {
      bool ok = oracles_save_boot_state(gb, state);
      title_reset = show_status(win, title, ok ? "state saved" : "could not save state", frames);
      fprintf(stderr, "save state %s %s\n", ok ? "ok:" : "failed:", state);
      save_from = NO_SAVE;
    } else if (save_from != NO_SAVE && frames - save_from > 600) {
      title_reset = show_status(win, title, "could not save state here", frames);
      save_from = NO_SAVE;
    }
    if (title_reset && frames >= title_reset) { SDL_SetWindowTitle(win, title); title_reset = 0; }
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
  oracles_close_window(win, window_size);
  SDL_Quit();
  return gb->hung ? 1 : 0;
}
