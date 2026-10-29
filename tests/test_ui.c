#include "unit.h"
#include "core/gb.h"
#include "ui/ui.h"
#include "ui/savefile.h"
#include "ui/launcher.h"
#include "ui/home.h"
#include "ui/menu.h"
#include "ui/settings.h"
#include "ui/filter.h"
#include "ui/upscale.h"
#include "ui/touch.h"
#include "ui/syncui.h"
#include "ui/labels.h"

static uint8_t rom[0x80000];

static void font_loads_from_the_rom_offset(void) {
  UiFont font;
  ASSERT(!ui_font_load(&font, rom, 0x1000));
  size_t off = 0x1c * 0x4000 + 0x0720;
  memset(rom + off, 0xff, 128 * 16);                   // every glyph blank: set bits are paper
  rom[off + 'A' * 16 + 3] = 0x7e;
  ASSERT(ui_font_load(&font, rom, sizeof rom));
  ASSERT_EQ(font.glyph['A'][3], 0x7e);
}

static void make_file(uint8_t *file, bool ages, const char *name, int max_health, uint8_t essences) {
  memset(file, 0, 0x550);
  memcpy(file + 2, ages ? "Z21216-0" : "Z11216-0", 8);
  memcpy(file + 0x52, name, strlen(name));
  int oh = ages ? 0xfa : 0xf2;
  file[oh] = (uint8_t)max_health; file[oh + 1] = (uint8_t)max_health;
  file[ages ? 0x10f : 0x10b] = essences;
  uint16_t sum = ui_file_checksum(file);
  file[0] = sum & 0xff; file[1] = sum >> 8;
}

static void save_files_are_read_and_verified(void) {
  static uint8_t sram[0x2000];
  memset(sram, 0, sizeof sram);
  make_file(sram + 0x0010, false, "LINK", 20, 0x05);
  make_file(sram + 0x1550, false, "ZELDA", 12, 0);     // only the backup of file 2 is valid
  make_file(sram + 0x0ab0, true, "AGES", 12, 0);        // an Ages file in a Seasons save: rejected
  UiFileInfo f[UI_FILES];
  ASSERT_EQ(ui_read_files(sram, sizeof sram, false, f), 2);
  ASSERT(f[0].valid && strcmp(f[0].name, "LINK") == 0);
  ASSERT_EQ(f[0].max_health, 20);
  ASSERT_EQ(f[0].essence_count, 2);
  ASSERT(f[1].valid && strcmp(f[1].name, "ZELDA") == 0);
  ASSERT(!f[2].valid);
  sram[0x0010 + 0x60] ^= 1;                             // corrupt file 1: checksum fails
  ASSERT_EQ(ui_read_files(sram, sizeof sram, false, f), 1);
  ASSERT(!f[0].valid);
  ASSERT_EQ(ui_read_files(NULL, 0, false, f), 0);
  make_file(sram + 0x0010, false, "\xc3\xd2\xab" "go", 20, 0);   // the European name entry: Í, á, ñ
  ASSERT_EQ(ui_read_files(sram, sizeof sram, false, f), 2);
  ASSERT(f[0].valid && strcmp(f[0].name, "Iango") == 0);
}

static void home_leads_with_resume_and_slots(void) {
  static Home h;
  memset(&h, 0, sizeof h);
  h.l.games[UI_GAME_SEASONS].installed = true;
  h.slots[UI_GAME_SEASONS][1] = (UiSlot){.used = true, .time = 200};
  h.slots[UI_GAME_SEASONS][3] = (UiSlot){.used = true, .time = 100};
  home_init(&h, UI_GAME_AGES);
  ASSERT_EQ(h.l.game, UI_GAME_SEASONS);                 // Ages isn't installed
  ASSERT_EQ(home_latest(&h), 1);
  ASSERT_EQ(h.row, HOME_SLOT1 + 1);                     // no resume state: the newest slot
  LaunchResult r = home_press(&h, UI_ACCEPT);
  ASSERT_EQ(r.action, LAUNCH_PLAY);
  ASSERT_EQ(r.file, LAUNCH_SLOTS);
  ASSERT_EQ(r.slot, 1);
  home_press(&h, UI_DOWN);
  ASSERT_EQ(h.row, HOME_SLOT4);                         // empty slot 3 is skipped
  home_press(&h, UI_DOWN);
  ASSERT_EQ(h.row, HOME_FILES);
  ASSERT_EQ(home_press(&h, UI_ACCEPT).action, LAUNCH_NONE);   // opens the files page
  ASSERT(h.files_page);
  home_press(&h, UI_DOWN);
  r = home_press(&h, UI_ACCEPT);
  ASSERT_EQ(r.action, LAUNCH_PLAY);
  ASSERT_EQ(r.file, 1);
  ASSERT_EQ(home_press(&h, UI_BACK).action, LAUNCH_NONE);     // leaves the files page, not the menu
  ASSERT(!h.files_page);
  home_press(&h, UI_DOWN);
  ASSERT_EQ(home_press(&h, UI_ACCEPT).file, LAUNCH_TITLE);
  home_press(&h, UI_DOWN);
  ASSERT_EQ(h.row, HOME_ADD_ROM);
  ASSERT_EQ(home_press(&h, UI_ACCEPT).action, LAUNCH_ADD_ROM);
  home_press(&h, UI_DOWN);
  ASSERT_EQ(h.row, HOME_SLOT1 + 1);                     // wraps past Resume (none) and slot 1 (empty)
  h.l.games[UI_GAME_SEASONS].has_resume = true;
  home_tap_tab(&h, UI_GAME_SEASONS);
  ASSERT_EQ(h.row, HOME_RESUME);
  ASSERT_EQ(home_press(&h, UI_ACCEPT).file, LAUNCH_RESUME);
  ASSERT_EQ(home_press(&h, UI_BACK).action, LAUNCH_QUIT);
}

static void home_tabs_reach_settings_and_add_rom(void) {
  static Home h;
  memset(&h, 0, sizeof h);
  h.l.games[UI_GAME_SEASONS].installed = true;
  home_init(&h, UI_GAME_SEASONS);
  ASSERT_EQ(h.row, HOME_FILES);                         // nothing to continue: the save files
  home_press(&h, UI_RIGHT);
  ASSERT(h.l.settings_tab);
  ASSERT_EQ(home_press(&h, UI_DOWN).action, LAUNCH_NONE);
  LaunchResult r = home_press(&h, UI_LEFT);
  ASSERT_EQ(r.action, LAUNCH_SETTINGS);
  ASSERT_EQ(r.button, UI_LEFT);
  home_press(&h, UI_BACK);
  home_press(&h, UI_RIGHT);                             // wraps to Ages, not installed
  ASSERT_EQ(h.l.game, UI_GAME_AGES);
  ASSERT_EQ(home_press(&h, UI_ACCEPT).action, LAUNCH_ADD_ROM);
}

