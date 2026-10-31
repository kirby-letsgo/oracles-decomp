// The shipped app: native engine, no interpreter, no boot ROM, no ROM code. On first launch it
// takes the user's ROM (argument, or the launcher's ADD ROM dialog; loose or in a zip or gz, see
// platform/archive.c), checks its SHA1 against the known ROMs (each game's USA and European
// release), builds the cycle table from the original bytes, zeroes the code bytes and writes the
// result to the per-platform cache; later launches load the cache and never see the ROM again.
// Each ROM keeps its own folder there (ages, ages-eu, seasons, seasons-eu), saves and states
// included, and the one added last is the one its game's tab starts (active.txt). Saves are the game's own SRAM image in that
// cache; Cmd+S / Cmd+R save and load one state slot there.
//
// usage: oracles-native [ROM] [--game ages|seasons] [--file 1-3 | --state auto|1-4 | --latest] [--cache DIR] [--frames N]
//   --file N boots straight into save file N, as choosing it in the launcher does.
//   With neither a ROM nor --game it opens the launcher, which asks for a ROM through its ADD ROM
//   row rather than putting a file dialog on screen before anything is drawn.
//   --frames N exits after N frames (tests run the app under SDL_VIDEO_DRIVER=dummy).
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "core/gb.h"
#include "core/sha1.h"
#include "hw/render.h"
#include "platform/setup.h"
#include "platform/archive.h"
#include "platform/error_log.h"
#include "platform/window.h"
#include "platform/png.h"
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
#include "ui/home.h"
#include "gui/screens.h"
#include "platform/sync_client.h"
#include "platform/haptic.h"
#include "sync/http.h"
#include "sync/update.h"

#ifdef __ANDROID__
#define ON_PHONE true           // the settings show Vibration and leave out Fullscreen
#else
#define ON_PHONE false
#endif
// what the update notice downloads: this system's file in the release
#if defined(__ANDROID__)
#define RELEASE_ASSET "Oracles-android.apk"
#elif defined(_WIN32)
#define RELEASE_ASSET "Oracles-windows-x64.zip"
#elif defined(__APPLE__)
#define RELEASE_ASSET "Oracles-macOS.dmg"
#else
#define RELEASE_ASSET "Oracles-linux-x86_64.AppImage"
#endif
#include "wide/wide.h"
#include "wide/hud.h"
#include "ui/labels.h"
#include "wide/npcs.h"
#include "mods/mods.h"
#include "mods/picture.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define AUDIO_TARGET_BYTES (APU_SAMPLE_RATE / 10 * 4)
#define ASSETS_VERSION 1

// id: the ROM's folder in the cache. A save made on one release would not load on the other
// (the European one keeps a language and a signature in it), so each keeps its own.
static const struct { const char *sha1, *id; } known[] = {
  {"880374fb978b18af4aa529e2e32f7ffb4d7dd2f4", "ages"},
  {"ba1268290fb2b1b70505d2d7b5825fc8a4816a4b", "seasons"},
  {"9f2dff3c46406839f839a8d535353dfc1e3e8670", "ages-eu"},        // Europe (En,Fr,De,Es,It)
  {"360b9408e569ba826212eac1ebea1b09573cb48c", "seasons-eu"},     // Europe (En,Fr,De,Es,It)
};

// A ROM folder's game: "ages-eu" is Ages.
static bool is_ages(const char *id) { return !strncmp(id, "ages", 4); }

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
    if (strcmp(hex, known[i].sha1) == 0) return known[i].id;
  fprintf(stderr, "ROM sha1 %s is not Oracle of Ages or Seasons (USA or Europe)\n", hex);
  return NULL;
}

static bool cached(const char *cache, const char *game);

// active.txt: per game, the ROM folder its tab starts ("ages ages-eu"), so adding the other
// release switches to it and leaves the first one's saves where they are.
static void active_set(const char *cache, const char *id) {
  char path[1100], ids[2][16] = {"", ""};
  snprintf(path, sizeof path, "%sactive.txt", cache);
  FILE *f = fopen(path, "r");
  if (f) {
    char a[16], b[16];
    while (fscanf(f, "%15s %15s", a, b) == 2) snprintf(ids[is_ages(a) ? 0 : 1], sizeof ids[0], "%s", b);
    fclose(f);
  }
  snprintf(ids[is_ages(id) ? 0 : 1], sizeof ids[0], "%s", id);
  if (!(f = fopen(path, "w"))) return;
  if (ids[0][0]) fprintf(f, "ages %s\n", ids[0]);
  if (ids[1][0]) fprintf(f, "seasons %s\n", ids[1]);
  fclose(f);
}

// The ROM folder a game's tab uses: the one active.txt names if it is installed, else whichever
// release is installed (the USA one first), else the USA folder (shown as not installed).
static const char *rom_id(const char *cache, int ages) {
  static char out[2][16];
  const char *us = ages ? "ages" : "seasons", *eu = ages ? "ages-eu" : "seasons-eu";
  char path[1100], a[16], b[16];
  snprintf(path, sizeof path, "%sactive.txt", cache);
  FILE *f = fopen(path, "r");
  char *o = out[ages ? 0 : 1];
  o[0] = 0;
  if (f) {
    while (fscanf(f, "%15s %15s", a, b) == 2)
      if (!strcmp(a, ages ? "ages" : "seasons") && (!strcmp(b, us) || !strcmp(b, eu)) && cached(cache, b)) snprintf(o, 16, "%s", b);
    fclose(f);
  }
  if (!o[0]) snprintf(o, 16, "%s", cached(cache, us) || !cached(cache, eu) ? us : eu);
  return o;
}

