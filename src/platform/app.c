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
#include "game/features.h"
#include "assets/assets.h"
#include "ui/launcher.h"
#include "ui/menu.h"
#include "ui/settings.h"
#include "ui/filter.h"
#include "ui/touch.h"
#include "ui/syncui.h"
#include "platform/sync_client.h"
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
  uint8_t *rom = SDL_LoadFile(rom_path, &n);
  if (!rom) { fprintf(stderr, "cannot read %s: %s\n", rom_path, SDL_GetError()); return false; }
  const char *game = game_of(rom, n);
  if (!game) { SDL_free(rom); return false; }
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
  SDL_free(rom); free(tab);
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

static Settings settings;

static uint8_t joy_of(Action a) {
  static const uint8_t bits[ACTIONS] = {JOY_UP, JOY_DOWN, JOY_LEFT, JOY_RIGHT, JOY_A, JOY_B, JOY_START, JOY_SELECT, 0, 0, 0, 0, 0};
  return a < ACTIONS ? bits[a] : 0;
}
static uint32_t action_bit(Action a) { return a < ACTIONS ? 1u << a : 0; }

static const char *key_name(int sc) {
  static char buf[32];
  snprintf(buf, sizeof buf, "%s", SDL_GetScancodeName((SDL_Scancode)sc));
  for (char *p = buf; *p; p++) *p = (char)SDL_toupper(*p);
  return *buf ? buf : "?";
}
static const char *pad_name(int button) {
  static char buf[32];
  if (button == PAD_RIGHT_TRIGGER) return "RT";
  const char *s = SDL_GetGamepadStringForButton((SDL_GamepadButton)button);
  snprintf(buf, sizeof buf, "%s", s ? s : "?");
  for (char *p = buf; *p; p++) *p = (char)SDL_toupper(*p);
  return buf;
}

static uint8_t live_joy, slot_joy[2];
static uint8_t live_input(void *ctx, uint64_t frame) { (void)ctx; (void)frame; return live_joy | slot_joy[0] | slot_joy[1]; }

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

static bool poll_event(SDL_Event *ev);

static bool pick_rom(char *game_out, const char *cache) {
  static const SDL_DialogFileFilter filters[] = {{"Game Boy Color ROM", "gbc;gb"}};
  pick_state = 0;
#ifdef __ANDROID__
  (void)filters;
  SDL_ShowOpenFileDialog(on_pick, NULL, NULL, NULL, 0, NULL, false);
#else
  SDL_ShowOpenFileDialog(on_pick, NULL, NULL, filters, 1, NULL, false);
#endif
  while (!pick_state) { SDL_Event ev; while (poll_event(&ev)) if (ev.type == SDL_EVENT_QUIT) return false; SDL_Delay(10); }
  return pick_state > 0 && extract(picked, cache, game_out);
}

// Save states: slots 1-4 (state_1..state_4) and the auto state written on quit (state_auto), each
// with its frame as a thumbnail (state_N.thumb, raw RGB24) and its file time as the date.
#define SLOT_AUTO UI_SLOTS

static void slot_path(char *out, size_t n, const char *dir, int slot, const char *ext) {
  if (slot == SLOT_AUTO) snprintf(out, n, "%s/state_auto%s", dir, ext);
  else snprintf(out, n, "%s/state_%d%s", dir, slot + 1, ext);
}

static bool file_exists(const char *path) { return SDL_GetPathInfo(path, NULL); }

// The single Cmd+S state of earlier versions becomes slot 1.
static void migrate_state(const char *dir) {
  char old[1300], slot1[1300];
  snprintf(old, sizeof old, "%s/savestate", dir);
  slot_path(slot1, sizeof slot1, dir, 0, "");
  if (file_exists(old) && !file_exists(slot1)) SDL_RenamePath(old, slot1);
}

static bool slot_save(GB *gb, const char *dir, int slot, const uint8_t *rgb) {
  char path[1300];
  slot_path(path, sizeof path, dir, slot, "");
  if (!oracles_save_boot_state(gb, path)) return false;
  slot_path(path, sizeof path, dir, slot, ".thumb");
  write_file(path, rgb, FB_W * FB_H * 3);
  return true;
}

static bool slot_load(GB *gb, const char *dir, int slot) {
  char path[1300];
  slot_path(path, sizeof path, dir, slot, "");
  if (!oracles_load_boot_state(gb, path)) return false;
  fibers_reset(gb);
  return true;
}

static void slots_scan(SlotsMenu *m, const char *dir) {
  static const char *const months[12] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
  for (int i = 0; i < UI_SLOTS; i++) {
    UiSlot *s = &m->slots[i];
    char path[1300];
    SDL_PathInfo info;
    slot_path(path, sizeof path, dir, i, "");
    s->used = SDL_GetPathInfo(path, &info);
    if (!s->used) continue;
    SDL_DateTime dt;
    if (SDL_TimeToDateTime(info.modify_time, &dt, true))
      snprintf(s->when, sizeof s->when, "%s%02d %02d:%02d", months[(dt.month - 1) % 12], dt.day, dt.hour, dt.minute);
    else snprintf(s->when, sizeof s->when, "SAVED");
    memset(s->thumb, 0, sizeof s->thumb);
    slot_path(path, sizeof path, dir, i, ".thumb");
    size_t n;
    uint8_t *img = oracles_read_file(path, &n);
    if (img && n == sizeof s->thumb) memcpy(s->thumb, img, n);
    free(img);
  }
}

static bool any_slot(const char *dir) {
  for (int i = 0; i < UI_SLOTS; i++) {
    char path[1300];
    slot_path(path, sizeof path, dir, i, "");
    if (file_exists(path)) return true;
  }
  return false;
}

static char settings_path[1100];

static void settings_store(void) {
  char text[1024];
  int n = settings_format(&settings, text, sizeof text);
  write_file(settings_path, text, (size_t)n);
}

static void apply_window_settings(SDL_Window *win) {
#ifdef __ANDROID__
  settings.fullscreen = true;
#endif
  if (!!(SDL_GetWindowFlags(win) & SDL_WINDOW_FULLSCREEN) != settings.fullscreen) SDL_SetWindowFullscreen(win, settings.fullscreen);
}

static float volume_gain(bool muted) { return muted ? 0.0f : settings.volume / 10.0f; }

// A message shown over the screen for two seconds (state saved, item set, ...).
static char toast[40];
static uint64_t toast_until;
static void show_status(const char *msg) {
  snprintf(toast, sizeof toast, "%s", msg);
  toast_until = SDL_GetTicks() + 2000;
}

static void apply_game_settings(void) {
  features.fast_text = settings.fast_text;
  features.quick_swap = settings.quick_swap;
  features.fast_menus = settings.fast_menus;
  features.four_slots = settings.four_slots;
}

// The items on X and Y, per game: two bytes in item_buttons.
static void item_buttons_load(const char *dir) {
  char path[1300];
  size_t n;
  snprintf(path, sizeof path, "%s/item_buttons", dir);
  uint8_t *b = oracles_read_file(path, &n);
  features.slot_item[0] = b && n >= 2 ? b[0] : 0;
  features.slot_item[1] = b && n >= 2 ? b[1] : 0;
  free(b);
}