static void pause_menu_skips_disabled_items(void) {
  PauseMenu m;
  pause_open(&m, false, false);
  PauseItem picked;
  pause_press(&m, UI_DOWN, &picked);
  ASSERT_EQ(m.sel, PAUSE_SETTINGS);                     // save and load are unavailable
  pause_press(&m, UI_DOWN, &picked);
  ASSERT_EQ(pause_press(&m, UI_ACCEPT, &picked), MENU_PICK);
  ASSERT_EQ(picked, PAUSE_QUIT);
  pause_open(&m, true, true);
  pause_press(&m, UI_DOWN, &picked);
  ASSERT_EQ(m.sel, PAUSE_SAVE);
  ASSERT_EQ(pause_press(&m, UI_BACK, &picked), MENU_BACK);
}

static void slots_load_only_used_slots(void) {
  static SlotsMenu m;
  memset(&m, 0, sizeof m);
  m.slots[2].used = true;
  slots_open(&m, false);
  ASSERT_EQ(m.sel, 2);
  int slot = -1;
  slots_press(&m, UI_DOWN, &slot);
  ASSERT_EQ(m.sel, 2);                                  // no other used slot to move to
  ASSERT_EQ(slots_press(&m, UI_ACCEPT, &slot), MENU_PICK);
  ASSERT_EQ(slot, 2);
  slots_open(&m, true);
  ASSERT_EQ(m.sel, 0);
  slots_press(&m, UI_UP, &slot);
  ASSERT_EQ(m.sel, 3);                                  // saving: every slot, wrapping
}

static void themes_come_from_the_title_palettes(void) {
  UiTheme t;
  ASSERT(!ui_theme_load(&t, rom, 0x100, true));        // no ROM: the built-in Ages colours
  ASSERT_EQ(t.panel.b, 139);
  size_t off = 0x17 * 0x4000 + 0x18;
  rom[off + 6] = 0x1f; rom[off + 7] = 0x00;             // bg0 colour 3 = pure red (BGR555)
  ASSERT(ui_theme_load(&t, rom, sizeof rom, true));
  ASSERT_EQ(t.panel.r, 255);
  ASSERT_EQ(t.panel.b, 0);
  ui_theme_default(&t, false);
  ASSERT_EQ(t.border.r, 213);                           // Seasons gold
}

static void settings_round_trip(void) {
  Settings s, back;
  settings_default(&s);
  ASSERT_EQ(s.volume, 10);
  ASSERT(!s.essence_tracker);
  s.essence_tracker = true;
  s.volume = 3; s.filter = FILTER_CRT; s.gbc_colours = true; s.fullscreen = true; s.fill = true;
  s.widescreen = true; s.dim_sides = false;
  ASSERT(s.haptics);                                    // on until the player turns it off
  s.haptics = false;
  ASSERT_EQ(s.ff_speed, 3);
  s.ff_speed = FF_SPEED_MAX;
  char buf[1024];
  settings_format(&s, buf, sizeof buf);
  settings_default(&back);
  settings_parse(&back, buf);
  ASSERT(back.essence_tracker);
  ASSERT_EQ(back.volume, 3);
  ASSERT_EQ(back.filter, FILTER_CRT);
  ASSERT_EQ(back.ff_speed, FF_SPEED_MAX);
  ASSERT(back.gbc_colours && back.fullscreen && back.fill && back.widescreen && !back.dim_sides && !back.haptics);
  settings_parse(&back, "volume=99\nfilter=weird\nunknown=1\n  volume = 7\nfast_forward=9\nfast_forward=1\n");
  ASSERT_EQ(back.volume, 7);                            // 99 rejected, spaced line accepted
  ASSERT_EQ(back.filter, FILTER_CRT);
  ASSERT_EQ(back.ff_speed, FF_SPEED_MAX);               // above the top and below the bottom: both rejected
  ASSERT_EQ(s.pad_style, STYLE_AUTO);
  s.pad_style = STYLE_PLAYSTATION;
  settings_format(&s, buf, sizeof buf);
  settings_parse(&back, buf);
  ASSERT_EQ(back.pad_style, STYLE_PLAYSTATION);
  settings_parse(&back, "controller_style=sega\n");    // unknown: kept as it was
  ASSERT_EQ(back.pad_style, STYLE_PLAYSTATION);
  s.pad_style = (PadStyle)99;                          // out of range: written as auto
  settings_format(&s, buf, sizeof buf);
  back.pad_style = STYLE_XBOX;                         // so a missing line would show
  settings_parse(&back, buf);
  ASSERT_EQ(back.pad_style, STYLE_AUTO);
}