// The cache holds, per game, the ROM image with its code bytes zeroed, the cycle table built
// from the original bytes, and a manifest naming both.
// `err` takes a short reason the status line can show, since the player picking a file in the app
// never sees stderr.
static bool extract(const char *rom_path, const char *cache, char *game_out, char *err, size_t err_size) {
  size_t n, raw_n;
  // Read through SDL, not stdio: on Android the picker returns a content:// URI, which only
  // SDL_IOFromFile can open -- the app asks for no storage permission and has no path to open.
  uint8_t *raw = SDL_LoadFile(rom_path, &raw_n);
  if (!raw) {
    snprintf(err, err_size, "CANNOT READ FILE");
    fprintf(stderr, "cannot read %s: %s\n", rom_path, SDL_GetError());
    return false;
  }
  uint8_t *rom = archive_rom_from_memory(raw, raw_n, &n, err, err_size);
  SDL_free(raw);
  if (!rom) { fprintf(stderr, "cannot read %s: %s\n", rom_path, *err ? err : "unreadable"); return false; }
  const char *game = game_of(rom, n);
  if (!game) { snprintf(err, err_size, "NOT AN ORACLES ROM"); free(rom); return false; }
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
  snprintf(manifest, sizeof manifest, "oracles-assets %d\ngame %s\nregion %s\nrom_sha1 %s\nsize %zu\ncode_bytes_zeroed %zu\n", ASSETS_VERSION, is_ages(game) ? "ages" : "seasons", strchr(game, '-') ? "eu" : "us", hex, n, zeroed);
  snprintf(path, sizeof path, "%s/manifest.txt", dir);
  ok = ok && write_file(path, manifest, strlen(manifest));
  if (ok) fprintf(stderr, "extracted %s: %zu code bytes zeroed, cache %s\n", game, zeroed, dir);
  else { fprintf(stderr, "cannot write the cache in %s\n", dir); snprintf(err, err_size, "CANNOT SAVE CACHE"); }
  strcpy(game_out, game);
  free(rom); free(tab);
  if (ok) active_set(cache, game);
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
static char app_cache[1024];
static char error_report[ERROR_LOG_SIZE];

static void error_log_refresh(void) {
  settings.error_log = error_log_read(app_cache, error_report);
}


// The device that sent the last input decides what the screen calls the buttons (touch_on covers
// touch), and the last pad's type what Auto means.
static bool touch_on, pad_last;
static PadStyle pad_seen = STYLE_AUTO;

static PadStyle style_of_pad(SDL_JoystickID id) {
  switch (SDL_GetGamepadTypeForID(id)) {
  case SDL_GAMEPAD_TYPE_PS3: case SDL_GAMEPAD_TYPE_PS4: case SDL_GAMEPAD_TYPE_PS5: return STYLE_PLAYSTATION;
  case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO: case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_LEFT:
  case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT: case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_PAIR: return STYLE_NINTENDO;
  default: return STYLE_XBOX;
  }
}
static InputDevice input_device(void) { return touch_on ? DEVICE_TOUCH : pad_last ? DEVICE_PAD : DEVICE_KEYBOARD; }
static PadStyle pad_style(void) { return pad_style_resolve(settings.pad_style, pad_seen); }

static uint8_t joy_of(Action a) {
  static const uint8_t bits[ACTIONS] = {JOY_UP, JOY_DOWN, JOY_LEFT, JOY_RIGHT, JOY_A, JOY_B, JOY_START, JOY_SELECT, 0, 0, 0, 0, 0, 0, 0};
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
  const char *styled = pad_style_name(pad_style(), button);
  if (styled) return styled;
  if (button == PAD_RIGHT_TRIGGER) return "RT";
  if (button == PAD_LEFT_TRIGGER) return "LT";
  const char *s = SDL_GetGamepadStringForButton((SDL_GamepadButton)button);
  snprintf(buf, sizeof buf, "%s", s ? s : "?");
  for (char *p = buf; *p; p++) *p = (char)SDL_toupper(*p);
  return buf;
}

static uint8_t live_joy, slot_joy[2], stick_joy, stick_pressed, triggers_held;
static TapLatch taps;
static int trigger_down = NO_BINDING, trigger_up = NO_BINDING;
static uint8_t live_input(void *ctx, uint64_t frame) { (void)ctx; (void)frame; return live_joy | slot_joy[0] | slot_joy[1] | stick_joy; }

#define STICK_DEADZONE 16000

static void stick_event(const SDL_Event *ev) {
  uint8_t old = stick_joy;
  if (ev->type == SDL_EVENT_GAMEPAD_REMOVED) stick_joy = triggers_held = 0;
  else if (ev->type == SDL_EVENT_GAMEPAD_AXIS_MOTION && ev->gaxis.axis == SDL_GAMEPAD_AXIS_LEFTX)
    stick_joy = (uint8_t)((stick_joy & ~(JOY_LEFT | JOY_RIGHT)) | (ev->gaxis.value < -STICK_DEADZONE ? JOY_LEFT : ev->gaxis.value > STICK_DEADZONE ? JOY_RIGHT : 0));
  else if (ev->type == SDL_EVENT_GAMEPAD_AXIS_MOTION && ev->gaxis.axis == SDL_GAMEPAD_AXIS_LEFTY)
    stick_joy = (uint8_t)((stick_joy & ~(JOY_UP | JOY_DOWN)) | (ev->gaxis.value < -STICK_DEADZONE ? JOY_UP : ev->gaxis.value > STICK_DEADZONE ? JOY_DOWN : 0));
  stick_pressed = stick_joy & (uint8_t)~old;
  trigger_down = trigger_up = NO_BINDING;
  if (ev->type != SDL_EVENT_GAMEPAD_AXIS_MOTION) return;
  bool left = ev->gaxis.axis == SDL_GAMEPAD_AXIS_LEFT_TRIGGER;
  if (!left && ev->gaxis.axis != SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) return;
  uint8_t bit = left ? 2 : 1;
  bool down = ev->gaxis.value > STICK_DEADZONE;
  if (down == !!(triggers_held & bit)) return;
  triggers_held ^= bit;
  *(down ? &trigger_down : &trigger_up) = left ? PAD_LEFT_TRIGGER : PAD_RIGHT_TRIGGER;
}

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

// The three normal files share one SRAM image. Keep dates per slot, tied to its contents so a
// replaced/imported/synced SRAM cannot reuse a stale date. Older files fall back to SRAM's mtime;
// if several are tied, --latest leaves the choice to the player. This metadata is local only.
typedef struct { char hash[41]; SDL_Time time; } SaveDate;

static void save_dates(const char *path, const uint8_t *sram, size_t size, bool ages, SaveDate out[UI_FILES]) {
  SDL_PathInfo info;
  SDL_Time fallback = SDL_GetPathInfo(path, &info) ? info.modify_time : 0;
  memset(out, 0, sizeof(SaveDate) * UI_FILES);
  for (int i = 0; i < UI_FILES; i++) {
    const uint8_t *file = ui_file_data(sram, size, ages, i);
    if (file) { sha1_hex(file, UI_FILE_SIZE, out[i].hash); out[i].time = fallback; }
  }
  char meta[1400];
  snprintf(meta, sizeof meta, "%s.dates", path);
  FILE *f = fopen(meta, "r");
  if (!f) return;
  char line[128], hash[41];
  int slot;
  long long time;
  while (fgets(line, sizeof line, f))
    if (sscanf(line, "%d %40s %lld", &slot, hash, &time) == 3 && slot >= 0 && slot < UI_FILES &&
        time >= 0 && time <= fallback && !strcmp(hash, out[slot].hash)) out[slot].time = (SDL_Time)time;
  fclose(f);
}

static void save_sram(GB *gb, const char *path) {
  if (!gb->eram_size) return;
  size_t n = 0;
  uint8_t *old = oracles_read_file(path, &n);
  // Periodic flushes and quitting must not make an unchanged save newer than a save state.
  if (old && n == gb->eram_size && !memcmp(old, gb->eram, n)) { free(old); return; }
  bool ages = gb->rom_size > 0x150 && !memcmp(gb->rom + 0x134, "ZELDA NAYRU", 11);
  SaveDate dates[UI_FILES];
  save_dates(path, old, n, ages, dates);
  free(old);
  if (!write_file(path, gb->eram, gb->eram_size)) return;
  SDL_PathInfo info;
  if (!SDL_GetPathInfo(path, &info)) return;
  char meta[1400], text[256];
  int len = 0;
  for (int i = 0; i < UI_FILES; i++) {
    const uint8_t *file = ui_file_data(gb->eram, gb->eram_size, ages, i);
    if (!file) continue;
    char hash[41];
    sha1_hex(file, UI_FILE_SIZE, hash);
    SDL_Time time = !strcmp(hash, dates[i].hash) ? dates[i].time : info.modify_time;
    len += snprintf(text + len, sizeof text - len, "%d %s %lld\n", i, hash, (long long)time);
  }
  snprintf(meta, sizeof meta, "%s.dates", path);
  write_file(meta, text, (size_t)len);
}

static bool poll_event(SDL_Event *ev);
static void show_status(const char *msg);

// .7z is offered so picking one explains itself rather than being greyed out with no reason.
static bool pick_rom(char *game_out, const char *cache) {
  static const SDL_DialogFileFilter filters[] = {{"ROM or archive", "gbc;gb;zip;gz;7z"}};
  pick_state = 0;
#ifdef __ANDROID__
  (void)filters;
  SDL_ShowOpenFileDialog(on_pick, NULL, NULL, NULL, 0, NULL, false);
#else
  SDL_ShowOpenFileDialog(on_pick, NULL, NULL, filters, 1, NULL, false);
#endif
  while (!pick_state) { SDL_Event ev; while (poll_event(&ev)) if (ev.type == SDL_EVENT_QUIT) return false; SDL_Delay(10); }
  if (pick_state < 0) return false;                 // the player cancelled the dialog
  char err[40] = {0};
  if (extract(picked, cache, game_out, err, sizeof err)) return true;
  show_status(*err ? err : "Can't add that ROM");
  return false;
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

// Saving over a slot keeps what was there as state_N.undo (and its thumbnail), until the next save.
static void slot_keep_undo(const char *dir, int slot) {
  char from[1300], to[1300];
  for (int k = 0; k < 2; k++) {
    const char *ext = k ? ".thumb" : "";
    slot_path(from, sizeof from, dir, slot, ext);
    snprintf(to, sizeof to, "%s.undo", from);
    SDL_RemovePath(to);
    if (file_exists(from)) SDL_RenamePath(from, to);
  }
}

// Puts back what the last save replaced; a save into an empty slot leaves no .undo, so undoing it
// empties the slot again.
static bool slot_undo(const char *dir, int slot) {
  char from[1300], to[1300];
  bool ok = true;
  for (int k = 0; k < 2; k++) {
    slot_path(to, sizeof to, dir, slot, k ? ".thumb" : "");
    snprintf(from, sizeof from, "%s.undo", to);
    SDL_RemovePath(to);
    if (file_exists(from)) ok = SDL_RenamePath(from, to) && ok;
  }
  return ok;
}

static bool slot_save(GB *gb, const char *dir, int slot, const uint8_t *rgb) {
  char path[1300];
  // Flush normal saves first: this state is the newer, complete snapshot of the same progress.
  snprintf(path, sizeof path, "%s/sram.sav", dir);
  save_sram(gb, path);
  slot_path(path, sizeof path, dir, slot, "");
  if (slot != SLOT_AUTO) slot_keep_undo(dir, slot);
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
  wide_npcs_reset();
  return true;
}

// The copies of the game that walk into the rooms beside the screen (wide/npcs.h) run here, so
// the game never waits for one.
static SDL_Semaphore *npcs_wake;

static int npcs_thread(void *arg) {
  (void)arg;
  SDL_SetCurrentThreadPriority(SDL_THREAD_PRIORITY_LOW);
  for (;;) {
    SDL_WaitSemaphore(npcs_wake);
    wide_npcs_work();
  }
  return 0;
}

static void npcs_signal(void) {
  if (!npcs_wake) {
    npcs_wake = SDL_CreateSemaphore(0);
    // the copy's kernel runs on this stack as the game's does on the main thread's
    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetPointerProperty(props, SDL_PROP_THREAD_CREATE_ENTRY_FUNCTION_POINTER, (void *)npcs_thread);
    SDL_SetStringProperty(props, SDL_PROP_THREAD_CREATE_NAME_STRING, "npcs");
    SDL_SetNumberProperty(props, SDL_PROP_THREAD_CREATE_STACKSIZE_NUMBER, 8 << 20);
    SDL_DetachThread(SDL_CreateThreadWithProperties(props));
    SDL_DestroyProperties(props);
  }
  SDL_SignalSemaphore(npcs_wake);
}

// A state file's time and thumbnail (black when it has none).
static void slot_read(UiSlot *s, const char *dir, int slot) {
  char path[1300];
  SDL_PathInfo info;
  slot_path(path, sizeof path, dir, slot, "");
  s->used = SDL_GetPathInfo(path, &info) && info.type == SDL_PATHTYPE_FILE;
  s->time = s->used ? info.modify_time : 0;
  memset(s->thumb, 0, sizeof s->thumb);
  if (!s->used) return;
  slot_path(path, sizeof path, dir, slot, ".thumb");
  size_t n;
  uint8_t *img = oracles_read_file(path, &n);
  if (img && n == sizeof s->thumb) memcpy(s->thumb, img, n);
  free(img);
}

static void slots_scan(SlotsMenu *m, const char *dir) {
  for (int i = 0; i < UI_SLOTS; i++) slot_read(&m->slots[i], dir, i);
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

// Portable mode: portable.txt beside the app (beside Oracles.app on a Mac, whose bundle must stay
// unmodified) keeps the data in oracles-data/ there instead of the user's app-data folder.
static bool portable_dir(char *out, size_t size) {
#ifdef __ANDROID__
  (void)out; (void)size;
  return false;
#else
  // an AppImage runs from a read-only mount; APPIMAGE is the file itself
  const char *appimage = getenv("APPIMAGE");
  if (appimage && strrchr(appimage, '/')) {
    snprintf(out, size, "%.*s", (int)(strrchr(appimage, '/') - appimage + 1), appimage);
    return true;
  }
  const char *base = SDL_GetBasePath();
  if (!base) return false;
  snprintf(out, size, "%s", base);
  char *bundle = strstr(out, ".app/Contents/");
  if (bundle) { *bundle = 0; char *slash = strrchr(out, '/'); if (slash) slash[1] = 0; }
  return true;
#endif
}

static bool portable_marker(char *out, size_t size) {
  char dir[1024];
  if (!portable_dir(dir, sizeof dir)) return false;
  snprintf(out, size, "%sportable.txt", dir);
  return true;
}

static bool portable_marked(void) {
  char marker[1100];
  return portable_marker(marker, sizeof marker) && file_exists(marker);
}

static SDL_EnumerationResult SDLCALL any_entry(void *found, const char *dir, const char *name) {
  (void)dir; (void)name;
  *(bool *)found = true;
  return SDL_ENUM_SUCCESS;
}

static bool dir_empty(const char *dir) {
  bool found = false;
  SDL_EnumerateDirectory(dir, any_entry, &found);
  return !found;
}

static bool copy_tree(const char *from, const char *to);

static SDL_EnumerationResult SDLCALL copy_entry(void *to, const char *dir, const char *name) {
  char src[1100], dst[1100];
  SDL_PathInfo info;
  snprintf(src, sizeof src, "%s%s", dir, name);
  snprintf(dst, sizeof dst, "%s%s", (const char *)to, name);
  if (!SDL_GetPathInfo(src, &info)) return SDL_ENUM_FAILURE;
  if (info.type == SDL_PATHTYPE_DIRECTORY) {
    strcat(src, "/");
    strcat(dst, "/");
    return copy_tree(src, dst) ? SDL_ENUM_CONTINUE : SDL_ENUM_FAILURE;
  }
  return SDL_CopyFile(src, dst) ? SDL_ENUM_CONTINUE : SDL_ENUM_FAILURE;
}

static bool copy_tree(const char *from, const char *to) {
  return SDL_CreateDirectory(to) && SDL_EnumerateDirectory(from, copy_entry, (void *)to);
}

// The data folder for this run, with pref the user's app-data folder. On the first run after Portable
// is switched on or off, the folder it switches to gets a copy of the other one, unless it already
// holds something: nothing is ever overwritten, and the old folder stays as it was.
static void data_folder(char *cache, size_t size, const char *pref) {
  char dir[1024], portable[1100];
  snprintf(cache, size, "%s", pref);
  if (!portable_dir(dir, sizeof dir)) return;
  snprintf(portable, sizeof portable, "%soracles-data/", dir);
  bool on = portable_marked();
  const char *from = on ? pref : portable;
  if (on) snprintf(cache, size, "%s", portable);
  bool copied = true;
  if (file_exists(from) && !dir_empty(from) && (!file_exists(cache) || dir_empty(cache))) {
    char dest[1100], part[1110], part_dir[1120];
    snprintf(dest, sizeof dest, "%.*s", (int)strlen(cache) - 1, cache);
    snprintf(part, sizeof part, "%s.part", dest);
    snprintf(part_dir, sizeof part_dir, "%s/", part);
    SDL_RemovePath(dest);
    copied = copy_tree(from, part_dir) && SDL_RenamePath(part, dest);
  }
  if (!copied || !SDL_CreateDirectory(cache)) {
    fprintf(stderr, "could not set up %s: %s\n", cache, SDL_GetError());
    snprintf(cache, size, "%s", pref);
    return;
  }
  // SDL_GetPrefPath creates the app-data folder and its organization folder; a portable run that
  // never used them takes them away again (SDL_RemovePath leaves a folder with anything in it)
  if (on) {
    char p[1024];
    snprintf(p, sizeof p, "%s", pref);
    for (int i = 0; i < 2; i++) {
      size_t n = strlen(p);
      while (n && (p[n - 1] == '/' || p[n - 1] == '\\')) p[--n] = 0;
      if (!n || !SDL_RemovePath(p)) break;
      while (n && p[n - 1] != '/' && p[n - 1] != '\\') p[--n] = 0;
    }
  }
}

static void apply_window_settings(SDL_Window *win) {
#ifdef __ANDROID__
  settings.fullscreen = true;
#endif
  if (!!(SDL_GetWindowFlags(win) & SDL_WINDOW_FULLSCREEN) != settings.fullscreen) SDL_SetWindowFullscreen(win, settings.fullscreen);
}

// Turning widescreen on or off reshapes a desktop window to the new picture at its current scale.
static void fit_window_to_picture(SDL_Window *win) {
#ifndef __ANDROID__
  if (SDL_GetWindowFlags(win) & SDL_WINDOW_FULLSCREEN) return;
  int w, h;
  if (!SDL_GetWindowSize(win, &w, &h)) return;
  int s = SDL_max(1, h / FB_H);
  SDL_SetWindowSize(win, (settings.widescreen ? 256 : FB_W) * s, FB_H * s);
#else
  (void)win;
#endif
}

static float volume_gain(bool muted) { return muted ? 0.0f : settings.volume / 10.0f; }

// A message shown over the screen for two seconds (state saved, item set, ...).
static char toast[40];
static uint64_t toast_until;
static const char *toast_action;            // the message's button (Undo, Restart), or NULL
static bool undo_tapped, restart_tapped;
static void show_status(const char *msg) {
  snprintf(toast, sizeof toast, "%s", msg);
  toast_until = SDL_GetTicks() + 2000;
  toast_action = NULL;
  undo_tapped = false;
}

// While set, the wait shown over everything (a sync the player has to wait for); it replaces the message.
static const char *waiting;

// A message with an Undo to tap, shown a little longer.
static void show_undoable(const char *msg) {
  show_status(msg);
  toast_until = SDL_GetTicks() + 4000;
  toast_action = "Undo";
}

// The Portable row writes or removes portable.txt; the data folder follows on the next start.
static void portable_switched(void) {
  char marker[1100];
  if (!portable_marker(marker, sizeof marker)) return;
  bool ok = settings.portable ? write_file(marker, "", 0) : SDL_RemovePath(marker);
  if (!ok) { settings.portable = !settings.portable; show_status("Can't write beside the app"); return; }
  show_status("Restart to change folder");
  toast_until = SDL_GetTicks() + 8000;
  toast_action = "Restart";
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
    show_status(slot ? "Item set on Y" : "Item set on X");
    return;
  }
  slot_joy[slot] = features_slot_press(gb, slot);
}

// The touch overlay: shown from the first touch until a controller or keyboard is used. The layout
// comes from the last present, and every finger holds the actions under it.
#define TEST_TOUCH_ID 0x7e57
#define FINGERS 10
static TouchLayout layout;
static bool touch_mouse;
static uint32_t touch_held, touch_pressed, touch_released;
static struct { SDL_FingerID id; uint32_t held; bool down; } fingers[FINGERS];
static UiTheme overlay_theme;            // the game's title-screen colours, behind widescreen's sides
static UiFont mod_font;                  // the ROM's dialogue font, for mods' overlay text

static void overlay_style(const UiTheme *theme) { overlay_theme = *theme; }

// The menus at screen resolution (src/gui). ui_layer, when set, declares the open menu into the
// frame present_frame draws; a full-screen page blocks the touch overlay so taps reach its rows.
// A tap becomes a button event (gui_button_event, code = UiButton) after moving the selection, so
// every menu keeps one code path for keys, pads and touch.
static GuiTheme gui_t;
static void (*ui_layer)(const GuiFrame *f);
static bool overlay_blocked;
static Uint32 gui_button_event;

static void push_button(int b) {
  if (b < 0 || !gui_button_event) return;
  SDL_Event ev = {0};
  ev.type = gui_button_event;
  ev.user.code = b;
  SDL_PushEvent(&ev);
}

typedef struct { void (*layer)(const GuiFrame *f); bool blocked; } UiScope;
static const uint8_t page_black[FB_W * FB_H * 3];

static UiScope ui_enter(void (*layer)(const GuiFrame *f), bool blocked) {
  UiScope was = {ui_layer, overlay_blocked};
  ui_layer = layer;
  overlay_blocked = blocked;
  return was;
}
static void ui_leave(UiScope was) { ui_layer = was.layer; overlay_blocked = was.blocked; }

static bool direct_touch(SDL_TouchID id) {
  return id == TEST_TOUCH_ID || (touch_mouse && id == SDL_MOUSE_TOUCHID) || SDL_GetTouchDeviceType(id) == SDL_TOUCH_DEVICE_DIRECT;
}

// A finger that came down on a menu row or button belongs to the menu, not the controls under it.
static SDL_FingerID menu_finger;
static bool menu_finger_down;

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
  if (menu_finger_down && ev->tfinger.fingerID == menu_finger) {
    if (up) menu_finger_down = false;
    return;
  }
  int f = -1;
  for (int i = 0; i < FINGERS && f < 0; i++) if (fingers[i].down && fingers[i].id == ev->tfinger.fingerID) f = i;
  for (int i = 0; i < FINGERS && f < 0 && down; i++) if (!fingers[i].down) f = i;
  if (f < 0) return;
  fingers[f].id = ev->tfinger.fingerID;
  fingers[f].down = !up;
  float x = ev->tfinger.x * layout.screen_w / layout.scale, y = ev->tfinger.y * layout.screen_h / layout.scale;
  fingers[f].held = up || overlay_blocked ? 0 : touch_hit(&layout, x, y);
  uint32_t held = 0;
  for (int i = 0; i < FINGERS; i++) held |= fingers[i].held;
  touch_pressed = held & ~touch_held;
  touch_released = touch_held & ~held;
  touch_held = held;
  // a tick for each control a finger comes onto, sliding across the D-pad included
  if (touch_pressed && settings.haptics) haptic_tick();
}

// Every loop polls through here, so the touch state sees each event once.
static bool poll_event(SDL_Event *ev) {
  if (!SDL_PollEvent(ev)) return false;
  bool finger = ev->type >= SDL_EVENT_FINGER_DOWN && ev->type <= SDL_EVENT_FINGER_CANCELED;
  if ((!finger || direct_touch(ev->tfinger.touchID)) && gui_event(ev, touch_mouse) && touch_on) {
    menu_finger = ev->tfinger.fingerID;
    menu_finger_down = true;
  }
  if (ev->type == SDL_EVENT_KEY_DOWN && ev->key.scancode != SDL_SCANCODE_AC_BACK) pad_last = false;
  else if (ev->type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) { pad_last = true; pad_seen = style_of_pad(ev->gbutton.which); }
  else if (ev->type == SDL_EVENT_GAMEPAD_AXIS_MOTION && (ev->gaxis.value > 16000 || ev->gaxis.value < -16000)) {
    pad_last = true;
    pad_seen = style_of_pad(ev->gaxis.which);
  }
  oracles_window_event(ev);
  touch_event(ev);
  stick_event(ev);
  if (ev->type == SDL_EVENT_DID_ENTER_FOREGROUND) sync_retry_soon();
  return true;
}

// Sharp and uncorrected frames go straight to the 160x144 texture; filters render at the layout's
// integer scale into a texture of that size, shown 1:1. A fractional (fill) scale renders at the
// next whole scale and shrinks that smoothly, so pixels stay sharp without uneven widths. The
// overlay is drawn in game pixels too.
#define MAX_FILTER_SCALE 12

static uint64_t test_tick;

// Test hook: ORACLES_SHOT="tick:path.png[,tick:path.png]" saves the window's pixels at the first frame
// drawn at or after each loop tick.
static void shot_if_due(SDL_Renderer *ren) {
  static const char *spec;
  static bool init;
  static uint32_t taken;
  if (!init) { init = true; spec = getenv("ORACLES_SHOT"); }
  int i = 0;
  for (const char *p = spec; p && *p; i++) {
    unsigned long long at;
    char path[512];
    int len = 0;
    if (sscanf(p, "%llu:%511[^,]%n", &at, path, &len) != 2) break;
    if (test_tick >= at && i < 32 && !(taken >> i & 1)) {
      taken |= 1u << i;
      SDL_Surface *raw = SDL_RenderReadPixels(ren, NULL);
      SDL_Surface *rgb = raw ? SDL_ConvertSurface(raw, SDL_PIXELFORMAT_RGB24) : NULL;
      if (rgb) {
        uint8_t *packed = malloc((size_t)rgb->w * rgb->h * 3);
        for (int y = 0; y < rgb->h; y++) memcpy(packed + (size_t)y * rgb->w * 3, (uint8_t *)rgb->pixels + (size_t)y * rgb->pitch, (size_t)rgb->w * 3);
        png_write_rgb(path, packed, rgb->w, rgb->h);
        free(packed);
      }
      SDL_DestroySurface(rgb);
      SDL_DestroySurface(raw);
    }
    p += len;
    if (*p == ',') p++;
  }
}

// A picture's textures: straight to the screen, or through the filter at an integer scale first.
typedef struct { SDL_Texture *direct, *big; int direct_w, big_scale, big_w; uint8_t *buf; } PicTextures;

static void draw_pic(SDL_Renderer *ren, PicTextures *t, const uint8_t *rgb, int pic_w, int scale, bool fractional, ScreenFilter filter,
                     const SDL_FRect *dst) {
  if (!fractional && (filter == FILTER_SHARP || scale < 2) && !settings.gbc_colours) {
    if (!t->direct || t->direct_w != pic_w) {
      if (t->direct) SDL_DestroyTexture(t->direct);
      t->direct = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, pic_w, FB_H);
      SDL_SetTextureScaleMode(t->direct, SDL_SCALEMODE_NEAREST);
      t->direct_w = pic_w;
    }
    SDL_UpdateTexture(t->direct, NULL, rgb, pic_w * 3);
    SDL_RenderTexture(ren, t->direct, NULL, dst);
    return;
  }
  if (!t->big || t->big_scale != scale || t->big_w != pic_w) {
    if (t->big) SDL_DestroyTexture(t->big);
    t->big = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, pic_w * scale, FB_H * scale);
    free(t->buf);
    t->buf = malloc((size_t)pic_w * scale * FB_H * scale * 3);
    t->big_scale = scale;
    t->big_w = pic_w;
  }
  SDL_SetTextureScaleMode(t->big, fractional ? SDL_SCALEMODE_LINEAR : SDL_SCALEMODE_NEAREST);
  ui_filter(rgb, pic_w, settings.gbc_colours, scale < 2 ? FILTER_SHARP : filter, scale, t->buf);
  SDL_UpdateTexture(t->big, NULL, t->buf, pic_w * scale * 3);
  SDL_RenderTexture(ren, t->big, NULL, dst);
}