static void item_buttons_store(const char *dir) {
  char path[1300];
  snprintf(path, sizeof path, "%s/item_buttons", dir);
  write_file(path, features.slot_item, 2);
}

// X/Y: in the inventory they take the highlighted item; in play they use theirs while held.
static void item_button(GB *gb, const char *dir, int slot, bool down) {
  if (!down) { features_slot_release(gb, slot); slot_joy[slot] = 0; return; }
  if (features_assign_slot(gb, slot)) {
    item_buttons_store(dir);
    show_status(slot ? "ITEM SET ON Y" : "ITEM SET ON X");
    return;
  }
  slot_joy[slot] = features_slot_press(gb, slot);
}

// The touch overlay: shown from the first touch until a controller or keyboard is used. The layout
// comes from the last present, and every finger holds the actions under it.
#define TEST_TOUCH_ID 0x7e57
#define FINGERS 10
static TouchLayout layout;
static bool touch_on, touch_mouse;
static uint32_t touch_held, touch_pressed, touch_released;
static struct { SDL_FingerID id; uint32_t held; bool down; } fingers[FINGERS];
static const UiFont *overlay_font;
static UiTheme overlay_theme;

static void overlay_style(const UiFont *font, const UiTheme *theme) { overlay_font = font; overlay_theme = *theme; }

static bool direct_touch(SDL_TouchID id) {
  return id == TEST_TOUCH_ID || (touch_mouse && id == SDL_MOUSE_TOUCHID) || SDL_GetTouchDeviceType(id) == SDL_TOUCH_DEVICE_DIRECT;
}

static void touch_event(const SDL_Event *ev) {
  touch_pressed = touch_released = 0;
  if (ev->type == SDL_EVENT_GAMEPAD_BUTTON_DOWN || (ev->type == SDL_EVENT_KEY_DOWN && ev->key.scancode != SDL_SCANCODE_AC_BACK)) {
    touch_on = false;
    memset(fingers, 0, sizeof fingers);
    touch_released = touch_held;
    touch_held = 0;
    return;
  }
  bool down = ev->type == SDL_EVENT_FINGER_DOWN, move = ev->type == SDL_EVENT_FINGER_MOTION;
  bool up = ev->type == SDL_EVENT_FINGER_UP || ev->type == SDL_EVENT_FINGER_CANCELED;
  if ((!down && !move && !up) || !direct_touch(ev->tfinger.touchID)) return;
  if (!touch_on) { touch_on = down; return; }
  int f = -1;
  for (int i = 0; i < FINGERS && f < 0; i++) if (fingers[i].down && fingers[i].id == ev->tfinger.fingerID) f = i;
  for (int i = 0; i < FINGERS && f < 0 && down; i++) if (!fingers[i].down) f = i;
  if (f < 0) return;
  fingers[f].id = ev->tfinger.fingerID;
  fingers[f].down = !up;
  float x = ev->tfinger.x * layout.screen_w / layout.scale, y = ev->tfinger.y * layout.screen_h / layout.scale;
  fingers[f].held = up ? 0 : touch_hit(&layout, x, y);
  uint32_t held = 0;
  for (int i = 0; i < FINGERS; i++) held |= fingers[i].held;
  touch_pressed = held & ~touch_held;
  touch_released = touch_held & ~held;
  touch_held = held;
}

// Every loop polls through here, so the touch state sees each event once.
static bool poll_event(SDL_Event *ev) {
  if (!SDL_PollEvent(ev)) return false;
  touch_event(ev);
  return true;
}

// Sharp and uncorrected frames go straight to the 160x144 texture; filters render at the layout's
// integer scale into a texture of that size, shown 1:1. A fractional (fill) scale renders at the
// next whole scale and shrinks that smoothly, so pixels stay sharp without uneven widths. The
// overlay is drawn in game pixels too.
#define MAX_FILTER_SCALE 12
static void present(SDL_Renderer *ren, SDL_Texture *tex, const uint8_t *rgb) {
  static SDL_Texture *big, *over;
  static int big_scale, over_w, over_h;
  static uint8_t *buf, *over_px;
  int w = FB_W, h = FB_H;
  SDL_GetCurrentRenderOutputSize(ren, &w, &h);
  SDL_Rect safe = {0, 0, w, h};
  SDL_GetRenderSafeArea(ren, &safe);
  touch_layout(&layout, w, h, (UiRect){safe.x, safe.y, safe.w, safe.h}, touch_on, settings.four_slots, settings.fill);
  if (toast[0] && SDL_GetTicks() < toast_until && overlay_font && overlay_font->loaded) {
    static UiCanvas with_toast;
    memcpy(with_toast.px, rgb, sizeof with_toast.px);
    ui_toast(&with_toast, overlay_font, &overlay_theme, toast);
    rgb = &with_toast.px[0][0][0];
  }
  static int logged_w, logged_h, logged_scale;
  if (w != logged_w || h != logged_h || layout.scale != logged_scale) {
    fprintf(stderr, "screen %dx%d, safe area %d,%d %dx%d, scale %d\n", w, h, safe.x, safe.y, safe.w, safe.h, layout.scale);
    logged_w = w; logged_h = h; logged_scale = layout.scale;
  }
  bool fractional = layout.game_px.w % FB_W != 0;
  int scale = SDL_min(fractional ? (layout.game_px.w + FB_W - 1) / FB_W : layout.scale, MAX_FILTER_SCALE);
  SDL_FRect dst = {(float)layout.game_px.x, (float)layout.game_px.y, (float)layout.game_px.w, (float)layout.game_px.h};
  if (touch_on) SDL_SetRenderDrawColor(ren, overlay_theme.bg.r, overlay_theme.bg.g, overlay_theme.bg.b, 255);
  SDL_RenderClear(ren);
  SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
  if (touch_on) SDL_RenderFillRect(ren, &dst);
  if (!fractional && (settings.filter == FILTER_SHARP || scale < 2) && !settings.gbc_colours) {
    SDL_UpdateTexture(tex, NULL, rgb, FB_W * 3);
    SDL_RenderTexture(ren, tex, NULL, &dst);
  } else {
    if (!big || big_scale != scale) {
      if (big) SDL_DestroyTexture(big);
      big = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, FB_W * scale, FB_H * scale);
      SDL_SetTextureScaleMode(big, SDL_SCALEMODE_NEAREST);
      free(buf);
      buf = malloc((size_t)FB_W * scale * FB_H * scale * 3);
      big_scale = scale;
    }
    SDL_SetTextureScaleMode(big, fractional ? SDL_SCALEMODE_LINEAR : SDL_SCALEMODE_NEAREST);
    ui_filter(rgb, settings.gbc_colours, scale < 2 ? FILTER_SHARP : settings.filter, scale, buf);
    SDL_UpdateTexture(big, NULL, buf, FB_W * scale * 3);
    SDL_RenderTexture(ren, big, NULL, &dst);
  }
  if (touch_on) {
    if (!over || over_w != layout.w || over_h != layout.h) {
      if (over) SDL_DestroyTexture(over);
      over = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, layout.w, layout.h);
      SDL_SetTextureScaleMode(over, SDL_SCALEMODE_NEAREST);
      SDL_SetTextureBlendMode(over, SDL_BLENDMODE_BLEND);
      free(over_px);
      over_px = malloc((size_t)layout.w * layout.h * 4);
      over_w = layout.w;
      over_h = layout.h;
    }
    touch_draw(&layout, overlay_font, &overlay_theme, touch_held, over_px);
    SDL_UpdateTexture(over, NULL, over_px, layout.w * 4);
    SDL_RenderTexture(ren, over, NULL, &(SDL_FRect){0, 0, (float)(layout.w * layout.scale), (float)(layout.h * layout.scale)});
  }
  SDL_RenderPresent(ren);
}