static void settings_menu_changes_values(void) {
  Settings s;
  SettingsMenu m;
  settings_default(&s);
  settings_menu_open(&m, false);                        // a computer: no Vibration row, keys step past it
  SettingsRow row;
  settings_press(&m, &s, UI_RIGHT, &row);
  ASSERT_EQ(s.volume, 10);                              // capped
  settings_press(&m, &s, UI_LEFT, &row);
  ASSERT_EQ(s.volume, 9);
  settings_press(&m, &s, UI_DOWN, &row);
  settings_press(&m, &s, UI_LEFT, &row);
  ASSERT_EQ(s.filter, FILTERS - 1);                     // wraps backwards, to whichever is last
  settings_press(&m, &s, UI_DOWN, &row);
  settings_press(&m, &s, UI_RIGHT, &row);
  ASSERT(s.fill);
  settings_press(&m, &s, UI_UP, &row);
  settings_press(&m, &s, UI_UP, &row);
  settings_press(&m, &s, UI_UP, &row);
  ASSERT_EQ(m.sel, SET_MODS);
  ASSERT_EQ(settings_press(&m, &s, UI_ACCEPT, &row), MENU_PICK);
  ASSERT_EQ(row, SET_MODS);
  settings_press(&m, &s, UI_UP, &row);
  ASSERT_EQ(m.sel, SET_ERROR_LOG);                      // unavailable until a report exists
  ASSERT_EQ(settings_press(&m, &s, UI_ACCEPT, &row), MENU_NONE);
  s.error_log = true;
  ASSERT_EQ(settings_press(&m, &s, UI_ACCEPT, &row), MENU_PICK);
  ASSERT_EQ(row, SET_ERROR_LOG);
  ASSERT_EQ(settings_press(&m, &s, UI_LEFT, &row), MENU_NONE);
  settings_press(&m, &s, UI_UP, &row);
  ASSERT_EQ(m.sel, SET_VERSION);
  ASSERT_EQ(settings_press(&m, &s, UI_ACCEPT, &row), MENU_NONE);   // nothing to download
  snprintf(s.update, sizeof s.update, "1.1.0");
  ASSERT_EQ(settings_press(&m, &s, UI_ACCEPT, &row), MENU_PICK);
  ASSERT_EQ(row, SET_VERSION);
  settings_press(&m, &s, UI_UP, &row);
  ASSERT_EQ(m.sel, SET_PORTABLE);
  ASSERT_EQ(settings_press(&m, &s, UI_ACCEPT, &row), MENU_NONE);
  ASSERT(s.portable);
  settings_press(&m, &s, UI_UP, &row);
  ASSERT_EQ(m.sel, SET_SYNC);
  ASSERT_EQ(settings_press(&m, &s, UI_ACCEPT, &row), MENU_PICK);
  ASSERT_EQ(row, SET_SYNC);
  settings_press(&m, &s, UI_UP, &row);
  ASSERT_EQ(m.sel, SET_PAD_STYLE);
  settings_press(&m, &s, UI_UP, &row);
  ASSERT_EQ(m.sel, SET_CONTROLS);
  settings_press(&m, &s, UI_UP, &row);
  settings_press(&m, &s, UI_ACCEPT, &row);
  ASSERT(s.four_slots);
  settings_press(&m, &s, UI_UP, &row);
  settings_press(&m, &s, UI_ACCEPT, &row);
  ASSERT(s.quick_swap);
  settings_press(&m, &s, UI_UP, &row);
  ASSERT_EQ(m.sel, SET_FF_SPEED);
  settings_press(&m, &s, UI_ACCEPT, &row);
  ASSERT_EQ(s.ff_speed, 4);
  settings_press(&m, &s, UI_LEFT, &row);
  settings_press(&m, &s, UI_LEFT, &row);
  ASSERT_EQ(s.ff_speed, FF_SPEED_MIN);
  settings_press(&m, &s, UI_LEFT, &row);
  ASSERT_EQ(s.ff_speed, FF_SPEED_MAX);                  // wraps, so tapping the row reaches every speed
  settings_press(&m, &s, UI_RIGHT, &row);
  ASSERT_EQ(s.ff_speed, FF_SPEED_MIN);
  settings_press(&m, &s, UI_UP, &row);
  settings_press(&m, &s, UI_ACCEPT, &row);
  ASSERT(s.fast_menus);
  settings_press(&m, &s, UI_UP, &row);
  settings_press(&m, &s, UI_ACCEPT, &row);
  ASSERT(s.fast_text);
  settings_press(&m, &s, UI_DOWN, &row);
  settings_press(&m, &s, UI_DOWN, &row);
  settings_press(&m, &s, UI_DOWN, &row);
  settings_press(&m, &s, UI_DOWN, &row);
  settings_press(&m, &s, UI_DOWN, &row);
  ASSERT_EQ(settings_press(&m, &s, UI_ACCEPT, &row), MENU_PICK);
  ASSERT_EQ(row, SET_CONTROLS);
  ASSERT_EQ(settings_press(&m, &s, UI_BACK, &row), MENU_BACK);
}

static void settings_rows_follow_the_device(void) {
  Settings s;
  SettingsMenu m;
  SettingsRow row;
  settings_default(&s);
  settings_menu_open(&m, false);
  ASSERT(!settings_row_shown(&m, SET_HAPTICS));
  m.sel = SET_FOUR_SLOTS;
  settings_press(&m, &s, UI_DOWN, &row);
  ASSERT_EQ(m.sel, SET_CONTROLS);
  ASSERT(settings_row_shown(&m, SET_FULLSCREEN));
  ASSERT(settings_row_shown(&m, SET_PAD_STYLE));
  ASSERT(settings_row_shown(&m, SET_PORTABLE));
  settings_menu_open(&m, true);                         // a phone: Vibration, and no Fullscreen
  ASSERT(settings_row_shown(&m, SET_HAPTICS));
  ASSERT(!settings_row_shown(&m, SET_FULLSCREEN));
  ASSERT(!settings_row_shown(&m, SET_PORTABLE));        // Android keeps its data where the system says
  ASSERT(settings_row_shown(&m, SET_ERROR_LOG));
  m.sel = SET_ERROR_LOG;
  ASSERT_EQ(settings_press(&m, &s, UI_ACCEPT, &row), MENU_NONE);
  s.error_log = true;
  ASSERT_EQ(settings_press(&m, &s, UI_ACCEPT, &row), MENU_PICK);
  ASSERT_EQ(row, SET_ERROR_LOG);
  m.sel = SET_COLOURS;
  settings_press(&m, &s, UI_DOWN, &row);
  ASSERT_EQ(m.sel, SET_FAST_TEXT);
  m.sel = SET_FOUR_SLOTS;
  settings_press(&m, &s, UI_DOWN, &row);
  ASSERT_EQ(m.sel, SET_HAPTICS);
  settings_press(&m, &s, UI_ACCEPT, &row);
  ASSERT(!s.haptics);
  settings_press(&m, &s, UI_RIGHT, &row);
  ASSERT(s.haptics);
  settings_press(&m, &s, UI_UP, &row);
  ASSERT_EQ(m.sel, SET_FOUR_SLOTS);
  m.sel = SET_CONTROLS;
  settings_press(&m, &s, UI_DOWN, &row);
  ASSERT_EQ(m.sel, SET_PAD_STYLE);                     // shown on phones too
  settings_press(&m, &s, UI_RIGHT, &row);
  ASSERT_EQ(s.pad_style, STYLE_NINTENDO);
  settings_press(&m, &s, UI_LEFT, &row);
  settings_press(&m, &s, UI_LEFT, &row);
  ASSERT_EQ(s.pad_style, STYLE_PLAYSTATION);           // wraps
  ASSERT_EQ(settings_press(&m, &s, UI_ACCEPT, &row), MENU_NONE); // accept steps, not opens
  ASSERT_EQ(s.pad_style, STYLE_AUTO);
}