// Item X and Y beside the narrow picture, set by show_game for the frame it presents: the boxes
// where they fit, else the narrow strips; strips_shown says whether the last frame had the room.
static const uint8_t *box_rgb[2], *strip_rgb[2];
static bool strips_shown;

// Draws a picture pic_w (160 or WIDE_W) x 144 at the layout's place and scale, then the overlay.
static void present_frame(SDL_Renderer *ren, const uint8_t *rgb, int pic_w) {
  static PicTextures pic, strip[2];
  int w = pic_w, h = FB_H;
  SDL_GetCurrentRenderOutputSize(ren, &w, &h);
  SDL_Rect safe = {0, 0, w, h};
  SDL_GetRenderSafeArea(ren, &safe);
  touch_layout(&layout, w, h, (UiRect){safe.x, safe.y, safe.w, safe.h}, touch_on, settings.four_slots, settings.fill, pic_w);
  static int logged_w, logged_h, logged_scale;
  if (w != logged_w || h != logged_h || layout.scale != logged_scale) {
    fprintf(stderr, "screen %dx%d, safe area %d,%d %dx%d, scale %d\n", w, h, safe.x, safe.y, safe.w, safe.h, layout.scale);
    logged_w = w; logged_h = h; logged_scale = layout.scale;
  }
  bool fractional = layout.game_px.w % pic_w != 0;
  int scale = SDL_min((layout.game_px.w + pic_w - 1) / pic_w, MAX_FILTER_SCALE);
  SDL_FRect dst = {(float)layout.game_px.x, (float)layout.game_px.y, (float)layout.game_px.w, (float)layout.game_px.h};
  if (touch_on) SDL_SetRenderDrawColor(ren, gui_t.ink.r, gui_t.ink.g, gui_t.ink.b, 255);
  SDL_RenderClear(ren);
  SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
  if (touch_on) SDL_RenderFillRect(ren, &dst);
  draw_pic(ren, &pic, rgb, pic_w, scale, fractional, settings.filter, &dst);
  strips_shown = false;
  for (int k = 0; k < 2 && !strips_shown && strip_rgb[0] && pic_w == FB_W && !touch_on; k++) {
    const uint8_t *const *src = k ? strip_rgb : box_rgb;
    int sw_px = k ? WIDE_STRIP_W : WIDE_BOX_W;
    float sw = dst.w * sw_px / pic_w;
    SDL_FRect left = {dst.x - sw, dst.y, sw, dst.h}, right = {dst.x + dst.w, dst.y, sw, dst.h};
    if (left.x < safe.x || right.x + sw > safe.x + safe.w) continue;
    // CRT bends and darkens a picture towards its own edges, which on a strip this narrow is all of it
    ScreenFilter f = settings.filter == FILTER_CRT ? FILTER_SHARP : settings.filter;
    draw_pic(ren, &strip[0], src[0], sw_px, scale, fractional, f, &left);
    draw_pic(ren, &strip[1], src[1], sw_px, scale, fractional, f, &right);
    strips_shown = true;
  }
  if (touch_on && !overlay_blocked) gui_pad(&layout, touch_held, &gui_t);
  bool toast_on = toast[0] && SDL_GetTicks() < toast_until;
  if (ui_layer || toast_on || waiting) {
    GuiFrame f = {{(float)layout.game_px.x, (float)layout.game_px.y, (float)layout.game_px.w, (float)layout.game_px.h},
                  {(float)safe.x, (float)safe.y, (float)safe.w, (float)safe.h}, (float)w, (float)h, !touch_on, NULL, NULL};
    menu_hint_keys(input_device(), pad_style(), &f.ok_key, &f.back_key);
    if (ui_layer) ui_layer(&f);
    else gui_begin(gui_display_scale());
    if (waiting) gui_wait(waiting, SDL_GetTicks(), &gui_t, &f);
    else if (toast_on && gui_toast(toast, toast_action, &gui_t, &f)) {
      if (toast_action && !strcmp(toast_action, "Restart")) { restart_tapped = true; SDL_PushEvent(&(SDL_Event){.type = SDL_EVENT_QUIT}); }
      else undo_tapped = true;
    }
    gui_end();
    if (gui_take_tap() && settings.haptics) haptic_tick();
  } else gui_none();
  shot_if_due(ren);
  SDL_RenderPresent(ren);
}

static void present(SDL_Renderer *ren, SDL_Texture *tex, const uint8_t *rgb);

// The current room, bank 0 ($c000..$cfff) at the same low-12-bit offset test_wide.c's
// neighbour_cb and test_room.c's record_cb already read (`w[seasons ? 0xc4c : 0xc30]` there, for
// `w = sample->wram[0]`) -- not a real-looking 4-digit $Cxxx address, but the low 12 bits of one
// ($C000 + 0xc4c == $CC4C), which is what indexing a bank-0 array that starts at $C000 wants.
// Kept identical to that convention on purpose: an off-by-a-bank slip here would make
// mods_frame's on_room_change fire at the wrong moments. app.c is still not linked into test_mods
// (the engine stays out of oraclesmods by design) and test_wide/test_room are still multi-hour ROM
// replays that never call through app.c at all, so neither pins this particular line. The
// `mods_room` ctest (tests/test_mods_room.py) closes that gap through the real frame path instead:
// it drives oracles-native through the start of the known-good Seasons movie with a mod watching
// on_room_change, and asserts the handler fires with a room other than the pre-game default.
// Confirmed by mutation: swapping the ternary's two branches is caught by that test (it never sees
// a second, non-zero room) while leaving ctest's fast suite and tests/test_app_menus.py unaffected.
static int rd_room(const GBSample *sample, bool seasons) {
  return sample->wram[0][seasons ? 0xc4c : 0xc30];
}

// The room's group, at the same low-12-bit bank-0 convention as rd_room above -- confirmed
// against two independent existing readers of this exact address pair: tests/test_room.c's
// record_cb (`w[r->seasons ? 0xc49 : 0xc2d]`) and this file's own write_crash_log a few hundred
// lines down (`gb->wram[0][(ages ? 0xcc2d : 0xcc49) - 0xc000]`, i.e. the same offset written as a
// full $Cxxx address minus $C000). All three agree, so this is not a guess.
static int rd_room_group(const GBSample *sample, bool seasons) {
  return sample->wram[0][seasons ? 0xc49 : 0xc2d];
}

// wRoomWidth/wRoomHeight, in 8x8 tiles -- unlike rd_room/rd_room_group above, these sit at the
// same address in both games (see the plan's verified-facts table), so no seasons ternary.
static int rd_room_w(const GBSample *sample) { return sample->wram[0][0xd0a]; }
static int rd_room_h(const GBSample *sample) { return sample->wram[0][0xd0b]; }

static void hud_glyphs(uint8_t g[4]) {
  InputDevice d = input_device();
  PadStyle style = pad_style();
  for (int i = 0; i < HUD_SLOTS; i++) g[i] = hud_slot_glyph(d, style, &settings.bindings, (HudSlot)i);
}

// The game's picture as shown: in widescreen the room around the camera fills the sides (the
// message stays centred), the status bar names the buttons in use and the mods' overlay is on top.
// rgb itself is left as the game drew it.
static void show_game(SDL_Renderer *ren, const GBSample *sample, const uint8_t *rgb, bool seasons, const uint8_t *rom, size_t rom_size) {
  static uint8_t pic[WIDE_W * FB_H * 3], boxes[2][WIDE_BOX_W * FB_H * 3], strips[2][WIDE_STRIP_W * FB_H * 3];
  int w = settings.widescreen ? WIDE_W : FB_W;
  // The X and Y items go in the space widescreen adds beside the status bar, or in the narrow
  // picture's side bars when they are wide enough; the game has nowhere to put them.
  WideOptions o = {seasons, settings.dim_sides, {overlay_theme.bg.r, overlay_theme.bg.g, overlay_theme.bg.b}, rom, rom_size,
                   settings.four_slots, {features.slot_item[0], features.slot_item[1]}, wide_npcs_room, {0}};
  hud_glyphs(o.glyph);
  if (!settings.widescreen) {
    memcpy(pic, rgb, FB_W * FB_H * 3);
    hud_relabel(sample, o.glyph[HUD_B], o.glyph[HUD_A], pic);
    if (wide_slot_strip(sample, &o, 0, true, boxes[0]) && wide_slot_strip(sample, &o, 1, true, boxes[1]) &&
        wide_slot_strip(sample, &o, 0, false, strips[0]) && wide_slot_strip(sample, &o, 1, false, strips[1]))
      for (int i = 0; i < 2; i++) { box_rgb[i] = boxes[i]; strip_rgb[i] = strips[i]; }
  } else wide_render(sample, &o, pic);
  // the overlay was drawn for the width the mods last saw, which a paused game can have changed since
  const ModOverlay *ov = mods_overlay();
  if (ov->width == w)
    for (int y = 0; y < FB_H; y++)
      for (int x = 0; x < w; x++)
        if (overlay_cover_at(ov, x, y)) memcpy(pic + ((size_t)y * w + x) * 3, overlay_rgb_at(ov, x, y), 3);
  present_frame(ren, pic, w);
  box_rgb[0] = box_rgb[1] = strip_rgb[0] = strip_rgb[1] = NULL;
  // With the boxes on screen the B box must keep showing the B item, so holding X or Y should not
  // repaint it with the item it lends to the B slot.
  features.slot_boxes = settings.four_slots && (settings.widescreen || strips_shown);
}