// A modal settings screen; returns false when the window is closed. Changes apply as they are made.
static bool run_settings(SDL_Window *win, SDL_Renderer *ren, SDL_Texture *tex, const UiFont *font, const UiTheme *theme, const uint8_t *game_rgb, SDL_AudioStream *audio);

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
    char dir[1100];
    snprintf(dir, sizeof dir, "%s%s", cache, game_ids[g]);
    migrate_state(dir);
    slot_path(path, sizeof path, dir, SLOT_AUTO, "");
    lg->has_resume = file_exists(path);
    lg->has_slots = any_slot(dir);
    snprintf(path, sizeof path, "%s%s/rom.bin", cache, game_ids[g]);
    uint8_t *rom = oracles_read_file(path, &n);
    ui_theme_load(&lg->theme, rom, rom ? n : 0, g == UI_GAME_AGES);
    if (!font->loaded) ui_font_load(font, rom, rom ? n : 0);
    free(rom);
  }
}

// Test hook: ORACLES_TEST_KEYS="120:Escape,130:Down,140:X" presses each key at that loop tick (every
// pass of the launcher, menu or game loop is a tick), so ctest can drive the menus headless.
static uint64_t test_tick;

// "#A", "#UP", ... in ORACLES_TEST_KEYS touch that control of the overlay (one finger).
static void test_touch(const char *name, bool down) {
  static const char *const names[ACTIONS] = {"UP", "DOWN", "LEFT", "RIGHT", "A", "B", "START", "SELECT", "PAUSE", "FAST", "SWAP", "X", "Y"};
  float x, y;
  for (int a = 0; a < ACTIONS; a++) {
    if (strcmp(name, names[a]) || !touch_point(&layout, (Action)a, &x, &y)) continue;
    SDL_Event ev = {0};
    ev.type = down ? SDL_EVENT_FINGER_DOWN : SDL_EVENT_FINGER_UP;
    ev.tfinger.touchID = TEST_TOUCH_ID;
    ev.tfinger.fingerID = 1;
    ev.tfinger.x = x * layout.scale / layout.screen_w;
    ev.tfinger.y = y * layout.scale / layout.screen_h;
    SDL_PushEvent(&ev);
  }
}

static void test_keys(void) {
  static const char *spec;
  static bool init;
  if (!init) { init = true; spec = getenv("ORACLES_TEST_KEYS"); }
  test_tick++;
  for (const char *p = spec; p && *p;) {
    char name[32];
    unsigned long long at;
    int len = 0;
    if (sscanf(p, "%llu:%31[^,]%n", &at, name, &len) != 2) break;
    if ((at == test_tick || at + 1 == test_tick) && name[0] == '#') test_touch(name + 1, at == test_tick);
    else if (at == test_tick || at + 1 == test_tick) {
      SDL_Event ev = {0};
      ev.type = at == test_tick ? SDL_EVENT_KEY_DOWN : SDL_EVENT_KEY_UP;
      ev.key.scancode = SDL_GetScancodeFromName(name);
      ev.key.down = at == test_tick;
      SDL_PushEvent(&ev);
    }
    p += len;
    if (*p == ',') p++;
  }
}

// F11 / Cmd+F toggles fullscreen and remembers it
static bool window_key(SDL_Window *win, const SDL_KeyboardEvent *key) {
  if (!oracles_window_key(win, key)) return false;
  if (!key->repeat) { settings.fullscreen = !settings.fullscreen; settings_store(); }
  return true;
}