static void filters_keep_pixel_alignment(void) {
  static uint8_t src[UI_W * UI_H * 3], dst[UI_W * 4 * UI_H * 4 * 3];
  memset(src, 200, sizeof src);
  ui_filter(src, UI_W, false, FILTER_SHARP, 4, dst);
  ASSERT_EQ(dst[0], 200);
  ASSERT_EQ(dst[(3 * UI_W * 4 + 3) * 3], 200);
  ui_filter(src, UI_W, false, FILTER_SCANLINES, 4, dst);
  ASSERT_EQ(dst[0], 200);
  ASSERT_EQ(dst[(3 * UI_W * 4) * 3], 100);              // the last row of each pixel is darker
  ui_filter(src, UI_W, false, FILTER_LCD, 4, dst);
  ASSERT_EQ(dst[3 * 3], 150);                           // the last column too
  ui_filter(src, UI_W, false, FILTER_CRT, 4, dst);
  ASSERT_EQ(dst[0], 0);                                 // the curved screen leaves the corner black
  static uint8_t wide[256 * UI_H * 3], wdst[256 * 3 * UI_H * 3 * 3];
  memset(wide, 10, sizeof wide);
  wide[(5 * 256 + 255) * 3] = 250;                      // the last column of the 256-wide picture
  ui_filter(wide, 256, false, FILTER_SHARP, 3, wdst);
  ASSERT_EQ(wdst[((5 * 3 + 2) * 256 * 3 + 255 * 3 + 2) * 3], 250);
  ASSERT_EQ(wdst[((5 * 3) * 256 * 3 + 254 * 3) * 3], 10);
  uint8_t in[3] = {255, 255, 255}, out[3];
  ui_gbc_colour(in, out);
  ASSERT(out[0] < 255 && out[2] < out[0]);              // paler and warm
}

static void a_tap_between_frames_reaches_the_game(void) {
  TapLatch t = {0};
  uint8_t held = tap_press(&t, 0, JOY_B);
  held = tap_release(&t, held, JOY_B);
  ASSERT_EQ(held, JOY_B);                               // no frame saw it yet: still down
  held = tap_frame_ran(&t, held);
  ASSERT_EQ(held, 0);                                   // one frame later it is up
  held = tap_press(&t, held, JOY_A);
  held = tap_frame_ran(&t, held);
  held = tap_release(&t, held, JOY_A);
  ASSERT_EQ(held, 0);                                   // a press a frame has seen lets go at once
  held = tap_press(&t, held, JOY_A | JOY_UP);
  held = tap_release(&t, held, JOY_A);
  held = tap_press(&t, held, JOY_A);
  held = tap_frame_ran(&t, held);
  ASSERT_EQ(held, JOY_A | JOY_UP);                      // pressed again before the frame: held on
  held = tap_release(&t, held, JOY_DOWN);
  ASSERT_EQ(held, JOY_A | JOY_UP);                      // a release of an unheld button changes nothing
}

static void bindings_remap_and_persist(void) {
  Settings s, back;
  settings_default(&s);
  ASSERT_EQ(bindings_key_action(&s.bindings, 27), ACT_A);        // X
  ASSERT_EQ(bindings_key_action(&s.bindings, 229), ACT_SELECT);  // right shift, a second key
  ASSERT_EQ(bindings_pad_action(&s.bindings, 5), ACT_PAUSE);     // guide
  ASSERT_EQ(bindings_pad_action(&s.bindings, 7), ACT_SAVE_STATE); // left stick
  ASSERT_EQ(bindings_pad_action(&s.bindings, 8), ACT_LOAD_STATE); // right stick
  ASSERT_EQ(s.bindings.key[ACT_SAVE_STATE][0], NO_BINDING);      // Cmd/Ctrl+S is the app's own
  bindings_bind_key(&s.bindings, ACT_B, 27);                    // X moves from A to B
  ASSERT_EQ(bindings_key_action(&s.bindings, 27), ACT_B);
  ASSERT_EQ(s.bindings.key[ACT_A][0], NO_BINDING);
  ASSERT_EQ(s.bindings.key[ACT_B][1], 29);                      // Z stays as B's second key
  bindings_bind_pad(&s.bindings, ACT_FAST, 0);
  ASSERT_EQ(s.bindings.pad[ACT_A], NO_BINDING);
  char buf[512];
  settings_format(&s, buf, sizeof buf);
  settings_default(&back);
  settings_parse(&back, buf);
  ASSERT(memcmp(&back.bindings, &s.bindings, sizeof s.bindings) == 0);
  settings_parse(&back, "keys=1,2;3\n");                        // malformed: ignored
  ASSERT(memcmp(&back.bindings, &s.bindings, sizeof s.bindings) == 0);
  settings_parse(&back, "keys=4,-1\n");                         // older, shorter line: the rest kept
  ASSERT_EQ(back.bindings.key[ACT_UP][0], 4);
  ASSERT_EQ(back.bindings.key[ACT_DOWN][0], s.bindings.key[ACT_DOWN][0]);
  settings_default(&back);                                      // a pads line from before save/load state
  settings_parse(&back, "pads=11;12;13;14;0;2;6;4;5;100;9;3;1\n");
  ASSERT_EQ(back.bindings.pad[ACT_SAVE_STATE], 7);
  ASSERT_EQ(back.bindings.pad[ACT_LOAD_STATE], 8);
  ControlsMenu m;
  controls_open(&m);
  controls_press(&m, &s.bindings, UI_DOWN);                     // DOWN
  controls_press(&m, &s.bindings, UI_ACCEPT);
  ASSERT(m.waiting);
  controls_capture_key(&m, &s.bindings, 22);                     // S
  ASSERT(!m.waiting);
  ASSERT_EQ(bindings_key_action(&s.bindings, 22), ACT_DOWN);
  ASSERT_EQ(s.bindings.key[ACT_DOWN][1], 81);
  m.sel = ACT_PAUSE;
  controls_press(&m, &s.bindings, UI_ACCEPT);
  controls_capture_pad(&m, &s.bindings, 3);                      // a pad button binds from the keyboard page too
  ASSERT(!m.waiting && m.pad_page);
  ASSERT_EQ(bindings_pad_action(&s.bindings, 3), ACT_PAUSE);
  m.sel = CTRL_RESET;
  controls_press(&m, &s.bindings, UI_ACCEPT);
  ASSERT_EQ(bindings_key_action(&s.bindings, 27), ACT_A);
}