// The running game's paused picture: menus drawn over it show it as the game frame it was.
static struct { const uint8_t *rgb; const GB *gb; bool seasons; const uint8_t *rom; size_t rom_size; } paused_game;

// A game frame.
static void present_game(SDL_Renderer *ren, const GBSample *sample, const uint8_t *rgb, bool seasons, const uint8_t *rom, size_t rom_size) {
  // Mods draw before the filters, so an overlay scales with the picture and takes the chosen
  // filter rather than sitting on top of it looking sharp. Run here, before the widescreen/narrow
  // split in show_game, so a mod runs in both.
  // Designated initializers, not positional: this struct already grew once since the launcher
  // first built it positionally, and a positional init here would have silently misassigned the
  // moment that happened -- see mods.h's own ModsFrame comment and tests/test_mods.c's identical
  // reasoning for Task 4 of the Stage 1 plan. `sample` is never NULL here (present_game's only
  // caller already dereferences it unguarded a line above), so wram[0]/wram[3] below are always
  // valid, live pointers -- never the NULL case the struct's own comment documents for "before
  // any game is running".
  int scroll_x, scroll_y, origin_x = 0, origin_y = 0;
  mods_picture_scroll(sample, &scroll_x, &scroll_y);
  bool origin = wide_play_origin(sample, seasons, &origin_x, &origin_y);
  ModsFrame mf = {
    .wram = sample->wram[0],
    .wram3 = sample->wram[3],
    .wram1 = sample->wram[1],
    .hram = sample->hram,
    .seasons = seasons,
    .width = settings.widescreen ? WIDE_W : FB_W,
    .room = rd_room(sample, seasons),
    .room_w = rd_room_w(sample),
    .room_h = rd_room_h(sample),
    .room_group = rd_room_group(sample, seasons),
    .bg_pal = mods_picture_bg_pal(sample),
    .oam = mods_picture_oam(sample),
    .scroll = true,
    .scroll_x = scroll_x,
    .scroll_y = scroll_y,
    .origin = origin,
    .origin_x = origin_x,
    .origin_y = origin_y,
    .rom = rom,
    .rom_size = rom_size,
    .font = &mod_font,
  };
  mods_frame(&mf);
  show_game(ren, sample, rgb, seasons, rom, rom_size);
}

// What mods wrote with oracles.wr this frame, between frames as the headless POKE does.
static void apply_mod_writes(GB *gb) {
  ModWrite w[MODS_WRITES_MAX];
  int n = mods_take_writes(w, MODS_WRITES_MAX);
  for (int i = 0; i < n; i++) gb->wram[w[i].addr >> 12 & 1][w[i].addr & 0xfff] = w[i].value;
}

// A 160-wide picture (a menu, or the game where the sides are not drawn): with the current message,
// and in widescreen centred on the theme's border colour.
static void present(SDL_Renderer *ren, SDL_Texture *tex, const uint8_t *rgb) {
  (void)tex;
  if (paused_game.gb && paused_game.gb->sample && rgb == paused_game.rgb) {
    show_game(ren, paused_game.gb->sample, rgb, paused_game.seasons, paused_game.rom, paused_game.rom_size);
    return;
  }
  if (!settings.widescreen) { present_frame(ren, rgb, FB_W); return; }
  static uint8_t wide[WIDE_W * FB_H * 3];
  const int side = (WIDE_W - FB_W) / 2;
  for (int y = 0; y < FB_H; y++) {
    uint8_t *row = wide + (size_t)y * WIDE_W * 3;
    for (int x = 0; x < WIDE_W; x++)
      if (x < side || x >= side + FB_W) { row[x * 3] = overlay_theme.bg.r; row[x * 3 + 1] = overlay_theme.bg.g; row[x * 3 + 2] = overlay_theme.bg.b; }
    memcpy(row + side * 3, rgb + (size_t)y * FB_W * 3, FB_W * 3);
  }
  present_frame(ren, wide, WIDE_W);
}

// A modal settings screen; returns false when the window is closed. Changes apply as they are made.
static bool run_settings(SDL_Window *win, SDL_Renderer *ren, SDL_Texture *tex, const uint8_t *game_rgb, SDL_AudioStream *audio);

// Installed games, their save files and save state, and each game's colours.
static void launcher_load(Launcher *l, const char *cache) {
  memset(l->games, 0, sizeof l->games);
  for (int g = 0; g < UI_GAMES; g++) {
    LauncherGame *lg = &l->games[g];
    ui_theme_default(&lg->theme, g == UI_GAME_AGES);
    const char *id = rom_id(cache, g == UI_GAME_AGES);
    if (!cached(cache, id)) continue;
    lg->installed = true;
    char path[1300];
    size_t n;
    snprintf(path, sizeof path, "%s%s/sram.sav", cache, id);
    uint8_t *sram = oracles_read_file(path, &n);
    ui_read_files(sram, sram ? n : 0, g == UI_GAME_AGES, lg->files);
    free(sram);
    char dir[1100];
    snprintf(dir, sizeof dir, "%s%s", cache, id);
    migrate_state(dir);
    slot_path(path, sizeof path, dir, SLOT_AUTO, "");
    lg->has_resume = file_exists(path);
    snprintf(path, sizeof path, "%s%s/rom.bin", cache, id);
    uint8_t *rom = oracles_read_file(path, &n);
    ui_theme_load(&lg->theme, rom, rom ? n : 0, g == UI_GAME_AGES);
    free(rom);
  }
}

// Test hook: ORACLES_TEST_KEYS="120:Escape,130:Down,140:X" presses each key at that loop tick (every
// pass of the launcher, menu or game loop is a tick), so ctest can drive the menus headless.
// "conflict+20:Left" counts from the tick the app last passed test_mark("conflict") instead, for
// screens that open after something the tick count cannot predict (a sync over the network).
static struct { char name[16]; uint64_t tick; } test_marks[4];

static void test_mark(const char *name) {
  int i = 0;
  while (i < 3 && test_marks[i].name[0] && strcmp(test_marks[i].name, name)) i++;
  snprintf(test_marks[i].name, sizeof test_marks[i].name, "%s", name);
  test_marks[i].tick = test_tick + 1;
}

static bool test_key_tick(const char *p, unsigned long long *at, char name[32], int *len) {
  char mark[16];
  if (sscanf(p, "%15[a-z]+%llu:%31[^,]%n", mark, at, name, len) == 3) {
    for (int i = 0; i < 4; i++)
      if (!strcmp(test_marks[i].name, mark)) { *at += test_marks[i].tick; return true; }
    *at = 0;
    return true;
  }
  return sscanf(p, "%llu:%31[^,]%n", at, name, len) == 2;
}

// "#A", "#UP", ... in ORACLES_TEST_KEYS touch that control of the overlay (one finger).
static void test_touch(const char *name, bool down) {
  static const char *const names[ACTIONS] = {"UP", "DOWN", "LEFT", "RIGHT", "A", "B", "START", "SELECT", "PAUSE", "FAST", "SWAP", "X", "Y", "SAVE", "LOAD"};
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

// "^Slot.0", "^Back" in ORACLES_TEST_KEYS tap that element of the last menu drawn (one finger),
// so tests drive the menus the way a phone does.
static void test_tap(const char *name, bool down) {
  char id[32];
  int index = 0;
  if (sscanf(name, "%31[^.].%d", id, &index) < 1) return;
  Clay_ElementId eid = Clay_GetElementIdWithIndex((Clay_String){.length = (int32_t)strlen(id), .chars = id}, (uint32_t)index);
  Clay_ElementData e = Clay_GetElementData(eid);
  if (!e.found || !gui_tappable(eid) || layout.screen_w <= 0 || layout.screen_h <= 0) { if (down) fprintf(stderr, "test tap: no %s\n", name); return; }
  SDL_Event ev = {0};
  ev.type = down ? SDL_EVENT_FINGER_DOWN : SDL_EVENT_FINGER_UP;
  ev.tfinger.touchID = TEST_TOUCH_ID;
  ev.tfinger.fingerID = 2;
  ev.tfinger.x = (e.boundingBox.x + e.boundingBox.width / 2) / layout.screen_w;
  ev.tfinger.y = (e.boundingBox.y + e.boundingBox.height / 2) / layout.screen_h;
  SDL_PushEvent(&ev);
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
    if (!test_key_tick(p, &at, name, &len)) break;
    if ((at == test_tick || at + 1 == test_tick) && name[0] == '#') test_touch(name + 1, at == test_tick);
    else if ((at == test_tick || at + 1 == test_tick) && name[0] == '^') test_tap(name + 1, at == test_tick);
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
  if (gui_button_event && ev->type == gui_button_event) { *b = (UiButton)ev->user.code; return true; }
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
  if (ev->type == SDL_EVENT_GAMEPAD_AXIS_MOTION && stick_pressed) {
    *b = stick_pressed & JOY_UP ? UI_UP : stick_pressed & JOY_DOWN ? UI_DOWN : stick_pressed & JOY_LEFT ? UI_LEFT : UI_RIGHT;
    return true;
  }
  return false;
}

static struct { SlotsMenu *menu; int tap; uint32_t stamp; } slots_ui;
static void slots_layer(const GuiFrame *f) { slots_ui.tap = gui_slots(slots_ui.menu, &gui_t, f, slots_ui.stamp); }

static int pick_slot_page(SDL_Window *win, SDL_Renderer *ren, SDL_Texture *tex, SlotsMenu *m, const uint8_t *game_rgb) {
  static uint8_t black[FB_W * FB_H * 3];
  slots_ui.menu = m;
  slots_ui.stamp++;
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
    slots_ui.tap = GUI_NONE;
    present(ren, tex, game_rgb ? game_rgb : black);
    push_button(slots_ui.tap == GUI_BACK ? UI_BACK : slots_ui.tap >= 0 ? UI_ACCEPT : GUI_NONE);
    oracles_frame_wait();
  }
}

// A modal slot list; returns the slot, -1 for back, or -2 when the window is closed.
static int pick_slot(SDL_Window *win, SDL_Renderer *ren, SDL_Texture *tex, SlotsMenu *m, const uint8_t *game_rgb) {
  UiScope was = ui_enter(slots_layer, true);
  int slot = pick_slot_page(win, ren, tex, m, game_rgb);
  ui_leave(was);
  return slot;
}

static struct { ControlsMenu *menu; int tap, page; } controls_ui;
static void controls_layer(const GuiFrame *f) {
  controls_ui.tap = gui_controls(controls_ui.menu, &settings.bindings, key_name, pad_name, &gui_t, f, &controls_ui.page);
}

static bool run_controls_page(SDL_Window *win, SDL_Renderer *ren, SDL_Texture *tex, const uint8_t *game_rgb) {
  ControlsMenu m;
  controls_open(&m);
  controls_ui.menu = &m;
  m.pad_page = SDL_HasGamepad();
  for (;;) {
    test_keys();
    SDL_Event ev;
    while (poll_event(&ev)) {
      if (ev.type == SDL_EVENT_QUIT) return false;
      if (ev.type == SDL_EVENT_GAMEPAD_ADDED) SDL_OpenGamepad(ev.gdevice.which);
      if (m.waiting) {
        if (ev.type == SDL_EVENT_KEY_DOWN && !ev.key.repeat) controls_capture_key(&m, &settings.bindings, (int)ev.key.scancode);
        else if (ev.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) controls_capture_pad(&m, &settings.bindings, ev.gbutton.button);
        else if (trigger_down != NO_BINDING) controls_capture_pad(&m, &settings.bindings, trigger_down);
        if (!m.waiting) settings_store();
        continue;
      }
      if (ev.type == SDL_EVENT_KEY_DOWN && window_key(win, &ev.key)) continue;
      UiButton b;
      if (!menu_button(&ev, &b) || (ev.type == SDL_EVENT_KEY_DOWN && ev.key.repeat)) continue;
      if (controls_press(&m, &settings.bindings, b) == MENU_BACK) { settings_store(); return true; }
      settings_store();
    }
    controls_ui.tap = controls_ui.page = GUI_NONE;
    present(ren, tex, game_rgb ? game_rgb : page_black);
    if (controls_ui.tap == GUI_BACK && m.waiting) m.waiting = false;      // a finger can't press a key: Back cancels
    else if (controls_ui.tap == GUI_BACK) push_button(UI_BACK);
    else if (controls_ui.tap >= 0) push_button(UI_ACCEPT);
    else if (controls_ui.page >= 0 && controls_ui.page != m.pad_page) push_button(UI_RIGHT);
    oracles_frame_wait();
  }
}

static bool run_controls(SDL_Window *win, SDL_Renderer *ren, SDL_Texture *tex, const uint8_t *game_rgb) {
  UiScope was = ui_enter(controls_layer, true);
  bool r = run_controls_page(win, ren, tex, game_rgb);
  ui_leave(was);
  return r;
}

static bool run_sync(SDL_Window *win, SDL_Renderer *ren, SDL_Texture *tex, const uint8_t *game_rgb);

static void wait_tick(SDL_Renderer *ren, SDL_Texture *tex, const uint8_t *rgb, bool *quit);