static bool menu_button(const SDL_Event *ev, UiButton *b) {
  static const UiButton touch_buttons[ACTIONS] = {
    [ACT_UP] = UI_UP, [ACT_DOWN] = UI_DOWN, [ACT_LEFT] = UI_LEFT, [ACT_RIGHT] = UI_RIGHT,
    [ACT_A] = UI_ACCEPT, [ACT_START] = UI_ACCEPT, [ACT_B] = UI_BACK, [ACT_PAUSE] = UI_BACK,
  };
  for (int a = 0; a < ACTIONS; a++)
    if ((touch_pressed >> a & 1) && (a <= ACT_START || a == ACT_PAUSE)) { *b = touch_buttons[a]; return true; }
  if (ev->type == SDL_EVENT_KEY_DOWN) {
    switch (ev->key.scancode) {
    case SDL_SCANCODE_AC_BACK: *b = UI_BACK; return true;
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

// A modal slot list; returns the slot, -1 for back, or -2 when the window is closed.
static int pick_slot(SDL_Window *win, SDL_Renderer *ren, SDL_Texture *tex, SlotsMenu *m, const UiFont *font, const UiTheme *theme, const uint8_t *game_rgb) {
  static UiCanvas canvas;
  for (;;) {
    test_keys();
    SDL_Event ev;
    while (poll_event(&ev)) {
      if (ev.type == SDL_EVENT_QUIT) return -2;
      if (ev.type == SDL_EVENT_KEY_DOWN && window_key(win, &ev.key)) continue;
      UiButton b;
      if (!menu_button(&ev, &b) || (ev.type == SDL_EVENT_KEY_DOWN && ev.key.repeat)) continue;
      int slot;
      MenuAction a = slots_press(m, b, &slot);
      if (a == MENU_PICK) return slot;
      if (a == MENU_BACK) return -1;
    }
    slots_draw(m, font, theme, game_rgb, &canvas);
    present(ren, tex, &canvas.px[0][0][0]);
    SDL_Delay(16);
  }
}

static bool run_controls(SDL_Window *win, SDL_Renderer *ren, SDL_Texture *tex, const UiFont *font, const UiTheme *theme, const uint8_t *game_rgb) {
  static UiCanvas canvas;
  ControlsMenu m;
  controls_open(&m);
  for (;;) {
    test_keys();
    SDL_Event ev;
    while (poll_event(&ev)) {
      if (ev.type == SDL_EVENT_QUIT) return false;
      if (ev.type == SDL_EVENT_GAMEPAD_ADDED) SDL_OpenGamepad(ev.gdevice.which);
      if (m.waiting) {
        if (ev.type == SDL_EVENT_KEY_DOWN && !ev.key.repeat) controls_capture_key(&m, &settings.bindings, (int)ev.key.scancode);
        else if (ev.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) controls_capture_pad(&m, &settings.bindings, ev.gbutton.button);
        else if (ev.type == SDL_EVENT_GAMEPAD_AXIS_MOTION && ev.gaxis.axis == SDL_GAMEPAD_AXIS_RIGHT_TRIGGER && ev.gaxis.value > 16000)
          controls_capture_pad(&m, &settings.bindings, PAD_RIGHT_TRIGGER);
        if (!m.waiting) settings_store();
        continue;
      }
      if (ev.type == SDL_EVENT_KEY_DOWN && window_key(win, &ev.key)) continue;
      UiButton b;
      if (!menu_button(&ev, &b) || (ev.type == SDL_EVENT_KEY_DOWN && ev.key.repeat)) continue;
      if (controls_press(&m, &settings.bindings, b) == MENU_BACK) { settings_store(); return true; }
      settings_store();
    }
    controls_draw(&m, &settings.bindings, font, theme, game_rgb, key_name, pad_name, &canvas);
    present(ren, tex, &canvas.px[0][0][0]);
    SDL_Delay(16);
  }
}

static bool run_sync_page(SDL_Window *win, SDL_Renderer *ren, SDL_Texture *tex, const UiFont *font, const UiTheme *theme, const uint8_t *game_rgb);

static bool run_settings(SDL_Window *win, SDL_Renderer *ren, SDL_Texture *tex, const UiFont *font, const UiTheme *theme, const uint8_t *game_rgb, SDL_AudioStream *audio) {
  static UiCanvas canvas;
  SettingsMenu m;
  settings_menu_open(&m);
  for (;;) {
    test_keys();
    SDL_Event ev;
    while (poll_event(&ev)) {
      if (ev.type == SDL_EVENT_QUIT) return false;
      if (ev.type == SDL_EVENT_KEY_DOWN && window_key(win, &ev.key)) continue;
      UiButton b;
      if (!menu_button(&ev, &b)) continue;
      SettingsRow row;
      bool was_full = settings.fullscreen;
      MenuAction a = settings_press(&m, &settings, b, &row);
      if (a == MENU_BACK) { settings_store(); return true; }
      if (a == MENU_PICK && row == SET_CONTROLS && !run_controls(win, ren, tex, font, theme, game_rgb)) return false;
      if (a == MENU_PICK && row == SET_SYNC && !run_sync_page(win, ren, tex, font, theme, game_rgb)) return false;
      if (settings.fullscreen != was_full) apply_window_settings(win);
      if (audio) SDL_SetAudioStreamGain(audio, volume_gain(false));
      apply_game_settings();
      settings_store();
    }
    settings_draw(&m, &settings, font, theme, game_rgb, &canvas);
    present(ren, tex, &canvas.px[0][0][0]);
    SDL_Delay(16);
  }
}

// Save sync (sync-server/): this device's code and server, and what the last sync did.
static SyncConfig sync_cfg;
static char app_cache[1024];
static char sync_status_line[24];
static const char *playing;     // the game running now, whose files never sync mid-game

static void sync_label_update(void) {
  if (sync_cfg.code[0]) snprintf(settings.sync_label, sizeof settings.sync_label, "..%s", sync_cfg.code + 12);
  else settings.sync_label[0] = 0;
}

// settings.ini as it is on disk (at start, and after a sync brought another device's)
static void settings_reload(SDL_Window *win) {
  settings_default(&settings);
  size_t n;
  char *text = (char *)oracles_read_file(settings_path, &n);
  char *z = text ? realloc(text, n + 1) : NULL;
  if (z) { z[n] = 0; settings_parse(&settings, z); free(z); } else free(text);
  sync_label_update();
  apply_window_settings(win);
  apply_game_settings();
}

// Shows `rgb` with a SYNCING message until the sync thread has finished one sync of `game`.
static bool sync_blocking(SDL_Renderer *ren, SDL_Texture *tex, const uint8_t *rgb, const char *game, SyncResult *r) {
  if (!sync_cfg.code[0]) return false;
  SyncResult earlier;
  while (sync_busy()) { show_status("SYNCING..."); present(ren, tex, rgb); SDL_Delay(16); SDL_PumpEvents(); }
  sync_poll(&earlier);
  if (!sync_start(&sync_cfg, app_cache, game)) return false;
  while (!sync_poll(r)) {
    SDL_Event ev;
    while (poll_event(&ev)) {}
    show_status("SYNCING...");
    present(ren, tex, rgb);
    SDL_Delay(16);
  }
  toast_until = 0;
  return true;
}

static void side_when(ConflictSide *side, int64_t seconds) {
  SDL_DateTime dt;
  if (SDL_TimeToDateTime((SDL_Time)seconds * SDL_NS_PER_SECOND, &dt, true))
    snprintf(side->when, sizeof side->when, "%02d %02d:%02d", dt.day, dt.hour, dt.minute);
}

static void side_fill(ConflictSide *side, const char *label, const char *game, const char *name, const uint8_t *data, size_t n, const uint8_t *thumb, size_t tn) {
  snprintf(side->label, sizeof side->label, "%.9s", label);
  for (char *p = side->label; *p; p++) *p = (char)SDL_toupper(*p);
  if (!data) { side->missing = true; return; }
  if (!strcmp(name, "sram.sav")) { side->has_files = true; ui_read_files(data, n, !strcmp(game, "ages"), side->files); }
  if (thumb && tn == sizeof side->thumb) { side->has_thumb = true; memcpy(side->thumb, thumb, tn); }
}

// Each file that changed here and on the server: the player keeps one side. Returns false on quit.
static bool run_conflicts(SDL_Renderer *ren, SDL_Texture *tex, const UiFont *font, const UiTheme *theme, const SyncResult *r) {
  static ConflictView v;
  static UiCanvas canvas;
  for (int i = 0; i < r->choices; i++) {
    const SyncChoice *ch = &r->choice[i];
    bool state = !strncmp(ch->name, "state_", 6);
    char path[1300], thumb_name[40];
    snprintf(thumb_name, sizeof thumb_name, "%s.thumb", ch->name);
    size_t n = 0, tn = 0, rn = 0, rtn = 0;
    snprintf(path, sizeof path, "%s%s/%s", app_cache, ch->game, ch->name);
    uint8_t *mine = oracles_read_file(path, &n), *mine_thumb = NULL;
    if (state) { snprintf(path, sizeof path, "%s%s/%s", app_cache, ch->game, thumb_name); mine_thumb = oracles_read_file(path, &tn); }
    uint8_t *theirs = sync_fetch(&sync_cfg, ch->game, ch->name, &rn);
    uint8_t *theirs_thumb = state ? sync_fetch(&sync_cfg, ch->game, thumb_name, &rtn) : NULL;
    memset(&v, 0, sizeof v);
    const char *what = !strcmp(ch->name, "sram.sav") ? "SAVE" : !strcmp(ch->name, "state_auto") ? "RESUME" : NULL;
    if (what) snprintf(v.what, sizeof v.what, "%s %s", !strcmp(ch->game, "ages") ? "AGES" : "SEASONS", what);
    else snprintf(v.what, sizeof v.what, "%s SLOT %c", !strcmp(ch->game, "ages") ? "AGES" : "SEASONS", ch->name[6]);
    side_fill(&v.side[0], "HERE", ch->game, ch->name, mine, n, mine_thumb, tn);
    side_when(&v.side[0], ch->local_mtime);
    side_fill(&v.side[1], ch->remote.device[0] ? ch->remote.device : "OTHER", ch->game, ch->name, theirs, rn, theirs_thumb, rtn);
    side_when(&v.side[1], ch->remote.updated);
    free(mine); free(mine_thumb); free(theirs); free(theirs_thumb);
    for (bool decided = false; !decided;) {
      test_keys();
      SDL_Event ev;
      while (poll_event(&ev)) {
        if (ev.type == SDL_EVENT_QUIT) return false;
        UiButton b;
        if (!menu_button(&ev, &b) || (ev.type == SDL_EVENT_KEY_DOWN && ev.key.repeat)) continue;
        MenuAction a = conflict_press(&v, b);
        if (a == MENU_BACK) decided = true;
        if (a == MENU_PICK) {
          show_status("SYNCING...");
          conflict_draw(&v, font, theme, &canvas);
          present(ren, tex, &canvas.px[0][0][0]);
          show_status(sync_resolve(&sync_cfg, app_cache, ch, v.sel == 0) ? "KEPT" : "SYNC FAILED");
          decided = true;
        }
      }
      conflict_draw(&v, font, theme, &canvas);
      present(ren, tex, &canvas.px[0][0][0]);
      SDL_Delay(16);
    }
  }
  return true;
}

// Toasts the outcome, loads settings that came from elsewhere, and asks about conflicts.
static bool sync_finished(SDL_Window *win, SDL_Renderer *ren, SDL_Texture *tex, const UiFont *font, const UiTheme *theme, const SyncResult *r) {
  if (r->settings_changed) settings_reload(win);
  SDL_DateTime dt;
  SDL_Time now;
  bool clock = SDL_GetCurrentTime(&now) && SDL_TimeToDateTime(now, &dt, true);
  if (!r->ok) {
    fprintf(stderr, "sync failed: %s\n", r->error);
    snprintf(sync_status_line, sizeof sync_status_line, "SYNC FAILED");
    show_status("SYNC FAILED");
    return true;
  }
  if (r->error[0]) fprintf(stderr, "sync: %s\n", r->error);
  if (clock) snprintf(sync_status_line, sizeof sync_status_line, "SYNCED %02d:%02d", dt.hour, dt.minute);
  if (r->uploaded || r->downloaded) show_status("SYNCED");
  return !r->choices || run_conflicts(ren, tex, font, theme, r);
}

// The sync page: the code, a new account, entering a code from another device, sync now, off.
static bool run_sync_page(SDL_Window *win, SDL_Renderer *ren, SDL_Texture *tex, const UiFont *font, const UiTheme *theme, const uint8_t *game_rgb) {
  static UiCanvas canvas;
  SyncMenu m;
  char code[20] = "";
  // what SYNC NOW covers: every game, or during play every game but the running one
  const char *scope = !playing ? SYNC_ALL_GAMES : !strcmp(playing, "ages") ? "seasons" : "ages";
#define REOPEN() do { code[0] = 0; if (sync_cfg.code[0]) sync_format_code(sync_cfg.code, code); \
    syncmenu_open(&m, sync_cfg.code[0] != 0, code, sync_status_line); } while (0)
  REOPEN();
  for (;;) {
    test_keys();
    SDL_Event ev;
    while (poll_event(&ev)) {
      if (ev.type == SDL_EVENT_QUIT) return false;
      if (ev.type == SDL_EVENT_KEY_DOWN && window_key(win, &ev.key)) continue;
      UiButton b;
      if (!menu_button(&ev, &b) || (ev.type == SDL_EVENT_KEY_DOWN && ev.key.repeat)) continue;
      SyncRow row;
      MenuAction a = syncmenu_press(&m, b, &row);
      if (a == MENU_BACK) return true;
      if (a != MENU_PICK) continue;
      bool new_code = false;
      if (row == SYNCROW_CREATE) {
        char nc[17], err[128];
        show_status("CREATING...");
        present(ren, tex, &canvas.px[0][0][0]);
        if (sync_create_account(sync_cfg.url, nc, err, sizeof err)) { memcpy(sync_cfg.code, nc, 17); new_code = true; }
        else { fprintf(stderr, "sync: %s\n", err); show_status("COULD NOT CREATE"); }
      } else if (row == SYNCROW_ENTER) {
        CodeEntry e;
        codeentry_open(&e, sync_cfg.code);
        for (bool done = false; !done;) {
          test_keys();
          while (poll_event(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) return false;
            if (!menu_button(&ev, &b) || (ev.type == SDL_EVENT_KEY_DOWN && ev.key.repeat)) continue;
            MenuAction ea = codeentry_press(&e, b);
            if (ea == MENU_BACK) done = true;
            if (ea == MENU_PICK) { memcpy(sync_cfg.code, e.digits, 17); new_code = done = true; }
          }
          codeentry_draw(&e, font, theme, &canvas);
          present(ren, tex, &canvas.px[0][0][0]);
          SDL_Delay(16);
        }
      } else if (row == SYNCROW_OFF) {
        sync_cfg.code[0] = 0;
        sync_config_store(&sync_cfg, app_cache);
        sync_label_update();
      }
      if (new_code) { sync_config_store(&sync_cfg, app_cache); sync_label_update(); }
      if (row == SYNCROW_NOW || new_code) {
        SyncResult r;
        if (sync_blocking(ren, tex, &canvas.px[0][0][0], scope, &r) && !sync_finished(win, ren, tex, font, theme, &r)) return false;
      }
      REOPEN();
    }
    syncmenu_draw(&m, font, theme, game_rgb, &canvas);
    present(ren, tex, &canvas.px[0][0][0]);
    SDL_Delay(16);
  }
#undef REOPEN
}

// Runs the launcher until a game is chosen (true) or the player quits (false).
static bool run_launcher(SDL_Window *win, SDL_Renderer *ren, SDL_Texture *tex, const char *cache, char *game_out, int *file_out, bool *file_used, int *slot_out) {
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
  static SettingsMenu settings_menu;
  settings_menu_open(&settings_menu);
  if (sync_cfg.code[0] && !sync_busy()) sync_start(&sync_cfg, cache, SYNC_ALL_GAMES);
  for (;;) {
    test_keys();
    SyncResult synced;
    if (sync_poll(&synced)) {
      if (!sync_finished(win, ren, tex, &font, &l.games[l.game].theme, &synced)) return false;
      launcher_load(&l, &font, cache);
    }
    SDL_Event ev;
    while (poll_event(&ev)) {
      if (ev.type == SDL_EVENT_QUIT) return false;
      if (ev.type == SDL_EVENT_KEY_DOWN && window_key(win, &ev.key)) continue;
      if (ev.type == SDL_EVENT_GAMEPAD_ADDED) SDL_OpenGamepad(ev.gdevice.which);
      UiButton b;
      if (!menu_button(&ev, &b) || (ev.type == SDL_EVENT_KEY_DOWN && ev.key.repeat)) continue;
      LaunchResult r = launcher_press(&l, b);
      if (r.action == LAUNCH_QUIT) return false;
      if (r.action == LAUNCH_SETTINGS) {
        SettingsRow row;
        bool was_full = settings.fullscreen;
        MenuAction sa = settings_press(&settings_menu, &settings, r.button, &row);
        if (sa == MENU_PICK && row == SET_CONTROLS && !run_controls(win, ren, tex, &font, &l.games[l.game].theme, NULL)) return false;
        if (sa == MENU_PICK && row == SET_SYNC) {
          if (!run_sync_page(win, ren, tex, &font, &l.games[l.game].theme, NULL)) return false;
          launcher_load(&l, &font, cache);
        }
        if (settings.fullscreen != was_full) apply_window_settings(win);
        apply_game_settings();
        settings_store();
      }
      if (r.action == LAUNCH_ADD_ROM) {
        char game[16];
        if (pick_rom(game, cache)) { UiGame g = l.game; launcher_load(&l, &font, cache); launcher_init(&l, g); }
      }
      if (r.action == LAUNCH_PLAY && r.file == LAUNCH_SLOTS) {
        static SlotsMenu slots;
        char dir[1100];
        snprintf(dir, sizeof dir, "%s%s", cache, game_ids[r.game]);
        slots_scan(&slots, dir);
        slots_open(&slots, false);
        int slot = pick_slot(win, ren, tex, &slots, &font, &l.games[r.game].theme, NULL);
        if (slot == -2) return false;
        if (slot < 0) continue;
        *slot_out = slot;
      }
      if (r.action == LAUNCH_PLAY && sync_cfg.code[0]) {
        SyncResult pre;
        if (sync_blocking(ren, tex, rgb, game_ids[r.game], &pre) && !sync_finished(win, ren, tex, &font, &l.games[r.game].theme, &pre)) return false;
        launcher_load(&l, &font, cache);
      }
      if (r.action == LAUNCH_PLAY) {
        strcpy(game_out, game_ids[r.game]);
        *file_out = r.file;
        *file_used = r.file >= 0 && l.games[r.game].files[r.file].valid;
        return true;
      }
    }
    overlay_style(&font, &l.games[l.game].theme);
    launcher_draw(&l, &font, &canvas);
    if (l.settings_tab) settings_draw_list(&settings_menu, &settings, &font, &l.games[l.game].theme, LAUNCHER_BODY_Y + 2, 5, l.settings_list, &canvas);
    ui_to_rgb(&canvas, rgb);
    present(ren, tex, rgb);
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

// Saving needs a frame whose threads are all parked at the top of their loops (threads_parked):
// Cmd+S and the pause menu wait for one, usually the same frame; a load then resumes them in C.
#define NO_SAVE UINT64_MAX
#define PARK_TIMEOUT 600


typedef enum { GAME_TO_LAUNCHER, GAME_QUIT, GAME_FAILED } GameEnd;
typedef enum { MODE_PLAY, MODE_PAUSING, MODE_PAUSED, MODE_QUITTING } GameMode;

typedef struct {
  const char *cache, *game;
  int start_file;               // 0..2, LAUNCH_TITLE, LAUNCH_RESUME, LAUNCH_SLOTS
  bool open_file;               // start_file holds a save: open it, not just highlight it
  int start_slot;               // with LAUNCH_SLOTS
  uint64_t max_frames;
} GameStart;

// The running game, for the background event watch.
typedef struct {
  GB *gb;
  const char *dir, *sav;
  uint8_t *paused_rgb;
  GameMode *mode;
  bool *parked, backgrounded;
  uint64_t *frames;
  const char *game;
} Session;
static Session session;

// Android stops the app inside the event pump that queues the background event, so it is handled in
// an event watch, on the main thread before the app stops: the game runs silently to a frame it can
// be saved at and writes the resume state and SRAM (Android may close it in the background); the
// game loop then opens the pause menu.
static bool SDLCALL on_app_event(void *data, SDL_Event *ev) {
  (void)data;
  if ((ev->type != SDL_EVENT_WILL_ENTER_BACKGROUND && ev->type != SDL_EVENT_TERMINATING) || !session.gb) return true;
  GB *gb = session.gb;
  if (*session.mode == MODE_PLAY || *session.mode == MODE_PAUSING) {
    static int16_t samples[APU_RING * 2];
    live_joy = 0;
    for (int i = 0; i < PARK_TIMEOUT && !threads_parked(gb); i++) { features_frame(gb); gb_run_frame(gb); (*session.frames)++; }
    apu_read_samples(&gb->apu, samples, APU_RING);
    *session.parked = threads_parked(gb);
    framebuffer_to_rgb(gb->sample->framebuffer, session.paused_rgb);
    session.backgrounded = true;
  }
  if (*session.parked && (session.backgrounded || *session.mode == MODE_PAUSED)) slot_save(gb, session.dir, SLOT_AUTO, session.paused_rgb);
  save_sram(gb, session.sav);
  if (sync_cfg.code[0]) sync_start(&sync_cfg, app_cache, session.game);
  return true;
}

static GameEnd run_game(SDL_Window *win, SDL_Renderer *ren, SDL_Texture *tex, const GameStart *gs) {
  char dir[1100], path[1300];
  snprintf(dir, sizeof dir, "%s%s", gs->cache, gs->game);
  migrate_state(dir);
  size_t rom_size, tab_size;
  snprintf(path, sizeof path, "%s/rom.bin", dir);
  uint8_t *rom = oracles_read_file(path, &rom_size);
  snprintf(path, sizeof path, "%s/cyctab.bin", dir);
  uint8_t *tab = oracles_read_file(path, &tab_size);
  if (!rom || !tab || tab_size != rom_size) { fprintf(stderr, "cache in %s is incomplete; run again with the ROM path\n", dir); return GAME_FAILED; }
  bool ages = strcmp(gs->game, "ages") == 0;
  static UiFont font;
  UiTheme theme;
  ui_font_load(&font, rom, rom_size);
  ui_theme_load(&theme, rom, rom_size, ages);
  overlay_style(&font, &theme);
  GB *gb = calloc(1, sizeof *gb);
  gb_init(gb);
  if (!gb_load_rom(gb, rom, rom_size)) { fprintf(stderr, "cache in %s is not a ROM image\n", dir); return GAME_FAILED; }
  gb->cyctab = tab;
#ifndef NDEBUG
  gb->code_bits = assets_code_bits(rom, rom_size);
#endif
  gb_reset(gb);
  char sav[1300];
  snprintf(sav, sizeof sav, "%s/sram.sav", dir);
  load_sram(gb, sav);

  char title[64];
  snprintf(title, sizeof title, "Oracle of %s", ages ? "Ages" : "Seasons");
  SDL_SetWindowTitle(win, title);
  SDL_AudioSpec spec = {SDL_AUDIO_S16, 2, APU_SAMPLE_RATE};
  SDL_AudioStream *audio = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
  if (audio) { SDL_SetAudioStreamGain(audio, volume_gain(false)); SDL_ResumeAudioStreamDevice(audio); }

  static uint8_t rgb[FB_W * FB_H * 3], paused_rgb[FB_W * FB_H * 3];
  static int16_t samples[APU_RING * 2];
  static UiCanvas canvas;
  static SlotsMenu slots;
  PauseMenu pause;
  gb->input_at = live_input;
  live_joy = slot_joy[0] = slot_joy[1] = 0;
  item_buttons_load(dir);
  uint64_t frames = 0, save_from = NO_SAVE, park_from = 0;
  bool muted = false, parked = false, fast = false;
  GameMode mode = MODE_PLAY;
  GameEnd end = GAME_QUIT;
  BootDriver boot = {gs->start_file >= 0 ? BOOT_TO_FILE_SELECT : BOOT_OFF, gs->start_file, gs->open_file, 0};
  if (gs->start_file == LAUNCH_RESUME || gs->start_file == LAUNCH_SLOTS) {
    int slot = gs->start_file == LAUNCH_RESUME ? SLOT_AUTO : gs->start_slot;
    if (!slot_load(gb, dir, slot)) fprintf(stderr, "could not load the state in slot %d\n", slot + 1);
  }
  framebuffer_to_rgb(gb->sample ? gb->sample->framebuffer : gb->framebuffer, rgb);
  session = (Session){gb, dir, sav, paused_rgb, &mode, &parked, false, &frames, gs->game};
  playing = gs->game;

  for (bool running = true; running && !gb->hung;) {
    test_keys();
    SyncResult synced;
    if (sync_poll(&synced)) show_status(synced.ok ? "SYNCED" : "SYNC FAILED");
    SDL_Event ev;
    while (poll_event(&ev)) {
      if (ev.type == SDL_EVENT_QUIT) {
        if (mode == MODE_PAUSED) { end = GAME_QUIT; running = false; if (parked) slot_save(gb, dir, SLOT_AUTO, paused_rgb); }
        else { mode = MODE_QUITTING; end = GAME_QUIT; park_from = frames; live_joy = 0; }
        continue;
      }
      if (ev.type == SDL_EVENT_GAMEPAD_ADDED) { SDL_OpenGamepad(ev.gdevice.which); continue; }
      if (ev.type == SDL_EVENT_KEY_DOWN && window_key(win, &ev.key)) continue;
      if (mode == MODE_PAUSED) {
        UiButton b;
        if (!menu_button(&ev, &b) || (ev.type == SDL_EVENT_KEY_DOWN && ev.key.repeat)) continue;
        PauseItem item;
        MenuAction a = pause_press(&pause, b, &item);
        if (a == MENU_BACK || (a == MENU_PICK && item == PAUSE_RESUME)) {
          mode = MODE_PLAY;
          if (audio) SDL_ResumeAudioStreamDevice(audio);
        } else if (a == MENU_PICK && (item == PAUSE_SAVE || item == PAUSE_LOAD)) {
          slots_scan(&slots, dir);
          slots_open(&slots, item == PAUSE_SAVE);
          int slot = pick_slot(win, ren, tex, &slots, &font, &theme, paused_rgb);
          if (slot == -2) { end = GAME_QUIT; running = false; if (parked) slot_save(gb, dir, SLOT_AUTO, paused_rgb); }
          else if (slot >= 0 && item == PAUSE_SAVE) {
            bool ok = slot_save(gb, dir, slot, paused_rgb);
            show_status(ok ? "STATE SAVED" : "COULD NOT SAVE STATE");
            pause.can_load = pause.can_load || ok;
          } else if (slot >= 0) {
            bool ok = slot_load(gb, dir, slot);
            show_status(ok ? "STATE LOADED" : "COULD NOT LOAD STATE");
            if (ok) { mode = MODE_PLAY; if (audio) SDL_ResumeAudioStreamDevice(audio); }
          }
        } else if (a == MENU_PICK && item == PAUSE_SETTINGS) {
          if (!run_settings(win, ren, tex, &font, &theme, paused_rgb, NULL)) { end = GAME_QUIT; running = false; if (parked) slot_save(gb, dir, SLOT_AUTO, paused_rgb); }
          if (audio) SDL_SetAudioStreamGain(audio, volume_gain(muted));
        } else if (a == MENU_PICK && item == PAUSE_QUIT) {
          if (parked) slot_save(gb, dir, SLOT_AUTO, paused_rgb);
          end = GAME_TO_LAUNCHER;
          running = false;
        }
        continue;
      }
      if (mode != MODE_PLAY) continue;
      uint32_t press = touch_pressed, release = touch_released;
      switch (ev.type) {
      case SDL_EVENT_KEY_DOWN:
        if (ev.key.repeat) break;
        if (ev.key.scancode == SDL_SCANCODE_AC_BACK) press |= 1u << ACT_PAUSE;
        else if ((ev.key.mod & SDL_KMOD_GUI) && ev.key.scancode == SDL_SCANCODE_S) save_from = frames;
        else if ((ev.key.mod & SDL_KMOD_GUI) && ev.key.scancode == SDL_SCANCODE_R) {
          bool ok = slot_load(gb, dir, 0);
          if (ok) { live_joy = 0; save_from = NO_SAVE; }
          show_status(ok ? "STATE 1 LOADED" : "NO STATE IN SLOT 1");
        }
        else if (ev.key.scancode == SDL_SCANCODE_M) {
          muted = !muted;
          if (audio) SDL_SetAudioStreamGain(audio, volume_gain(muted));
          show_status(muted ? "SOUND OFF" : "SOUND ON");
        }
        else press |= action_bit(bindings_key_action(&settings.bindings, (int)ev.key.scancode));
        break;
      case SDL_EVENT_KEY_UP:
        release |= action_bit(bindings_key_action(&settings.bindings, (int)ev.key.scancode));
        break;
      case SDL_EVENT_GAMEPAD_AXIS_MOTION:
        if (ev.gaxis.axis == SDL_GAMEPAD_AXIS_RIGHT_TRIGGER && settings.bindings.pad[ACT_FAST] == PAD_RIGHT_TRIGGER) fast = ev.gaxis.value > 16000;
        break;
      case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
        press |= action_bit(bindings_pad_action(&settings.bindings, ev.gbutton.button));
        break;
      case SDL_EVENT_GAMEPAD_BUTTON_UP:
        release |= action_bit(bindings_pad_action(&settings.bindings, ev.gbutton.button));
        break;
      }
      if (press >> ACT_PAUSE & 1) { mode = MODE_PAUSING; park_from = frames; live_joy = 0; boot.phase = BOOT_OFF; continue; }
      if (press && boot.phase != BOOT_OFF) { boot.phase = BOOT_OFF; live_joy = 0; }
      for (int a = 0; a < ACTIONS; a++) {
        bool item = a == ACT_ITEM_X || a == ACT_ITEM_Y;
        if (press >> a & 1) {
          if (a == ACT_FAST) fast = true;
          else if (a == ACT_SWAP) features_quick_swap(gb);
          else if (item) item_button(gb, dir, a == ACT_ITEM_Y, true);
          else live_joy |= joy_of((Action)a);
        }
        if (release >> a & 1) {
          if (a == ACT_FAST) fast = false;
          else if (item) item_button(gb, dir, a == ACT_ITEM_Y, false);
          else live_joy &= (uint8_t)~joy_of((Action)a);
        }
      }
    }
    if (session.backgrounded) {
      session.backgrounded = false;
      mode = MODE_PAUSED;
      boot.phase = BOOT_OFF;
      pause_open(&pause, parked, any_slot(dir));
      if (audio) { SDL_PauseAudioStreamDevice(audio); SDL_ClearAudioStream(audio); }
    }
    if (!running) break;

    if (mode == MODE_PAUSED) {
      pause_draw(&pause, &font, &theme, paused_rgb, &canvas);
      present(ren, tex, &canvas.px[0][0][0]);
      SDL_Delay(16);
      continue;
    }

    if (boot.phase != BOOT_OFF) {
      // fast-forward the logos and menus, silently
      for (int i = 0; i < 8 && boot.phase != BOOT_OFF; i++) { live_joy = boot_input(gb, &boot, frames); features_frame(gb); gb_run_frame(gb); frames++; }
      if (boot.phase == BOOT_OFF) { live_joy = 0; fprintf(stderr, "boot into file %d done at frame %llu\n", boot.file + 1, (unsigned long long)frames); }
      apu_read_samples(&gb->apu, samples, APU_RING);
    } else if (fast && mode == MODE_PLAY) {
      // fast-forward: up to 4 frames per shown frame, paced by the display, without sound
      for (int i = 0; i < 4; i++) { features_frame(gb); gb_run_frame(gb); frames++; }
      apu_read_samples(&gb->apu, samples, APU_RING);
      SDL_Delay(16);
    } else {
      if (mode == MODE_PLAY && audio && SDL_GetAudioStreamQueued(audio) > AUDIO_TARGET_BYTES) { SDL_Delay(1); continue; }
      features_frame(gb); gb_run_frame(gb);
      frames++;
    }

    // pausing and quitting wait for a frame the state can be saved at
    if (mode == MODE_PAUSING || mode == MODE_QUITTING) {
      parked = threads_parked(gb);
      if (parked || frames - park_from > PARK_TIMEOUT) {
        framebuffer_to_rgb(gb->sample->framebuffer, paused_rgb);
        if (mode == MODE_QUITTING) { if (parked) slot_save(gb, dir, SLOT_AUTO, paused_rgb); running = false; break; }
        mode = MODE_PAUSED;
        pause_open(&pause, parked, any_slot(dir));
        if (audio) { SDL_PauseAudioStreamDevice(audio); SDL_ClearAudioStream(audio); }
        continue;
      }
    }

    if (save_from != NO_SAVE && threads_parked(gb)) {
      framebuffer_to_rgb(gb->sample->framebuffer, rgb);
      bool ok = slot_save(gb, dir, 0, rgb);
      show_status(ok ? "STATE 1 SAVED" : "COULD NOT SAVE STATE");
      save_from = NO_SAVE;
    } else if (save_from != NO_SAVE && frames - save_from > PARK_TIMEOUT) {
      show_status("CAN'T SAVE HERE");
      save_from = NO_SAVE;
    }
    uint32_t n = apu_read_samples(&gb->apu, samples, APU_RING);
    if (audio && mode == MODE_PLAY) SDL_PutAudioStreamData(audio, samples, n * 4);
    framebuffer_to_rgb(gb->sample->framebuffer, rgb);
    present(ren, tex, rgb);
    if (frames % 600 == 0) save_sram(gb, sav);
    if (gs->max_frames && frames >= gs->max_frames) running = false;
  }
  if (gb->hung) { fprintf(stderr, "the engine stopped (pc %04x)\n", gb->pc); end = GAME_FAILED; }
  else if (gs->max_frames) fprintf(stderr, "ran %llu frames, state %016llx\n", (unsigned long long)frames, (unsigned long long)gb_state_hash(gb));
  save_sram(gb, sav);
  session.gb = NULL;
  playing = NULL;
  if (audio) SDL_DestroyAudioStream(audio);
  SDL_SetWindowTitle(win, "Oracles");
  free(gb); free(rom); free(tab);
  return end;
}

#ifdef __ANDROID__
#include <android/log.h>
#include <unistd.h>

static int log_pipe[2];

static int SDLCALL log_thread(void *data) {
  (void)data;
  char buf[1024];
  ssize_t n;
  while ((n = read(log_pipe[0], buf, sizeof buf - 1)) > 0) {
    buf[n] = 0;
    __android_log_write(ANDROID_LOG_INFO, "oracles", buf);
  }
  return 0;
}

// The engine reports on stderr, which Android discards: send it to logcat.
static void stderr_to_logcat(void) {
  if (pipe(log_pipe) != 0) return;
  setvbuf(stderr, NULL, _IOLBF, 0);
  dup2(log_pipe[1], 2);
  SDL_DetachThread(SDL_CreateThread(log_thread, "log", NULL));
}
#endif

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
#ifdef __ANDROID__
  stderr_to_logcat();
  SDL_SetHint(SDL_HINT_ANDROID_TRAP_BACK_BUTTON, "1");
  SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight Portrait");
  touch_on = true;
#endif
  // ORACLES_TOUCH=1 shows the overlay on desktop, with the mouse as a finger
  if (getenv("ORACLES_TOUCH")) { touch_on = touch_mouse = true; SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS, "1"); }
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) { fprintf(stderr, "%s\n", SDL_GetError()); return 2; }
  SDL_AddEventWatch(on_app_event, NULL);
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
  snprintf(settings_path, sizeof settings_path, "%ssettings.ini", cache);
  snprintf(app_cache, sizeof app_cache, "%s", cache);
  sync_config_load(&sync_cfg, cache);
  settings_reload(win);

  // a ROM or --game on the command line starts that game directly; quitting it opens the launcher
  char game[16] = "";
  GameStart gs = {cache, game, LAUNCH_TITLE, false, 0, max_frames};
  bool direct = false;
  if (rom_arg) { if (!extract(rom_arg, cache, game)) return 2; direct = true; }
  else if (game_arg) {
    if (!cached(cache, game_arg)) { fprintf(stderr, "%s is not installed; run once with its ROM path\n", game_arg); return 2; }
    snprintf(game, sizeof game, "%s", game_arg);
    direct = true;
    if (file_arg >= 0 && file_arg < UI_FILES) {
      static Launcher probe;
      static UiFont font;
      launcher_load(&probe, &font, cache);
      gs.start_file = file_arg;
      gs.open_file = probe.games[strcmp(game, "ages") == 0 ? UI_GAME_AGES : UI_GAME_SEASONS].files[file_arg].valid;
    }
  }
  int status = 0;
  for (;;) {
    if (!direct && !run_launcher(win, ren, tex, cache, game, &gs.start_file, &gs.open_file, &gs.start_slot)) break;
    direct = false;
    GameEnd end = run_game(win, ren, tex, &gs);
    if (sync_cfg.code[0] && end != GAME_FAILED && !max_frames) {
      sync_wait(20000);
      SyncResult earlier;
      sync_poll(&earlier);
      sync_start(&sync_cfg, cache, game);
    }
    if (end == GAME_FAILED) { status = 1; break; }
    if (end == GAME_QUIT || max_frames) break;
    gs.start_file = LAUNCH_TITLE;
    gs.open_file = false;
  }
  if (!sync_wait(20000)) fprintf(stderr, "sync still running at exit\n");
  oracles_close_window(win, window_size);
  SDL_Quit();
  return status;
}