static bool overlaps(UiRect a, UiRect b) { return a.x < b.x + b.w && b.x < a.x + a.w && a.y < b.y + b.h && b.y < a.y + a.h; }
static bool inside(UiRect a, int w, int h) { return a.x >= 0 && a.y >= 0 && a.x + a.w <= w && a.y + a.h <= h; }

static void touch_layout_fits_screens(void) {
  static const int sizes[][2] = {{1080, 2400}, {720, 1280}, {1440, 3200}, {1600, 2560}, {1080, 1920}, {2400, 1080}, {1280, 720}, {2560, 1600}, {1920, 1080}};
  for (size_t i = 0; i < sizeof sizes / sizeof sizes[0]; i++)
    for (int variant = 0; variant < 4; variant++) {
      int w = sizes[i][0], h = sizes[i][1], items = variant & 1;
      TouchLayout l;
      touch_layout(&l, w, h, (UiRect){0, 90, w, h - 90 - 60}, true, items, variant >> 1, UI_W);
      ASSERT(l.scale >= 3);
      ASSERT(inside(l.game, l.w, l.h));
      ASSERT(l.game_px.x >= 0 && l.game_px.x + l.game_px.w <= w && l.game_px.y + l.game_px.h <= h);
      if (l.portrait) ASSERT(l.game.y * l.scale >= 90);
      UiRect controls[ACTIONS + 1];
      int n = 0;
      controls[n++] = l.dpad;
      for (int a = 0; a < ACTIONS; a++) if (l.button[a].w) controls[n++] = l.button[a];
      ASSERT_EQ(n, items ? 9 : 7);                     // D-pad, A, B, Start, Select, Pause, Fast (+ X, Y)
      int side = l.game.x;
      for (int j = 0; j < n; j++) {
        ASSERT(inside(controls[j], l.w, l.h - 60 / l.scale));
        if (l.portrait || side >= 70) ASSERT(!overlaps(controls[j], l.game));
        for (int k = 0; k < j; k++) ASSERT(!overlaps(controls[j], controls[k]));
      }
    }
  TouchLayout l;
  touch_layout(&l, 1080, 2400, (UiRect){78, 164, 924, 2152}, true, false, false, UI_W);   // Pixel 8: corners in the safe area
  ASSERT_EQ(l.scale, 6);
  ASSERT(l.game.y * 6 >= 164);
  touch_layout(&l, 2400, 1080, (UiRect){132, 74, 2190, 922}, true, false, false, UI_W);
  ASSERT_EQ(l.scale, 7);
  ASSERT(l.game.x * 7 >= 132);
  touch_layout(&l, 1080, 2400, (UiRect){0, 0, 1080, 2400}, true, false, false, UI_W);
  ASSERT(l.portrait);
  ASSERT_EQ(l.scale, 6);
  ASSERT_EQ(l.game.x, 10);
  touch_layout(&l, 1080, 2400, (UiRect){0, 0, 1080, 2400}, false, false, false, UI_W);
  ASSERT(!l.portrait);
  ASSERT_EQ(l.game.y, (400 - 144) / 2);                // no overlay: centred, nothing to press
  ASSERT_EQ(touch_hit(&l, 100, 300), 0);
}

static void touch_fill_scales_past_whole_pixels(void) {
  TouchLayout l;
  touch_layout(&l, 1080, 2400, (UiRect){78, 164, 924, 2152}, true, false, true, UI_W);   // Pixel 8 portrait
  ASSERT_EQ(l.scale, 6);
  ASSERT_EQ(l.game_px.w, 1080);                        // 6.75x: the full width
  ASSERT_EQ(l.game_px.h, 972);
  ASSERT(l.game_px.y >= 164);
  for (int a = 0; a < ACTIONS; a++) if (l.button[a].w) ASSERT(!overlaps(l.button[a], l.game));
  ASSERT(!overlaps(l.dpad, l.game));
  touch_layout(&l, 2400, 1080, (UiRect){132, 74, 2190, 922}, true, false, true, UI_W);
  ASSERT_EQ(l.game_px.h, 1080);                        // landscape: the full height
  ASSERT_EQ(l.game_px.w, 1200);
  touch_layout(&l, 1000, 700, (UiRect){0, 0, 1000, 700}, false, false, true, UI_W);        // desktop window
  ASSERT_EQ(l.game_px.w, 777);
  ASSERT_EQ(l.game_px.h, 700);
  touch_layout(&l, 1000, 700, (UiRect){0, 0, 1000, 700}, false, false, false, UI_W);
  ASSERT_EQ(l.game_px.w, 640);
  ASSERT_EQ(l.game_px.x % 4, 0);
}

static void touch_layout_takes_the_wide_picture(void) {
  TouchLayout l;
  TouchLayout narrow;
  touch_layout(&narrow, 1080, 2400, (UiRect){78, 164, 924, 2152}, true, false, false, UI_W);
  touch_layout(&l, 1080, 2400, (UiRect){78, 164, 924, 2152}, true, false, false, 256);
  ASSERT_EQ(l.scale, 6);                                // the controls stay as big as beside 160
  ASSERT_EQ(l.game_px.w, 1024);                         // 256 x 4 = 1024 of 1080
  ASSERT_EQ(l.game_px.x, 28);
  ASSERT_EQ(l.dpad.w, narrow.dpad.w);
  ASSERT_EQ(l.button[ACT_A].w, narrow.button[ACT_A].w);
  for (int a = 0; a < ACTIONS; a++) if (l.button[a].w) ASSERT(!overlaps(l.button[a], l.game));
  ASSERT(!overlaps(l.dpad, l.game));
  touch_layout(&l, 1080, 2400, (UiRect){78, 164, 924, 2152}, true, false, true, 256);
  ASSERT_EQ(l.scale, 6);
  ASSERT_EQ(l.game_px.w, 1080);                         // fill: the full width
  for (int a = 0; a < ACTIONS; a++) if (l.button[a].w) ASSERT(!overlaps(l.button[a], l.game));
  touch_layout(&l, 2400, 1080, (UiRect){132, 74, 2190, 922}, true, false, false, 256);
  ASSERT_EQ(l.scale, 7);
  ASSERT_EQ(l.game_px.w, 1792);
  touch_layout(&l, 1920, 1080, (UiRect){0, 0, 1920, 1080}, false, false, false, 256);
  ASSERT_EQ(l.scale, 7);
  ASSERT_EQ(l.game_px.w, 1792);
  ASSERT(abs(l.game_px.x - (1920 - 1792) / 2) < l.scale);  // centred on the pixel grid
  touch_layout(&l, 1920, 1080, (UiRect){0, 0, 1920, 1080}, false, false, true, 256);
  ASSERT_EQ(l.game_px.w, 1920);                         // fill: 16:9 exactly fills 16:9
  ASSERT_EQ(l.game_px.h, 1080);
}