// A newer release, looked for once at start by a thread. ORACLES_TEST_RELEASE=<url> asks that URL
// instead of GitHub (even on a build without a version), and links and the restart are printed
// instead of opened and done.
static UpdateRelease update_rel;
static SDL_AtomicInt update_found;
static bool update_restart;

static const char *update_test(void) { return getenv("ORACLES_TEST_RELEASE"); }

// Running as an AppImage (the file itself, which the download replaces), else NULL.
static const char *update_appimage(void) {
#if !defined(_WIN32) && !defined(__ANDROID__)
  return getenv("APPIMAGE");
#else
  return NULL;
#endif
}

static int SDLCALL update_thread(void *data) {
  (void)data;
  char api[256];
  if (update_test()) snprintf(api, sizeof api, "%s", update_test());
  else snprintf(api, sizeof api, "https://api.github.com/repos/%s/releases/latest", ORACLES_RELEASES);
  const char *asset = update_appimage() ? "Oracles-linux-x86_64.AppImage" : RELEASE_ASSET;
  if (update_check(api, ORACLES_VERSION, asset, &update_rel)) SDL_SetAtomicInt(&update_found, 1);
  return 0;
}

static void update_start(void) {
  if (ORACLES_VERSION[0] || update_test()) SDL_DetachThread(SDL_CreateThread(update_thread, "update", NULL));
}

// The Version row's values, and the notice once the thread has found a release.
static void update_show(void) {
  snprintf(settings.version, sizeof settings.version, "%s", ORACLES_VERSION[0] ? ORACLES_VERSION : "dev");
  if (SDL_GetAtomicInt(&update_found)) snprintf(settings.update, sizeof settings.update, "%.15s", update_rel.tag + (update_rel.tag[0] == 'v'));
}

static void update_poll(void) {
  if (!settings.update[0] && SDL_GetAtomicInt(&update_found)) update_show();
}

#ifndef _WIN32
static struct { HttpResponse res; SDL_AtomicInt done; } update_dl;

static int SDLCALL update_download_thread(void *data) {
  (void)data;
  http_download(update_rel.url, &update_dl.res);
  SDL_SetAtomicInt(&update_dl.done, 1);
  return 0;
}

// Downloads the release's AppImage over the running one, the wait shown meanwhile. On success the
// app quits and main starts the new file.
static bool update_appimage_install(SDL_Renderer *ren, SDL_Texture *tex, const uint8_t *rgb, const char *path) {
  static char msg[40];
  snprintf(msg, sizeof msg, "Updating to %s", settings.update);
  SDL_SetAtomicInt(&update_dl.done, 0);
  SDL_Thread *t = SDL_CreateThread(update_download_thread, "download", NULL);
  if (!t) return false;
  bool quit = false;
  waiting = msg;
  while (!SDL_GetAtomicInt(&update_dl.done)) wait_tick(ren, tex, rgb, &quit);
  waiting = NULL;
  SDL_WaitThread(t, NULL);
  HttpResponse *res = &update_dl.res;
  char err[64] = "";
  bool ok = res->status == 200 && update_install(path, res->body, res->size, update_rel.sha256, err, sizeof err);
  if (!ok && !err[0]) snprintf(err, sizeof err, "%s", res->error[0] ? res->error : "download failed");
  http_response_free(res);
  if (!ok) {
    fprintf(stderr, "update failed: %s\n", err);
    char line[40];
    snprintf(line, sizeof line, "Update failed: %s", err);
    show_status(line);
  }
  update_restart = ok;
  if (ok || quit) SDL_PushEvent(&(SDL_Event){.type = SDL_EVENT_QUIT});
  return ok;
}
#endif

// The notice and the Version row: an AppImage updates itself; anything else (or an AppImage that
// could not) opens this system's file in the release.
static void update_open(SDL_Renderer *ren, SDL_Texture *tex, const uint8_t *rgb) {
#ifndef _WIN32
  const char *appimage = update_appimage();
  if (appimage && update_rel.url[0] && update_rel.sha256[0] && update_appimage_install(ren, tex, rgb, appimage)) return;
#endif
  char url[300];
  if (update_rel.url[0] && (update_test() || !strncmp(update_rel.url, "https://github.com/", 19))) snprintf(url, sizeof url, "%s", update_rel.url);
  else snprintf(url, sizeof url, "https://github.com/%s/releases/tag/%s", ORACLES_RELEASES, update_rel.tag);
  if (update_test()) fprintf(stderr, "open %s\n", url);
  else if (!SDL_OpenURL(url)) fprintf(stderr, "could not open %s: %s\n", url, SDL_GetError());
}

static void copy_error_log(void) {
  error_log_refresh();
  if (!settings.error_log) { show_status("No error log available"); return; }
  if (SDL_SetClipboardText(error_report)) show_status("Error log copied");
  else {
    fprintf(stderr, "copy error log: %s\n", SDL_GetError());
    show_status("Couldn't copy the error log");
  }
}

static struct { int sel, tap; } error_ui;
static void error_layer(const GuiFrame *f) {
  error_ui.tap = gui_error(&error_ui.sel, settings.error_log, &gui_t, f);
}

static bool run_error(SDL_Window *win, SDL_Renderer *ren, SDL_Texture *tex) {
  static const uint8_t rgb[FB_W * FB_H * 3];
  error_log_refresh();
  error_ui.sel = settings.error_log ? ERROR_COPY : ERROR_ISSUE;
  UiScope was = ui_enter(error_layer, true);
  bool done = false, quit = false;
  while (!done && !quit) {
    test_keys();
    SDL_Event ev;
    while (poll_event(&ev)) {
      if (ev.type == SDL_EVENT_QUIT) { quit = true; break; }
      if (ev.type == SDL_EVENT_KEY_DOWN && window_key(win, &ev.key)) continue;
      UiButton b;
      if (!menu_button(&ev, &b)) continue;
      if (b == UI_BACK) { done = true; break; }
      if (b == UI_UP || b == UI_DOWN) {
        int dir = b == UI_UP ? -1 : 1;
        do error_ui.sel = (error_ui.sel + dir + ERROR_ACTIONS) % ERROR_ACTIONS;
        while (error_ui.sel == ERROR_COPY && !settings.error_log);
      }
      if (b != UI_ACCEPT) continue;
      if (error_ui.sel == ERROR_BACK) { done = true; break; }
      if (error_ui.sel == ERROR_COPY) copy_error_log();
      if (error_ui.sel == ERROR_ISSUE && !SDL_OpenURL("https://github.com/" ORACLES_RELEASES "/issues/new"))
        show_status("Couldn't open GitHub");
    }
    if (done || quit) break;
    error_ui.tap = GUI_NONE;
    present(ren, tex, rgb);
    if (error_ui.tap >= 0) { error_ui.sel = error_ui.tap; push_button(UI_ACCEPT); }
    oracles_frame_wait();
  }
  ui_leave(was);
  return !quit;
}

static struct { int tap, selected; bool playing; char status[128]; ModInfo info[2]; ModInstalled installed[MODS_MAX]; int installed_count; } mods_ui;
static void mods_layer(const GuiFrame *f) {
  bool enabled[] = {settings.essence_tracker, settings.d1_randomizer};
  mods_ui.tap = gui_mods(mods_ui.info, enabled, mods_ui.selected, mods_ui.playing, mods_ui.installed, mods_ui.installed_count, mods_ui.status, &gui_t, f);
}

static bool run_mods(SDL_Window *win, SDL_Renderer *ren, SDL_Texture *tex, const uint8_t *game_rgb) {
  static const uint8_t black[FB_W * FB_H * 3];
  mods_ui.info[0] = mods_official_tracker_info();
  mods_ui.info[1] = mods_official_randomizer_info();
  mods_ui.selected = 0;
  mods_ui.playing = game_rgb != NULL;
  char dir[1200];
  snprintf(dir, sizeof dir, "%smods", app_cache);
  mods_ui.installed_count = mods_scan_installed(dir, mods_ui.installed, MODS_MAX);
  mods_ui.status[0] = 0;
  for (int i = 0; i < mods_count(); i++)
    if ((!strcmp(mods_id(i), "essence-tracker") || !strcmp(mods_id(i), "d1-randomizer")) && !mods_enabled(i))
      snprintf(mods_ui.status, sizeof mods_ui.status, "%s", mods_error(i));
  UiScope was = ui_enter(mods_layer, true);
  bool done = false, quit = false;
  while (!done && !quit) {
    test_keys();
    SDL_Event ev;
    while (poll_event(&ev)) {
      if (ev.type == SDL_EVENT_QUIT) { quit = true; break; }
      if (ev.type == SDL_EVENT_KEY_DOWN && window_key(win, &ev.key)) continue;
      UiButton b;
      if (!menu_button(&ev, &b)) continue;
      if (b == UI_BACK) { done = true; break; }
      if (b == UI_UP || b == UI_DOWN) mods_ui.selected = 1 - mods_ui.selected;
      if (b == UI_ACCEPT || b == UI_LEFT || b == UI_RIGHT) {
        if (mods_ui.selected == 0) settings.essence_tracker = !settings.essence_tracker;
        else settings.d1_randomizer = !settings.d1_randomizer;
        settings_store();
      }
    }
    if (done || quit) break;
    mods_ui.tap = GUI_NONE;
    present(ren, tex, game_rgb ? game_rgb : black);
    if (mods_ui.tap == GUI_BACK) push_button(UI_BACK);
    else if (mods_ui.tap >= 0 && mods_ui.tap < 2) { mods_ui.selected = mods_ui.tap; push_button(UI_ACCEPT); }
    oracles_frame_wait();
  }
  ui_leave(was);
  return !quit;
}

static struct { SettingsMenu *menu; GuiSettingsTap tap; bool back; } settings_ui;
static void settings_layer(const GuiFrame *f) { settings_ui.back = gui_settings(settings_ui.menu, &settings, settings.sync_label, &gui_t, f, &settings_ui.tap); }

// A settings tap: a volume bar sets the volume through the same Right press a key would give.
static void push_settings_tap(const GuiSettingsTap *tap) {
  if (tap->volume >= 1) { settings.volume = tap->volume - 1; push_button(UI_RIGHT); }
  else if (tap->row >= 0) push_button(tap->button);
}

static bool run_settings_page(SDL_Window *win, SDL_Renderer *ren, SDL_Texture *tex, const uint8_t *game_rgb, SDL_AudioStream *audio) {
  SettingsMenu m;
  settings_menu_open(&m, ON_PHONE);
  settings_ui.menu = &m;
  error_log_refresh();
  for (;;) {
    test_keys();
    update_poll();
    SDL_Event ev;
    while (poll_event(&ev)) {
      if (ev.type == SDL_EVENT_QUIT) return false;
      if (ev.type == SDL_EVENT_KEY_DOWN && window_key(win, &ev.key)) continue;
      UiButton b;
      if (!menu_button(&ev, &b)) continue;
      SettingsRow row;
      bool was_full = settings.fullscreen, was_wide = settings.widescreen, was_portable = settings.portable;
      MenuAction a = settings_press(&m, &settings, b, &row);
      if (settings.portable != was_portable) portable_switched();
      if (a == MENU_BACK) { settings_store(); return true; }
      if (a == MENU_PICK && row == SET_MODS && !run_mods(win, ren, tex, game_rgb)) return false;
      if (a == MENU_PICK && row == SET_CONTROLS && !run_controls(win, ren, tex, game_rgb)) return false;
      if (a == MENU_PICK && row == SET_SYNC && !run_sync(win, ren, tex, game_rgb)) return false;
      if (a == MENU_PICK && row == SET_VERSION) update_open(ren, tex, game_rgb);
      if (a == MENU_PICK && row == SET_ERROR_LOG) copy_error_log();
      if (settings.fullscreen != was_full) apply_window_settings(win);
      if (settings.widescreen != was_wide) fit_window_to_picture(win);
      if (audio) SDL_SetAudioStreamGain(audio, volume_gain(false));
      apply_game_settings();
      settings_store();
    }
    settings_ui.back = false;
    settings_ui.tap.row = settings_ui.tap.volume = GUI_NONE;
    present(ren, tex, game_rgb);
    if (settings_ui.back) push_button(UI_BACK);
    else push_settings_tap(&settings_ui.tap);
    oracles_frame_wait();
  }
}

static bool run_settings(SDL_Window *win, SDL_Renderer *ren, SDL_Texture *tex, const uint8_t *game_rgb, SDL_AudioStream *audio) {
  UiScope was = ui_enter(settings_layer, true);
  bool ok = run_settings_page(win, ren, tex, game_rgb, audio);
  ui_leave(was);
  return ok;
}

// Save sync (server in its own repository, save-sync-server): this device's code and server, and
// what the last sync did.
static SyncConfig sync_cfg;
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
  update_show();
  error_log_refresh();
  settings.portable = portable_marked();
  apply_window_settings(win);
  apply_game_settings();
}

// One frame of the wait over what is on screen, before a call that blocks (it stays up through it).
static void wait_shown(SDL_Renderer *ren, SDL_Texture *tex, const uint8_t *rgb, const char *msg) {
  waiting = msg;
  present(ren, tex, rgb);
}

// One frame of a sync wait: input is dropped, but closing the window is kept for after.
static void wait_tick(SDL_Renderer *ren, SDL_Texture *tex, const uint8_t *rgb, bool *quit) {
  test_keys();
  SDL_Event ev;
  while (poll_event(&ev))
    if (ev.type == SDL_EVENT_QUIT) *quit = true;
  present(ren, tex, rgb);
  oracles_frame_wait();
}

// Waits over the open page until the sync thread has finished one sync of `game` (HTTP gives up
// after 30 seconds).
static bool sync_blocking(SDL_Renderer *ren, SDL_Texture *tex, const uint8_t *rgb, const char *game, SyncResult *r) {
  if (!sync_cfg.code[0]) return false;
  SyncResult earlier;
  bool quit = false;
  waiting = "Syncing";
  while (sync_busy()) wait_tick(ren, tex, rgb, &quit);
  sync_poll(&earlier);
  bool started = sync_start(&sync_cfg, app_cache, game);
  while (started && !sync_poll(r)) wait_tick(ren, tex, rgb, &quit);
  waiting = NULL;
  if (quit) SDL_PushEvent(&(SDL_Event){.type = SDL_EVENT_QUIT});
  return started;
}

static void side_fill(ConflictSide *side, const char *label, const char *game, const char *name, const uint8_t *data, size_t n, const uint8_t *thumb, size_t tn) {
  snprintf(side->label, sizeof side->label, "%s", label);
  if (!data) { side->missing = true; return; }
  if (!strcmp(name, "sram.sav")) { side->has_files = true; ui_read_files(data, n, is_ages(game), side->files); }
  if (thumb && tn == sizeof side->thumb) { side->has_thumb = true; memcpy(side->thumb, thumb, tn); }
}

// Each file that changed here and on the server: the player keeps one side. Returns false on quit.
static struct { ConflictView *view; int tap; } conflict_ui;
static void conflict_layer(const GuiFrame *f) { conflict_ui.tap = gui_conflict(conflict_ui.view, &gui_t, f); }

static bool run_conflicts_page(SDL_Renderer *ren, SDL_Texture *tex, const SyncResult *r) {
  static ConflictView v;
  conflict_ui.view = &v;
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
    const char *what = !strcmp(ch->name, "sram.sav") ? "save files" : !strcmp(ch->name, "state_auto") ? "resume" : NULL;
    if (what) snprintf(v.what, sizeof v.what, "%s %s", is_ages(ch->game) ? "Ages" : "Seasons", what);
    else snprintf(v.what, sizeof v.what, "%s slot %c", is_ages(ch->game) ? "Ages" : "Seasons", ch->name[6]);
    side_fill(&v.side[0], "This device", ch->game, ch->name, mine, n, mine_thumb, tn);
    v.side[0].time = (int64_t)ch->local_mtime * SDL_NS_PER_SECOND;
    side_fill(&v.side[1], ch->remote.device[0] ? ch->remote.device : "Other device", ch->game, ch->name, theirs, rn, theirs_thumb, rtn);
    v.side[1].time = (int64_t)ch->remote.updated * SDL_NS_PER_SECOND;
    free(mine); free(mine_thumb); free(theirs); free(theirs_thumb);
    test_mark("conflict");
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
          wait_shown(ren, tex, page_black, "Syncing");
          bool kept = sync_resolve(&sync_cfg, app_cache, ch, v.sel == 0);
          waiting = NULL;
          show_status(kept ? "Kept" : "Sync failed");
          decided = true;
        }
      }
      conflict_ui.tap = GUI_NONE;
      present(ren, tex, page_black);
      push_button(conflict_ui.tap == GUI_BACK ? UI_BACK : conflict_ui.tap >= 0 ? UI_ACCEPT : GUI_NONE);
      oracles_frame_wait();
    }
  }
  return true;
}

static bool run_conflicts(SDL_Renderer *ren, SDL_Texture *tex, const SyncResult *r) {
  UiScope was = ui_enter(conflict_layer, true);
  bool ok = run_conflicts_page(ren, tex, r);
  ui_leave(was);
  return ok;
}

// Toasts the outcome, loads settings that came from elsewhere, and asks about conflicts.
static bool sync_finished(SDL_Window *win, SDL_Renderer *ren, SDL_Texture *tex, const SyncResult *r) {
  if (r->settings_changed) settings_reload(win);
  SDL_DateTime dt;
  SDL_Time now;
  bool clock = SDL_GetCurrentTime(&now) && SDL_TimeToDateTime(now, &dt, true);
  if (!r->ok) {
    fprintf(stderr, "sync failed: %s\n", r->error);
    snprintf(sync_status_line, sizeof sync_status_line, "Sync failed");
    if (sync_failures() == 1) show_status("Sync failed");         // not again for every retry
    return true;
  }
  if (r->error[0]) fprintf(stderr, "sync: %s\n", r->error);
  else test_mark("synced");
  if (clock) snprintf(sync_status_line, sizeof sync_status_line, "Synced at %02d:%02d", dt.hour, dt.minute);
  if (r->uploaded || r->downloaded) show_status("Synced");
  return !r->choices || run_conflicts(ren, tex, r);
}

// The sync page: the code, a new account, entering a code from another device, sync now, off.
static struct { SyncMenu *menu; CodeEntry *code; bool reveal; int tap; } sync_ui;
static void sync_layer(const GuiFrame *f) {
  if (sync_ui.code) sync_ui.tap = gui_code(sync_ui.code, &gui_t, f);
  else sync_ui.tap = gui_sync(sync_ui.menu, &gui_t, f, &sync_ui.reveal);
}

static bool run_sync_page(SDL_Window *win, SDL_Renderer *ren, SDL_Texture *tex, const uint8_t *game_rgb) {
  SyncMenu m;
  sync_ui.menu = &m;
  sync_ui.code = NULL;
  sync_ui.reveal = false;
  if (!game_rgb) game_rgb = page_black;
  char code[20] = "";
  // what SYNC NOW covers: every game, or during play every game but the running one
  const char *scope = !playing ? SYNC_ALL_GAMES : rom_id(app_cache, !is_ages(playing));
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
        wait_shown(ren, tex, game_rgb, "Creating a code");
        bool made = sync_create_account(sync_cfg.url, nc, err, sizeof err);
        waiting = NULL;
        if (made) { memcpy(sync_cfg.code, nc, 17); new_code = true; }
        else { fprintf(stderr, "sync: %s\n", err); show_status("Couldn't create a code"); }
      } else if (row == SYNCROW_ENTER) {
        CodeEntry e;
        codeentry_open(&e, sync_cfg.code);
        sync_ui.code = &e;
        // typing: the phone's number pad, or the keyboard's digits; each fills the box and moves on
        SDL_PropertiesID props = SDL_CreateProperties();
        SDL_SetNumberProperty(props, SDL_PROP_TEXTINPUT_TYPE_NUMBER, SDL_TEXTINPUT_TYPE_NUMBER);
        SDL_StartTextInputWithProperties(win, props);
        bool quit = false;
        for (bool done = false; !done && !quit;) {
          test_keys();
          while (poll_event(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) { quit = true; break; }
            if (ev.type == SDL_EVENT_TEXT_INPUT) {
              for (const char *c = ev.text.text; *c; c++)
                if (*c >= '0' && *c <= '9') { e.digits[e.pos] = *c; if (e.pos < 15) e.pos++; }
              continue;
            }
            if (!menu_button(&ev, &b) || (ev.type == SDL_EVENT_KEY_DOWN && ev.key.repeat)) continue;
            MenuAction ea = codeentry_press(&e, b);
            if (ea == MENU_BACK) done = true;
            if (ea == MENU_PICK) { memcpy(sync_cfg.code, e.digits, 17); new_code = done = true; }
          }
          sync_ui.tap = GUI_NONE;
          present(ren, tex, game_rgb);
          push_button(sync_ui.tap == GUI_BACK ? UI_BACK : sync_ui.tap == GUI_CONNECT ? UI_ACCEPT : GUI_NONE);
          oracles_frame_wait();
        }
        SDL_StopTextInput(win);
        SDL_DestroyProperties(props);
        sync_ui.code = NULL;
        if (quit) return false;
      } else if (row == SYNCROW_OFF) {
        sync_cfg.code[0] = 0;
        sync_config_store(&sync_cfg, app_cache);
        sync_label_update();
      }
      if (new_code) { sync_config_store(&sync_cfg, app_cache); sync_label_update(); }
      if (row == SYNCROW_NOW || new_code) {
        SyncResult r;
        if (sync_blocking(ren, tex, game_rgb, scope, &r) && !sync_finished(win, ren, tex, &r)) return false;
      }
      REOPEN();
    }
    sync_ui.tap = GUI_NONE;
    present(ren, tex, game_rgb);
    push_button(sync_ui.tap == GUI_BACK ? UI_BACK : sync_ui.tap >= 0 ? UI_ACCEPT : GUI_NONE);
    oracles_frame_wait();
  }
#undef REOPEN
}

static bool run_sync(SDL_Window *win, SDL_Renderer *ren, SDL_Texture *tex, const uint8_t *game_rgb) {
  UiScope was = ui_enter(sync_layer, true);
  bool r = run_sync_page(win, ren, tex, game_rgb);
  ui_leave(was);
  return r;
}

// The main menu's states: each game's four slots and its resume state, with their pictures.
static void home_scan(Home *h, const char *cache) {
  launcher_load(&h->l, cache);
  for (int g = 0; g < UI_GAMES; g++) {
    char dir[1100];
    snprintf(dir, sizeof dir, "%s%s", cache, rom_id(cache, g == UI_GAME_AGES));
    for (int i = 0; i < HOME_SLOTS; i++) slot_read(&h->slots[g][i], dir, i);
    slot_read(&h->resume[g], dir, SLOT_AUTO);
  }
  h->stamp++;
}

static struct { Home *home; SettingsMenu *settings; int button; GuiSettingsTap tap; } home_ui;
static void home_layer(const GuiFrame *f) {
  home_ui.button = gui_home(home_ui.home, &gui_t, f, home_ui.settings, &settings, settings.sync_label, &home_ui.tap);
}

// Runs the main menu until a game is chosen (true) or the player quits (false).
static bool run_home(SDL_Window *win, SDL_Renderer *ren, SDL_Texture *tex, const char *cache, char *game_out, int *file_out, bool *file_used, int *slot_out) {
  static Home h;
  static uint8_t rgb[UI_W * UI_H * 3];          // under the page; also what the sync wait shows
  // No ROM yet is not a special case: the menu opens as usual and its Add ROM row asks for one,
  // rather than a file dialog appearing over nothing on first launch.
  home_scan(&h, cache);
  error_log_refresh();
  home_init(&h, UI_GAME_SEASONS);
  static SettingsMenu settings_menu;
  settings_menu_open(&settings_menu, ON_PHONE);
  home_ui.home = &h;
  home_ui.settings = &settings_menu;
  if (sync_cfg.code[0] && !sync_busy()) sync_start(&sync_cfg, cache, SYNC_ALL_GAMES);
  for (;;) {
    test_keys();
    SyncResult synced;
    if (sync_poll(&synced)) {
      if (!sync_finished(win, ren, tex, &synced)) return false;
      home_scan(&h, cache);
    }
    if (sync_cfg.code[0] && sync_should_retry()) sync_start(&sync_cfg, cache, SYNC_ALL_GAMES);
    update_poll();
    SDL_Event ev;
    while (poll_event(&ev)) {
      if (ev.type == SDL_EVENT_QUIT) return false;
      if (ev.type == SDL_EVENT_KEY_DOWN && window_key(win, &ev.key)) continue;
      if (ev.type == SDL_EVENT_GAMEPAD_ADDED) SDL_OpenGamepad(ev.gdevice.which);
      UiButton b;
      if (!menu_button(&ev, &b) || (ev.type == SDL_EVENT_KEY_DOWN && ev.key.repeat)) continue;
      LaunchResult r = home_press(&h, b);
      if (r.action == LAUNCH_QUIT) return false;
      if (r.action == LAUNCH_SETTINGS) {
        SettingsRow row;
        bool was_full = settings.fullscreen, was_wide = settings.widescreen, was_portable = settings.portable;
        MenuAction sa = settings_press(&settings_menu, &settings, r.button, &row);
        if (settings.portable != was_portable) portable_switched();
        if (sa == MENU_PICK && row == SET_MODS && !run_mods(win, ren, tex, NULL)) return false;
        if (sa == MENU_PICK && row == SET_CONTROLS && !run_controls(win, ren, tex, NULL)) return false;
        if (sa == MENU_PICK && row == SET_SYNC) {
          if (!run_sync(win, ren, tex, NULL)) return false;
          home_scan(&h, cache);
        }
        if (sa == MENU_PICK && row == SET_VERSION) update_open(ren, tex, rgb);
        if (sa == MENU_PICK && row == SET_ERROR_LOG) copy_error_log();
        if (settings.fullscreen != was_full) apply_window_settings(win);
        if (settings.widescreen != was_wide) fit_window_to_picture(win);
        apply_game_settings();
        settings_store();
      }
      if (r.action == LAUNCH_ADD_ROM) {
        char game[16];
        if (pick_rom(game, cache)) { home_scan(&h, cache); home_init(&h, is_ages(game) ? UI_GAME_AGES : UI_GAME_SEASONS); }
      }
      if (r.action == LAUNCH_PLAY && r.file == LAUNCH_SLOTS) {
        if (r.slot < 0) continue;
        *slot_out = r.slot;
      }
      if (r.action == LAUNCH_PLAY && sync_cfg.code[0]) {
        SyncResult pre;
        if (sync_blocking(ren, tex, rgb, rom_id(cache, r.game == UI_GAME_AGES), &pre) && !sync_finished(win, ren, tex, &pre)) return false;
        home_scan(&h, cache);
      }
      if (r.action == LAUNCH_PLAY) {
        strcpy(game_out, rom_id(cache, r.game == UI_GAME_AGES));
        *file_out = r.file;
        *file_used = r.file >= 0 && h.l.games[r.game].files[r.file].valid;
        return true;
      }
    }
    overlay_style(&h.l.games[h.l.game].theme);
    gui_theme(&gui_t, h.l.game == UI_GAME_AGES);
    home_ui.button = GUI_NONE;
    home_ui.tap.row = home_ui.tap.volume = GUI_NONE;
    present(ren, tex, rgb);
    if (home_ui.button == GUI_UPDATE) update_open(ren, tex, rgb);
    else if (home_ui.button >= 0) push_button(home_ui.button);
    else push_settings_tap(&home_ui.tap);
    oracles_frame_wait();
  }
}

static bool run_launcher(SDL_Window *win, SDL_Renderer *ren, SDL_Texture *tex, const char *cache, char *game_out, int *file_out, bool *file_used, int *slot_out) {
  UiScope was = ui_enter(home_layer, true);
  bool ok = run_home(win, ren, tex, cache, game_out, file_out, file_used, slot_out);
  ui_leave(was);
  return ok;
}

// Boots straight into a save file: presses Start until the file select screen is up, moves the
// cursor to the file, then A (the file) and A (the text-speed prompt). Driven by the game's own
// file-select state, so the same inputs a player would give; any key cancels it. The European
// releases open their language screen (file-select mode 8) before the file menu the first time
// they start: that choice is the player's, so the driver waits there at normal speed, the player's
// keys going to the game, and carries on once the file menu is up.
typedef enum { BOOT_OFF, BOOT_TO_FILE_SELECT, BOOT_MOVE, BOOT_SELECT, BOOT_TEXT_SPEED, BOOT_LANGUAGE } BootPhase;
typedef struct { BootPhase phase; int file; bool open_file; uint64_t since; } BootDriver;