static void touch_hits_each_control(void) {
  TouchLayout l;
  touch_layout(&l, 1080, 2400, (UiRect){0, 0, 1080, 2400}, true, true, false, UI_W);
  float x, y;
  for (int a = 0; a < ACTIONS; a++) {
    bool shown = touch_point(&l, (Action)a, &x, &y);
    ASSERT_EQ(shown, a != ACT_SWAP && a != ACT_SAVE_STATE && a != ACT_LOAD_STATE);  // those three are keys and buttons only
    if (shown) ASSERT_EQ(touch_hit(&l, x, y), 1u << a);
  }
  float r = l.dpad.w / 2.0f, cx = l.dpad.x + r, cy = l.dpad.y + r;
  ASSERT_EQ(touch_hit(&l, cx + r * 0.6f, cy - r * 0.6f), (1u << ACT_UP) | (1u << ACT_RIGHT));
  ASSERT_EQ(touch_hit(&l, cx - r * 0.7f, cy + r * 0.2f), 1u << ACT_LEFT);
  ASSERT_EQ(touch_hit(&l, cx, cy), 0);                 // the centre is a dead zone
  ASSERT_EQ(touch_hit(&l, l.game.x + 80, l.game.y + 72), 0);
  touch_layout(&l, 1080, 2400, (UiRect){0, 0, 1080, 2400}, true, false, false, UI_W);
  ASSERT(!touch_point(&l, ACT_ITEM_X, &x, &y));
}

static void sync_page_shows_the_rows_for_its_state(void) {
  SyncMenu m;
  SyncRow row;
  syncmenu_open(&m, false, "", "");
  ASSERT_EQ(m.sel, SYNCROW_CREATE);
  syncmenu_press(&m, UI_DOWN, &row);
  ASSERT_EQ(m.sel, SYNCROW_ENTER);
  syncmenu_press(&m, UI_DOWN, &row);
  ASSERT_EQ(m.sel, SYNCROW_CREATE);                     // off: only create and enter
  syncmenu_open(&m, true, "4827-1930-5561-0284", "Synced at 17:32");
  ASSERT_EQ(m.sel, SYNCROW_NOW);
  syncmenu_press(&m, UI_DOWN, &row);
  ASSERT_EQ(m.sel, SYNCROW_ENTER);                      // on: no create
  syncmenu_press(&m, UI_DOWN, &row);
  ASSERT_EQ(syncmenu_press(&m, UI_ACCEPT, &row), MENU_PICK);
  ASSERT_EQ(row, SYNCROW_OFF);
  ASSERT_EQ(syncmenu_press(&m, UI_BACK, &row), MENU_BACK);
}

static void code_entry_edits_each_digit(void) {
  CodeEntry e;
  codeentry_open(&e, "");
  ASSERT(!strcmp(e.digits, "0000000000000000"));
  codeentry_press(&e, UI_DOWN);                         // 0 wraps to 9
  codeentry_press(&e, UI_LEFT);                         // the first digit wraps to the last
  codeentry_press(&e, UI_UP);
  codeentry_press(&e, UI_UP);
  ASSERT(!strcmp(e.digits, "9000000000000002"));
  codeentry_open(&e, "4827193055610284");
  codeentry_press(&e, UI_RIGHT);
  codeentry_press(&e, UI_UP);
  ASSERT(!strcmp(e.digits, "4927193055610284"));
  ASSERT_EQ(codeentry_press(&e, UI_ACCEPT), MENU_PICK);
  ASSERT_EQ(codeentry_press(&e, UI_BACK), MENU_BACK);
}

static void conflict_picks_a_side(void) {
  static ConflictView v;
  memset(&v, 0, sizeof v);
  snprintf(v.what, sizeof v.what, "Seasons save files");
  v.side[0].has_files = true;
  v.side[0].files[0].valid = true;
  snprintf(v.side[0].files[0].name, sizeof v.side[0].files[0].name, "LINK");
  v.side[0].files[0].max_health = 12;
  v.side[1].missing = true;
  ASSERT_EQ(v.sel, 0);
  conflict_press(&v, UI_RIGHT);
  ASSERT_EQ(v.sel, 1);
  conflict_press(&v, UI_LEFT);
  ASSERT_EQ(conflict_press(&v, UI_ACCEPT), MENU_PICK);
  ASSERT_EQ(v.sel, 0);
  ASSERT_EQ(conflict_press(&v, UI_BACK), MENU_BACK);
}

// The scalers enlarge by 3x themselves and repeat whole pixels the rest of the way, so the picture
// still lands on the game's pixel grid. XBRZ and HQX share the path, so each test covers both.
static const ScreenFilter upscalers[2] = {FILTER_XBRZ, FILTER_HQX};

static void upscalers_fill_the_whole_picture(void) {
  static uint8_t src[UI_W * UI_H * 3], dst[UI_W * 3 * UI_H * 3 * 3];
  memset(src, 0, sizeof src);
  for (int i = 0; i < UI_W * UI_H; i++) { src[i * 3] = 30; src[i * 3 + 1] = 60; src[i * 3 + 2] = 90; }
  for (int k = 0; k < 2; k++) {
    memset(dst, 0xab, sizeof dst);                      // canary: every pixel must be overwritten
    ui_filter(src, UI_W, false, upscalers[k], 3, dst);
    // One flat colour in means the same flat colour out, everywhere: nothing left unwritten.
    for (int i = 0; i < UI_W * 3 * UI_H * 3; i++) {
      ASSERT_EQ(dst[i * 3], 30);
      ASSERT_EQ(dst[i * 3 + 1], 60);
      ASSERT_EQ(dst[i * 3 + 2], 90);
    }
  }
}