// wThreadStateBuffer+6/+7 ($c2e6/$c2e7) in both games: the intro stage and the title screen's state
#define INTRO_STAGE 0x2e6
#define INTRO_VAR 0x2e7
#define FS_MODE(gb) ((gb)->wram[0][wFileSelect_mode & 0xfff])
#define FS_MODE2(gb) ((gb)->wram[0][wFileSelect_mode2 & 0xfff])
#define FS_CURSOR(gb) ((gb)->wram[0][wFileSelect_cursorPos & 0xfff])
#define FS_MODE_LANGUAGE 8

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
    if (FS_MODE(gb) == FS_MODE_LANGUAGE) { d->phase = BOOT_LANGUAGE; test_mark("language"); return 0; }
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
  case BOOT_LANGUAGE:                     // the main loop runs these frames with the player's keys
    return 0;
  }
  return 0;
}

// Saving needs a frame whose threads are all parked at the top of their loops (threads_parked):
// Cmd+S and the pause menu wait for one, usually the same frame; a load then resumes them in C.
#define NO_SAVE UINT64_MAX
#define PARK_TIMEOUT 600

#define GB_FRAME_NS (FRAME_CYCLES * 1000000000ull / 4194304)
#define FF_MAX_LAG_NS 50000000


typedef enum { GAME_TO_LAUNCHER, GAME_QUIT, GAME_FAILED, GAME_STOPPED } GameEnd;
typedef enum { MODE_PLAY, MODE_PAUSING, MODE_PAUSED, MODE_QUITTING } GameMode;

typedef struct {
  const char *cache, *game;
  int start_file;               // 0..2, LAUNCH_TITLE, LAUNCH_RESUME, LAUNCH_SLOTS
  bool open_file;               // start_file holds a save: open it, not just highlight it
  int start_slot;               // with LAUNCH_SLOTS
  uint64_t max_frames;
} GameStart;

// Restrict the search to the selected ROM's folder. On equal dates prefer a state (auto, then
// the lowest manual slot), since it also captures unsaved progress. A bad newest state fails
// through slot_load as an explicit --state would; never silently resume an older playthrough.
static bool latest_start(GameStart *gs) {
  char dir[1100], path[1300];
  snprintf(dir, sizeof dir, "%s%s", gs->cache, gs->game);
  migrate_state(dir);
  bool found = false;
  SDL_Time newest = 0;
  SDL_PathInfo info;
  for (int i = 0; i <= UI_SLOTS; i++) {
    int slot = i == 0 ? SLOT_AUTO : i - 1;
    slot_path(path, sizeof path, dir, slot, "");
    if (!SDL_GetPathInfo(path, &info) || info.type != SDL_PATHTYPE_FILE) continue;
    if (found && info.modify_time <= newest) continue;
    found = true;
    newest = info.modify_time;
    gs->start_file = slot == SLOT_AUTO ? LAUNCH_RESUME : LAUNCH_SLOTS;
    gs->start_slot = slot;
    gs->open_file = false;
  }
  snprintf(path, sizeof path, "%s/sram.sav", dir);
  size_t n = 0;
  uint8_t *sram = oracles_read_file(path, &n);
  SaveDate dates[UI_FILES];
  save_dates(path, sram, n, is_ages(gs->game), dates);
  free(sram);
  for (int i = 0; i < UI_FILES; i++) {
    if (!dates[i].hash[0]) continue;
    if (!found || dates[i].time > newest) {
      found = true;
      newest = dates[i].time;
      gs->start_file = i;
      gs->open_file = true;
    } else if (dates[i].time == newest && gs->start_file >= 0) gs->open_file = false;
  }
  if (!found) fprintf(stderr, "no save files or save states found for %s\n", gs->game);
  else if (gs->start_file >= 0) {
    if (gs->open_file) fprintf(stderr, "latest: starting save file %d\n", gs->start_file + 1);
    else fprintf(stderr, "latest: normal save slots have tied or unknown dates; opening file selection\n");
  }
  return found;
}

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
  if (gb->hung) return true;
  if (*session.mode == MODE_PLAY || *session.mode == MODE_PAUSING) {
    static int16_t samples[APU_RING * 2];
    live_joy = 0;
    for (int i = 0; i < PARK_TIMEOUT && !gb->hung && !threads_parked(gb); i++) { features_frame(gb); gb_run_frame(gb); (*session.frames)++; }
    if (gb->hung) return true;
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

// A player who launches the AppImage from a desktop launcher never sees stderr, so an engine stop
// has to leave something behind: the address it stopped at is the whole diagnosis (issue #13 was
// `no code at 01:7b6e`), and without it a report can only say the window disappeared.
static void write_crash_log(const GB *gb, const char *dir, const char *game, bool ages, uint64_t frames) {
  char path[1300];
  snprintf(path, sizeof path, "%s/crash.log", dir);
  FILE *f = fopen(path, "a");
  if (!f) { fprintf(stderr, "could not write %s\n", path); return; }
  time_t now = time(NULL);
  char when[32] = "";
  struct tm tm;
#ifdef _WIN32
  if (gmtime_s(&tm, &now) == 0) strftime(when, sizeof when, "%Y-%m-%dT%H:%M:%SZ", &tm);
#else
  if (gmtime_r(&now, &tm)) strftime(when, sizeof when, "%Y-%m-%dT%H:%M:%SZ", &tm);
#endif
  // the room is what makes it reproducible; the two games keep it at different addresses
  unsigned group = gb->wram[0][(ages ? 0xcc2d : 0xcc49) - 0xc000];
  unsigned room  = gb->wram[0][(ages ? 0xcc30 : 0xcc4c) - 0xc000];
  unsigned cut   = gb->wram[0][0xcc04 - 0xc000];
  fprintf(f, "%s the engine stopped\n", when);
  fprintf(f, "  reason   %s\n", gb_stop_reason[0] ? gb_stop_reason : "(not recorded)");
  fprintf(f, "  game     %s\n", game);
  fprintf(f, "  version  %s, platform %s\n", ORACLES_VERSION[0] ? ORACLES_VERSION : "dev", SDL_GetPlatform());
  fprintf(f, "  room     group %02x room %02x, cutscene %02x\n", group, room, cut);
  fprintf(f, "  cpu      pc %04x sp %04x bank %02x, frame %llu\n", gb->pc, gb->sp, gb->rom_bank,
          (unsigned long long)frames);
  fprintf(f, "  build    state format %08x\n", (unsigned)oracles_state_format());
  fclose(f);
  fprintf(stderr, "wrote %s\n", path);
}

static struct { PauseMenu *menu; int tap; } pause_ui;
static void pause_layer(const GuiFrame *f) { pause_ui.tap = gui_pause(pause_ui.menu, &gui_t, f); }

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
  bool ages = is_ages(gs->game);
  UiTheme theme;
  ui_theme_load(&theme, rom, rom_size, ages);
  overlay_style(&theme);
  ui_font_load(&mod_font, rom, rom_size);
  GB *gb = calloc(1, sizeof *gb);
  gb_init(gb);
  gb_stop_reason[0] = 0;
  if (!gb_load_rom(gb, rom, rom_size)) { fprintf(stderr, "cache in %s is not a ROM image\n", dir); return GAME_FAILED; }
  gb->cyctab = tab;
#ifndef NDEBUG
  gb->code_bits = assets_code_bits(rom, rom_size);
#endif
  gb_reset(gb);
  wide_npcs_reset();
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
  static SlotsMenu slots;
  PauseMenu pause;
  int undo_slot = -1;
  gui_theme(&gui_t, ages);
  gb->input_at = live_input;
  live_joy = slot_joy[0] = slot_joy[1] = 0;
  item_buttons_load(dir);
  // Re-scans for this specific game: the launcher's own, game-agnostic mods_init (see main())
  // already populated settings rows before the player picked anything, but only mods_init here
  // knows the real `seasons`, which is what a mod's on_frame/on_room_change handlers and its own
  // oracles.game()/oracles.rd() calls need to be correct for -- so every mod is reloaded now
  // under the right game, discarding whatever the launcher-time scan loaded. mods_init closes any
  // previously-loaded mods itself before rescanning, so this is safe however we got here.
  char mods_dir[1200];
  snprintf(mods_dir, sizeof mods_dir, "%smods", app_cache);
  mods_init(mods_dir, !ages);
  if (settings.essence_tracker) {
    const char *error = mods_load_official_tracker();
    if (error) show_status(error);
  }
  if (settings.d1_randomizer && !ages) {
    const char *error = mods_load_official_randomizer();
    if (error) show_status(error);
  }
  features.treasure_swap = mods_treasure_swap;
  // Tracks only true->false transitions below, so the player sees a status line the moment a mod
  // fails -- whether that's immediately (a bad main.lua) or frames into play (a runaway handler)
  // -- without repeating it every subsequent frame the same mod stays disabled.
  int mod_count = mods_count();
  bool mod_was_enabled[MODS_MAX];
  for (int i = 0; i < mod_count; i++) mod_was_enabled[i] = mods_enabled(i);
  for (int i = 0; i < mod_count; i++) if (!mod_was_enabled[i]) { show_status("Mod disabled"); break; }
  uint64_t frames = 0, save_from = NO_SAVE, park_from = 0, ff_due = 0;
  int ff_ratio = 1;                       // the audio stream's frequency ratio: settings.ff_speed while fast-forwarding
  bool muted = false, parked = false, fast = false;
  GameMode mode = MODE_PLAY;
  GameEnd end = GAME_QUIT;
  BootDriver boot = {gs->start_file >= 0 ? BOOT_TO_FILE_SELECT : BOOT_OFF, gs->start_file, gs->open_file, 0};
  if (gs->start_file == LAUNCH_RESUME || gs->start_file == LAUNCH_SLOTS) {
    int slot = gs->start_file == LAUNCH_RESUME ? SLOT_AUTO : gs->start_slot;
    if (!slot_load(gb, dir, slot)) {
      char path[1300];
      slot_path(path, sizeof path, dir, slot, "");
      fprintf(stderr, "could not load save state %s (missing, corrupt or incompatible)\n", path);
      end = GAME_FAILED;
    }
    else if (slot == SLOT_AUTO) fprintf(stderr, "started from automatic state\n");
    else fprintf(stderr, "started from slot %d\n", slot + 1);
  }
  framebuffer_to_rgb(gb->sample ? gb->sample->framebuffer : gb->framebuffer, rgb);
  session = (Session){gb, dir, sav, paused_rgb, &mode, &parked, false, &frames, gs->game};
  paused_game.rgb = paused_rgb;
  paused_game.gb = gb;
  paused_game.seasons = !ages;
  paused_game.rom = rom;
  paused_game.rom_size = rom_size;
  playing = gs->game;

  for (bool running = end != GAME_FAILED; running && !gb->hung;) {
    test_keys();
    SyncResult synced;
    if (sync_poll(&synced)) {
      if (synced.ok && !synced.error[0]) show_status("Synced");
      else if (sync_failures() == 1) show_status("Sync failed");
    }
    // the running game's files wait for the game to end; the rest go as soon as the server answers
    if (sync_cfg.code[0] && sync_should_retry()) sync_start(&sync_cfg, app_cache, rom_id(app_cache, !is_ages(gs->game)));
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
          int slot = pick_slot(win, ren, tex, &slots, paused_rgb);
          if (slot == -2) { end = GAME_QUIT; running = false; if (parked) slot_save(gb, dir, SLOT_AUTO, paused_rgb); }
          else if (slot >= 0 && item == PAUSE_SAVE) {
            bool ok = slot_save(gb, dir, slot, paused_rgb);
            char msg[40];
            snprintf(msg, sizeof msg, "Saved to slot %d", slot + 1);
            if (ok) { undo_slot = slot; show_undoable(msg); } else show_status("Couldn't save the state");
            pause.can_load = pause.can_load || ok;
          } else if (slot >= 0) {
            bool ok = slot_load(gb, dir, slot);
            char msg[40];
            snprintf(msg, sizeof msg, "Loaded slot %d", slot + 1);
            show_status(ok ? msg : "Couldn't load the state");
            if (ok) { mode = MODE_PLAY; if (audio) SDL_ResumeAudioStreamDevice(audio); }
          }
        } else if (a == MENU_PICK && item == PAUSE_SETTINGS) {
          if (!run_settings(win, ren, tex, paused_rgb, NULL)) { end = GAME_QUIT; running = false; if (parked) slot_save(gb, dir, SLOT_AUTO, paused_rgb); }
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
        // Cmd+S / Cmd+R on a Mac, Ctrl+S / Ctrl+R elsewhere (either works on both)
        else if ((ev.key.mod & (SDL_KMOD_GUI | SDL_KMOD_CTRL)) && ev.key.scancode == SDL_SCANCODE_S) press |= 1u << ACT_SAVE_STATE;
        else if ((ev.key.mod & (SDL_KMOD_GUI | SDL_KMOD_CTRL)) && ev.key.scancode == SDL_SCANCODE_R) press |= 1u << ACT_LOAD_STATE;
        else if (ev.key.scancode == SDL_SCANCODE_M) {
          muted = !muted;
          if (audio) SDL_SetAudioStreamGain(audio, volume_gain(muted));
          show_status(muted ? "Sound off" : "Sound on");
        }
        else press |= action_bit(bindings_key_action(&settings.bindings, (int)ev.key.scancode));
        break;
      case SDL_EVENT_KEY_UP:
        release |= action_bit(bindings_key_action(&settings.bindings, (int)ev.key.scancode));
        break;
      case SDL_EVENT_GAMEPAD_AXIS_MOTION:
        press |= action_bit(bindings_pad_action(&settings.bindings, trigger_down));
        release |= action_bit(bindings_pad_action(&settings.bindings, trigger_up));
        if (stick_pressed && boot.phase != BOOT_OFF && boot.phase != BOOT_LANGUAGE) { boot.phase = BOOT_OFF; live_joy = 0; }
        break;
      case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
        press |= action_bit(bindings_pad_action(&settings.bindings, ev.gbutton.button));
        break;
      case SDL_EVENT_GAMEPAD_BUTTON_UP:
        release |= action_bit(bindings_pad_action(&settings.bindings, ev.gbutton.button));
        break;
      }
      if (press >> ACT_PAUSE & 1) { mode = MODE_PAUSING; park_from = frames; live_joy = 0; boot.phase = BOOT_OFF; continue; }
      if (press && boot.phase != BOOT_OFF && boot.phase != BOOT_LANGUAGE) { boot.phase = BOOT_OFF; live_joy = 0; }
      for (int a = 0; a < ACTIONS; a++) {
        bool item = a == ACT_ITEM_X || a == ACT_ITEM_Y;
        if (press >> a & 1) {
          if (a == ACT_FAST) fast = true;
          else if (a == ACT_SWAP) features_quick_swap(gb);
          else if (a == ACT_SAVE_STATE) save_from = frames;
          else if (a == ACT_LOAD_STATE) {
            bool ok = slot_load(gb, dir, 0);
            if (ok) { live_joy = 0; save_from = NO_SAVE; }
            show_status(ok ? "Loaded slot 1" : "Slot 1 is empty");
          }
          else if (item) item_button(gb, dir, a == ACT_ITEM_Y, true);
          else live_joy = tap_press(&taps, live_joy, joy_of((Action)a));
        }
        if (release >> a & 1) {
          if (a == ACT_FAST) fast = false;
          else if (item) item_button(gb, dir, a == ACT_ITEM_Y, false);
          else live_joy = tap_release(&taps, live_joy, joy_of((Action)a));
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

    if (undo_tapped) {
      undo_tapped = false;
      char msg[40];
      snprintf(msg, sizeof msg, "Slot %d is back as it was", undo_slot + 1);
      show_status(undo_slot >= 0 && slot_undo(dir, undo_slot) ? msg : "Nothing to undo");
      undo_slot = -1;
    }

    if (mode == MODE_PAUSED) {
      pause_ui.menu = &pause;
      pause_ui.tap = GUI_NONE;
      UiScope was = ui_enter(pause_layer, false);
      present(ren, tex, paused_rgb);
      ui_leave(was);
      if (pause_ui.tap >= 0) push_button(UI_ACCEPT);
      oracles_frame_wait();
      continue;
    }

    // fast-forward's sound plays ff_speed times as fast; what it left queued is dropped on release,
    // as at normal speed it would play slowed down and late
    int ratio = fast && mode == MODE_PLAY && (boot.phase == BOOT_OFF || boot.phase == BOOT_LANGUAGE) ? settings.ff_speed : 1;
    if (ratio != ff_ratio) {
      if (audio && ratio == 1) SDL_ClearAudioStream(audio);
      if (audio) SDL_SetAudioStreamFrequencyRatio(audio, (float)ratio);
      ff_ratio = ratio;
    }

    if (boot.phase == BOOT_LANGUAGE && FS_MODE(gb) != FS_MODE_LANGUAGE) {
      boot.phase = BOOT_MOVE;             // a language was picked: on to the file
      boot.since = frames;
      live_joy = 0;
    }
    if (boot.phase != BOOT_OFF && boot.phase != BOOT_LANGUAGE) {
      // fast-forward the logos and menus, silently
      for (int i = 0; i < 8 && !gb->hung && boot.phase != BOOT_OFF && boot.phase != BOOT_LANGUAGE; i++) { live_joy = boot_input(gb, &boot, frames); features_frame(gb); gb_run_frame(gb); frames++; }
      if (boot.phase == BOOT_OFF) { live_joy = 0; fprintf(stderr, "boot into file %d done at frame %llu\n", boot.file + 1, (unsigned long long)frames); }
      apu_read_samples(&gb->apu, samples, APU_RING);
    } else if (fast && mode == MODE_PLAY) {
      // fast-forward: ff_speed times the Game Boy's frame rate, by the clock rather than the display
      // (a vsynced present comes at anything from 60 to 240 Hz), with the sound sped up to match.
      // Just pressed, or more than FF_MAX_LAG_NS behind on a device too slow for the speed: count
      // from now. The ring is drained every frame, as a late catch-up can run more than it holds.
      uint64_t now = SDL_GetTicksNS();
      if ((int64_t)(now - ff_due) > FF_MAX_LAG_NS) ff_due = now;
      if ((int64_t)(ff_due - now) > 0) { SDL_Delay(1); continue; }
      while (!gb->hung && (int64_t)(now - ff_due) >= 0) {
        features_frame(gb); gb_run_frame(gb); frames++; ff_due += GB_FRAME_NS / settings.ff_speed;
        uint32_t n = apu_read_samples(&gb->apu, samples, APU_RING);
        if (audio && SDL_GetAudioStreamQueued(audio) <= AUDIO_TARGET_BYTES * ff_ratio) SDL_PutAudioStreamData(audio, samples, n * 4);
      }
      oracles_frame_wait();
    } else {
      if (mode == MODE_PLAY && audio && SDL_GetAudioStreamQueued(audio) > AUDIO_TARGET_BYTES) { SDL_Delay(1); continue; }
      features_frame(gb); gb_run_frame(gb);
      frames++;
    }
    // A stopped frame may have no sample and must never reach autosave or rendering.
    if (gb->hung) break;
    live_joy = tap_frame_ran(&taps, live_joy);

    if (settings.widescreen && mode == MODE_PLAY && boot.phase == BOOT_OFF && wide_npcs_update(gb)) npcs_signal();

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
      if (ok) { undo_slot = 0; show_undoable("Saved to slot 1"); } else show_status("Couldn't save the state");
      save_from = NO_SAVE;
    } else if (save_from != NO_SAVE && frames - save_from > PARK_TIMEOUT) {
      show_status("Can't save here");
      save_from = NO_SAVE;
    }
    uint32_t n = apu_read_samples(&gb->apu, samples, APU_RING);
    if (audio && mode == MODE_PLAY) SDL_PutAudioStreamData(audio, samples, n * 4);
    framebuffer_to_rgb(gb->sample->framebuffer, rgb);
    present_game(ren, gb->sample, rgb, !ages, rom, rom_size);
    apply_mod_writes(gb);
    for (int i = 0; i < mod_count; i++) {
      bool now = mods_enabled(i);
      if (mod_was_enabled[i] && !now) show_status("Mod disabled");
      mod_was_enabled[i] = now;
    }
    if (frames % 600 == 0) save_sram(gb, sav);
    if (gs->max_frames && frames >= gs->max_frames) running = false;
  }
  if (gb->hung) {
    write_crash_log(gb, dir, gs->game, ages, frames);
    end = GAME_STOPPED;
  }
  else if (gs->max_frames && end != GAME_FAILED) fprintf(stderr, "ran %llu frames, state %016llx\n", (unsigned long long)frames, (unsigned long long)gb_state_hash(gb));
  if (!gb->hung && end != GAME_FAILED) save_sram(gb, sav);
  session.gb = NULL;
  playing = NULL;
  if (audio) SDL_DestroyAudioStream(audio);
  SDL_SetWindowTitle(win, "Oracles");
  // Closes every mod's Lua state now rather than leaving them alive (with handlers registered
  // against this game) for as long as the launcher happens to sit idle, or for a different game
  // entirely if the player picks one next. main()'s loop re-scans game-agnostically before the
  // launcher is shown again, and the next run_game re-inits for whichever game is actually picked.
  features.treasure_swap = NULL;
  mods_shutdown();
  paused_game.gb = NULL;
  wide_npcs_stop();
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
  int file_arg = -1, state_arg = -1;
  bool file_given = false, latest = false;
  uint64_t max_frames = 0;
  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "--game") && i + 1 < argc) game_arg = argv[++i];
    else if (!strcmp(argv[i], "--frames") && i + 1 < argc) max_frames = strtoull(argv[++i], NULL, 10);
    else if (!strcmp(argv[i], "--cache") && i + 1 < argc) cache_arg = argv[++i];
    else if (!strcmp(argv[i], "--file") && i + 1 < argc) { file_given = true; file_arg = atoi(argv[++i]) - 1; }
    else if (!strcmp(argv[i], "--latest")) latest = true;
    else if (!strcmp(argv[i], "--state") && i + 1 < argc) {
      const char *value = argv[++i];
      if (!strcmp(value, "auto")) state_arg = SLOT_AUTO;
      else if (strlen(value) == 1 && value[0] >= '1' && value[0] < '1' + UI_SLOTS) state_arg = value[0] - '1';
      else { fprintf(stderr, "--state must be auto or 1-%d\n", UI_SLOTS); return 2; }
    }
    else if (argv[i][0] != '-') rom_arg = argv[i];
    else { fprintf(stderr, "usage: oracles-native [ROM] [--game ages|seasons] [--file 1-3 | --state auto|1-4 | --latest] [--cache DIR] [--frames N]\n"); return 2; }
  }
  if (latest && (file_given || state_arg >= 0 || (!rom_arg && !game_arg))) {
    fprintf(stderr, "--latest requires a ROM or --game and cannot be combined with --file or --state\n");
    return 2;
  }
  if (state_arg >= 0 && (file_given || (!rom_arg && !game_arg))) {
    fprintf(stderr, "--state requires a ROM or --game and cannot be combined with --file\n");
    return 2;
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
  else { char *p = SDL_GetPrefPath("oracles-decomp", "oracles"); if (!p) { fprintf(stderr, "%s\n", SDL_GetError()); return 2; } data_folder(cache, sizeof cache, p); SDL_free(p); }
  SDL_CreateDirectory(cache);

  SDL_Window *win;
  SDL_Renderer *ren;
  SDL_Texture *tex;
  char window_size[1100];
  snprintf(window_size, sizeof window_size, "%swindow.txt", cache);
  if (!oracles_open_window("Oracles", window_size, &win, &ren, &tex)) { fprintf(stderr, "%s\n", SDL_GetError()); return 2; }
  if (!gui_init(ren)) { fprintf(stderr, "could not load the menu font\n"); return 2; }
  gui_button_event = SDL_RegisterEvents(1);
  gui_theme(&gui_t, false);
  snprintf(settings_path, sizeof settings_path, "%ssettings.ini", cache);
  snprintf(app_cache, sizeof app_cache, "%s", cache);
  sync_config_load(&sync_cfg, cache);
  settings_reload(win);
  update_start();
  if (settings.widescreen && !file_exists(window_size)) fit_window_to_picture(win);

  // a ROM or --game on the command line starts that game directly; quitting it opens the launcher
  char game[16] = "";
  GameStart gs = {cache, game, LAUNCH_TITLE, false, 0, max_frames};
  bool direct = false;
  char rom_err[40] = {0};
  if (rom_arg) { if (!extract(rom_arg, cache, game, rom_err, sizeof rom_err)) return 2; direct = true; }
  else if (game_arg) {
    // --game ages|seasons starts that game's active ROM; a folder name (ages-eu) picks one
    const char *id = strchr(game_arg, '-') ? game_arg : rom_id(cache, is_ages(game_arg));
    if (!cached(cache, id)) { fprintf(stderr, "%s is not installed; run once with its ROM path\n", game_arg); return 2; }
    snprintf(game, sizeof game, "%s", id);
    direct = true;
    if (file_arg >= 0 && file_arg < UI_FILES) {
      static Launcher probe;
      launcher_load(&probe, cache);
      gs.start_file = file_arg;
      gs.open_file = probe.games[is_ages(game) ? UI_GAME_AGES : UI_GAME_SEASONS].files[file_arg].valid;
    }
  }
  if (state_arg >= 0) {
    gs.start_file = state_arg == SLOT_AUTO ? LAUNCH_RESUME : LAUNCH_SLOTS;
    gs.start_slot = state_arg;
  }
  if (latest && !latest_start(&gs)) { oracles_close_window(win, window_size); SDL_Quit(); return 1; }
  // The spec wants settings rows a mod asked for to exist "from the launcher", before any game
  // has been chosen, so a player can configure a mod before playing -- but mods_init takes a
  // `seasons` bool, and the launcher does not commit to a game until LAUNCH_PLAY. Resolved by
  // scanning game-agnostically here, defaulting to Seasons (true): a mod declared for either game,
  // or for Seasons only, gets its rows now; an Ages-only mod's rows do not appear until Ages is
  // actually played, since nothing short of running the scan twice (once per game) could do
  // better, and mods_setting_* is already documented session-only (Task 9) in the same spirit.
  // run_game() below re-inits with the real `seasons` the moment a game actually starts, which is
  // also what makes a mod's on_frame/oracles.rd see the correct game rather than this default.
  char mods_dir[1200];
  snprintf(mods_dir, sizeof mods_dir, "%smods", cache);
  int status = 0;
  for (;;) {
    if (!direct) {
      mods_init(mods_dir, true);
      if (!run_launcher(win, ren, tex, cache, game, &gs.start_file, &gs.open_file, &gs.start_slot)) break;
    }
    direct = false;
    GameEnd end = run_game(win, ren, tex, &gs);
    if (sync_cfg.code[0] && end != GAME_FAILED && end != GAME_STOPPED && !max_frames) {
      sync_wait(20000);
      SyncResult earlier;
      sync_poll(&earlier);
      sync_start(&sync_cfg, cache, game);
    }
    if (end == GAME_STOPPED) {
      if (max_frames) { status = 1; break; }
      if (!run_error(win, ren, tex)) break;
    }
    if (end == GAME_FAILED) { status = 1; break; }
    if (end == GAME_QUIT || max_frames) break;
    gs.start_file = LAUNCH_TITLE;
    gs.open_file = false;
  }
  if (!sync_wait(20000)) fprintf(stderr, "sync still running at exit\n");
  mods_shutdown();
  oracles_close_window(win, window_size);
  // Restart (after Portable changed, or on the AppImage an update replaced): the quit above saved
  // everything; the new run opens the other folder, or is the new version
  if (update_restart) argv[0] = (char *)update_appimage();
  if (update_restart && update_test()) fprintf(stderr, "restart %s\n", argv[0]);
  else if ((restart_tapped || update_restart) && !SDL_CreateProcess((const char *const *)argv, false)) fprintf(stderr, "could not restart: %s\n", SDL_GetError());
  SDL_Quit();
  return status;
}