// A hard black/white staircase, the shape these filters exist to round off.
static void make_staircase(uint8_t *src) {
  memset(src, 0, UI_W * UI_H * 3);
  for (int y = 0; y < UI_H; y++)
    for (int x = 0; x < UI_W; x++)
      if (x > y) memset(src + (y * UI_W + x) * 3, 255, 3);
}

static void upscalers_smooth_a_diagonal(void) {
  static uint8_t src[UI_W * UI_H * 3], dst[UI_W * 3 * UI_H * 3 * 3];
  make_staircase(src);
  for (int k = 0; k < 2; k++) {
    ui_filter(src, UI_W, false, upscalers[k], 3, dst);
    int blended = 0;
    for (int i = 0; i < UI_W * 3 * UI_H * 3; i++)
      if (dst[i * 3] != 0 && dst[i * 3] != 255) blended++;
    ASSERT(blended > 0);                                // it interpolated along the edge
    // The flat areas well away from the edge are untouched.
    ASSERT_EQ(dst[((40 * 3) * UI_W * 3 + 4 * 3) * 3], 0);
    ASSERT_EQ(dst[((40 * 3) * UI_W * 3 + 120 * 3) * 3], 255);
  }
}

// At scale 6 the scaler runs at 3x and each of its pixels becomes a 2x2 block of output.
static int uniform_2x2_blocks(const uint8_t *px, int w, int h) {
  for (int y = 0; y < h; y += 2)
    for (int x = 0; x < w; x += 2)
      for (int dy = 0; dy < 2; dy++)
        for (int dx = 0; dx < 2; dx++)
          if (px[((y + dy) * w + x + dx) * 3] != px[(y * w + x) * 3]) return 0;
  return 1;
}

static void upscalers_keep_whole_pixels_past_their_factor(void) {
  static uint8_t src[UI_W * UI_H * 3];
  static uint8_t dst[UI_W * 6 * UI_H * 6 * 3];
  make_staircase(src);
  for (int k = 0; k < 2; k++) {
    ui_filter(src, UI_W, false, upscalers[k], 6, dst);
    ASSERT(uniform_2x2_blocks(dst, UI_W * 6, UI_H * 6));
  }
  // At scale 3 the scaler covers the whole enlargement, so there is detail inside those blocks.
  static uint8_t exact[UI_W * 3 * UI_H * 3 * 3];
  for (int k = 0; k < 2; k++) {
    ui_filter(src, UI_W, false, upscalers[k], 3, exact);
    ASSERT(!uniform_2x2_blocks(exact, UI_W * 3, UI_H * 3));
  }
}

// The widescreen picture is 256 wide, so the filters must not assume the 160 of the plain one.
// A lone pixel is no use as a probe here -- blending neighbours together is what these are for --
// so this checks the whole output is written and that each half stays on its own side.
static void upscalers_take_the_wide_picture(void) {
  static uint8_t wide[256 * UI_H * 3], wdst[256 * 3 * UI_H * 3 * 3];
  memset(wide, 7, sizeof wide);
  for (int y = 0; y < UI_H; y++)
    for (int x = 128; x < 256; x++) wide[(y * 256 + x) * 3] = 250;
  const int w = 256 * 3;
  for (int k = 0; k < 2; k++) {
    memset(wdst, 0xab, sizeof wdst);
    ui_filter(wide, 256, false, upscalers[k], 3, wdst);
    for (int i = 0; i < w * UI_H * 3; i++) ASSERT(wdst[i * 3] == 7 || wdst[i * 3] == 250);
    ASSERT_EQ(wdst[0], 7);                              // far left
    ASSERT_EQ(wdst[(w - 1) * 3], 250);                  // far right of the first row
    ASSERT_EQ(wdst[((UI_H * 3 - 1) * w + w - 1) * 3], 250);   // and the very last pixel
  }
}

// Colour correction has to happen before the scalers, so the pixels they compare are the ones shown.
static void upscalers_apply_gbc_colours_first(void) {
  static uint8_t src[UI_W * UI_H * 3], dst[UI_W * 3 * UI_H * 3 * 3];
  memset(src, 255, sizeof src);
  uint8_t in[3] = {255, 255, 255}, want[3];
  ui_gbc_colour(in, want);
  for (int k = 0; k < 2; k++) {
    ui_filter(src, UI_W, true, upscalers[k], 3, dst);
    ASSERT_EQ(dst[0], want[0]);
    ASSERT_EQ(dst[2], want[2]);
  }
}

// The two scalers are different algorithms, so they must not be quietly producing the same picture
// (which would mean one of them was never wired up). A plain 45-degree two-colour edge is no use as
// a probe -- both round it off the same obvious way -- so this uses several colours and curves.
static void the_two_scalers_differ(void) {
  static const uint8_t pal[5][3] = {{0,0,0},{255,255,255},{200,40,40},{40,200,80},{60,80,220}};
  static uint8_t src[UI_W * UI_H * 3];
  static uint8_t a[UI_W * 3 * UI_H * 3 * 3], b[UI_W * 3 * UI_H * 3 * 3];
  for (int y = 0; y < UI_H; y++)
    for (int x = 0; x < UI_W; x++)
      memcpy(src + (y * UI_W + x) * 3, pal[((x * x + y * y) / 97 + x / 7 + y / 5) % 5], 3);
  ui_filter(src, UI_W, false, FILTER_XBRZ, 3, a);
  ui_filter(src, UI_W, false, FILTER_HQX, 3, b);
  ASSERT(memcmp(a, b, sizeof a) != 0);
}

// A factor above the window scale would mean scaling up and sampling back down, throwing the work
// away; it is clamped to the scale instead.
static void upscale_factor_is_capped_at_the_window_scale(void) {
  static uint8_t src[UI_W * UI_H * 3];
  static uint8_t asked[UI_W * 2 * UI_H * 2 * 3], capped[UI_W * 2 * UI_H * 2 * 3];
  make_staircase(src);
  ui_xbrz_scale(src, UI_W, UI_H, 6, 2, asked);
  ui_xbrz_scale(src, UI_W, UI_H, 2, 2, capped);
  ASSERT_EQ(memcmp(asked, capped, sizeof asked), 0);
  ui_hqx_scale(src, UI_W, UI_H, 3, 2, asked);
  ui_hqx_scale(src, UI_W, UI_H, 2, 2, capped);
  ASSERT_EQ(memcmp(asked, capped, sizeof asked), 0);
}

static void labels_follow_device_style_and_bindings(void) {
  ASSERT_EQ(pad_style_resolve(STYLE_AUTO, STYLE_PLAYSTATION), STYLE_PLAYSTATION);
  ASSERT_EQ(pad_style_resolve(STYLE_AUTO, STYLE_AUTO), STYLE_XBOX);              // no pad seen yet
  ASSERT_EQ(pad_style_resolve(STYLE_NINTENDO, STYLE_PLAYSTATION), STYLE_NINTENDO); // the setting wins
  const char *ok, *back;
  menu_hint_keys(DEVICE_KEYBOARD, STYLE_PLAYSTATION, &ok, &back);
  ASSERT(!strcmp(ok, "Enter") && !strcmp(back, "Esc"));
  menu_hint_keys(DEVICE_PAD, STYLE_PLAYSTATION, &ok, &back);
  ASSERT(!strcmp(ok, "Cross") && !strcmp(back, "Circle"));
  menu_hint_keys(DEVICE_PAD, STYLE_XBOX, &ok, &back);
  ASSERT(!strcmp(ok, "A") && !strcmp(back, "B"));
  menu_hint_keys(DEVICE_PAD, STYLE_NINTENDO, &ok, &back);
  ASSERT(!strcmp(ok, "A") && !strcmp(back, "B"));
  ASSERT(!strcmp(pad_style_name(STYLE_PLAYSTATION, 2), "SQUARE"));
  ASSERT(!strcmp(pad_style_name(STYLE_XBOX, 4), "VIEW"));
  ASSERT(pad_style_name(STYLE_NINTENDO, 0) == NULL);
  ASSERT(!strcmp(pad_style_name(STYLE_PLAYSTATION, PAD_RIGHT_TRIGGER), "R2"));
  Bindings b;
  bindings_default(&b);
  // keyboard: A X, B Z, Item X A, Item Y S
  ASSERT_EQ(hud_slot_glyph(DEVICE_KEYBOARD, STYLE_XBOX, &b, HUD_A), 'X');
  ASSERT_EQ(hud_slot_glyph(DEVICE_KEYBOARD, STYLE_XBOX, &b, HUD_B), 'Z');
  ASSERT_EQ(hud_slot_glyph(DEVICE_KEYBOARD, STYLE_XBOX, &b, HUD_Y), 'S');
  bindings_bind_key(&b, ACT_A, 44);                                      // Space: no glyph
  ASSERT_EQ(hud_slot_glyph(DEVICE_KEYBOARD, STYLE_XBOX, &b, HUD_A), 'A');
  bindings_bind_key(&b, ACT_B, 31);                                      // 2
  ASSERT_EQ(hud_slot_glyph(DEVICE_KEYBOARD, STYLE_XBOX, &b, HUD_B), '2');
  // pads: A SOUTH, B WEST, Item X NORTH, Item Y EAST
  ASSERT_EQ(hud_slot_glyph(DEVICE_PAD, STYLE_XBOX, &b, HUD_B), 'X');
  ASSERT_EQ(hud_slot_glyph(DEVICE_PAD, STYLE_XBOX, &b, HUD_X), 'Y');
  ASSERT_EQ(hud_slot_glyph(DEVICE_PAD, STYLE_PLAYSTATION, &b, HUD_A), HUD_GLYPH_CROSS);
  ASSERT_EQ(hud_slot_glyph(DEVICE_PAD, STYLE_PLAYSTATION, &b, HUD_Y), HUD_GLYPH_CIRCLE);
  ASSERT_EQ(hud_slot_glyph(DEVICE_PAD, STYLE_NINTENDO, &b, HUD_B), 'B');  // Game Boy letters
  bindings_bind_pad(&b, ACT_A, 9);                                        // LB: no glyph
  ASSERT_EQ(hud_slot_glyph(DEVICE_PAD, STYLE_XBOX, &b, HUD_A), 'A');
  ASSERT_EQ(hud_slot_glyph(DEVICE_TOUCH, STYLE_PLAYSTATION, &b, HUD_A), 'A');
  bindings_default(&b);                                                   // A on NORTH, as the spec has it
  bindings_bind_pad(&b, ACT_A, 3);
  ASSERT_EQ(hud_slot_glyph(DEVICE_PAD, STYLE_XBOX, &b, HUD_A), 'Y');
  ASSERT_EQ(hud_slot_glyph(DEVICE_PAD, STYLE_PLAYSTATION, &b, HUD_A), HUD_GLYPH_TRIANGLE);
  bindings_default(&b);
  b.key[ACT_ITEM_X][0] = b.key[ACT_ITEM_X][1] = NO_BINDING;               // unbound: the slot letter
  ASSERT_EQ(hud_slot_glyph(DEVICE_KEYBOARD, STYLE_XBOX, &b, HUD_X), 'X');
  ASSERT(pad_style_name(STYLE_XBOX, 11) == NULL);                         // d-pad: SDL's own name
}

int main(void) {
  RUN(font_loads_from_the_rom_offset);
  RUN(save_files_are_read_and_verified);
  RUN(home_leads_with_resume_and_slots);
  RUN(home_tabs_reach_settings_and_add_rom);
  RUN(pause_menu_skips_disabled_items);
  RUN(slots_load_only_used_slots);
  RUN(themes_come_from_the_title_palettes);
  RUN(settings_round_trip);
  RUN(settings_menu_changes_values);
  RUN(settings_rows_follow_the_device);
  RUN(filters_keep_pixel_alignment);
  RUN(upscalers_fill_the_whole_picture);
  RUN(upscalers_smooth_a_diagonal);
  RUN(upscalers_keep_whole_pixels_past_their_factor);
  RUN(upscalers_take_the_wide_picture);
  RUN(upscalers_apply_gbc_colours_first);
  RUN(the_two_scalers_differ);
  RUN(upscale_factor_is_capped_at_the_window_scale);
  RUN(a_tap_between_frames_reaches_the_game);
  RUN(bindings_remap_and_persist);
  RUN(labels_follow_device_style_and_bindings);
  RUN(touch_layout_fits_screens);
  RUN(sync_page_shows_the_rows_for_its_state);
  RUN(code_entry_edits_each_digit);
  RUN(conflict_picks_a_side);
  RUN(touch_fill_scales_past_whole_pixels);
  RUN(touch_layout_takes_the_wide_picture);
  RUN(touch_hits_each_control);
  return 0;
}
