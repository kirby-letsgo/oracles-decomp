#include "unit.h"
#include "ui/ui.h"
#include "ui/savefile.h"
#include "ui/launcher.h"
#include "ui/menu.h"
#include "ui/settings.h"
#include "ui/filter.h"
#include "ui/upscale.h"
#include "ui/touch.h"
#include "ui/syncui.h"

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

static void text_draws_glyph_pixels(void) {
  UiFont font;
  ASSERT(ui_font_load(&font, rom, sizeof rom));
  static UiCanvas c;
  ui_clear(&c, UI_BLACK);
  ASSERT_EQ(ui_text(&c, &font, 10, 20, "AA", UI_WHITE), 16);
  ASSERT_EQ(c.px[23][10][0], UI_WHITE.r);
  ASSERT_EQ(c.px[23][17][0], UI_WHITE.r);
  ASSERT_EQ(c.px[23][11][0], 0);
  ASSERT_EQ(c.px[23][18][0], UI_WHITE.r);
  ui_text(&c, &font, UI_W - 4, UI_H - 4, "A", UI_WHITE);   // clipped, no crash
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
}

static void launcher_navigates_and_starts(void) {
  Launcher l;
  memset(&l, 0, sizeof l);
  l.games[UI_GAME_SEASONS].installed = true;
  l.games[UI_GAME_SEASONS].files[1].valid = true;
  launcher_init(&l, UI_GAME_AGES);
  ASSERT_EQ(l.game, UI_GAME_SEASONS);                   // Ages isn't installed
  ASSERT_EQ(l.row, 1);                                  // first file with data
  LaunchResult r = launcher_press(&l, UI_ACCEPT);
  ASSERT_EQ(r.action, LAUNCH_PLAY);
  ASSERT_EQ(r.game, UI_GAME_SEASONS);
  ASSERT_EQ(r.file, 1);
  launcher_press(&l, UI_DOWN); launcher_press(&l, UI_DOWN);
  ASSERT_EQ(launcher_press(&l, UI_ACCEPT).file, -1);    // the title-screen row
  launcher_press(&l, UI_DOWN);
  ASSERT_EQ(l.row, 0);                                  // wraps
  launcher_press(&l, UI_RIGHT);                         // Seasons -> the settings tab
  ASSERT(l.settings_tab && !l.settings_list);
  ASSERT_EQ(l.game, UI_GAME_SEASONS);                   // keeps Seasons' colours
  ASSERT_EQ(launcher_press(&l, UI_DOWN).action, LAUNCH_NONE);   // into the list
  ASSERT(l.settings_list);
  LaunchResult sr = launcher_press(&l, UI_LEFT);        // left/right now belong to the list
  ASSERT_EQ(sr.action, LAUNCH_SETTINGS);
  ASSERT_EQ(sr.button, UI_LEFT);
  ASSERT(l.settings_tab);
  launcher_press(&l, UI_BACK);                          // back to the tab row
  ASSERT(!l.settings_list);
  launcher_press(&l, UI_RIGHT);                         // wraps to Ages
  ASSERT(!l.settings_tab);
  ASSERT_EQ(l.game, UI_GAME_AGES);
  ASSERT_EQ(launcher_press(&l, UI_ACCEPT).action, LAUNCH_ADD_ROM);
  ASSERT_EQ(launcher_press(&l, UI_BACK).action, LAUNCH_QUIT);
  static UiCanvas c;
  UiFont font;
  ui_font_load(&font, rom, sizeof rom);
  launcher_draw(&l, &font, &c);                         // draws without the game art too
  launcher_press(&l, UI_LEFT);
  ASSERT(l.settings_tab);
  launcher_draw(&l, &font, &c);
}

static void launcher_offers_resume_and_slots(void) {
  Launcher l;
  memset(&l, 0, sizeof l);
  l.games[UI_GAME_AGES].installed = true;
  l.games[UI_GAME_AGES].has_resume = true;
  l.games[UI_GAME_AGES].has_slots = true;
  launcher_init(&l, UI_GAME_AGES);
  ASSERT_EQ(l.row, ROW_RESUME);
  ASSERT_EQ(launcher_press(&l, UI_ACCEPT).file, LAUNCH_RESUME);
  launcher_press(&l, UI_DOWN);
  ASSERT_EQ(launcher_press(&l, UI_ACCEPT).file, LAUNCH_SLOTS);
  launcher_press(&l, UI_DOWN);
  ASSERT_EQ(l.row, ROW_TITLE);
  l.games[UI_GAME_AGES].has_resume = l.games[UI_GAME_AGES].has_slots = false;
  launcher_press(&l, UI_UP);
  ASSERT_EQ(l.row, ROW_FILE3);                          // hidden rows are skipped
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
  static uint8_t rgb[UI_W * UI_H * 3];
  static UiCanvas c;
  UiFont font;
  UiTheme t;
  ui_font_load(&font, rom, sizeof rom);
  ui_theme_default(&t, true);
  slots_draw(&m, &font, &t, rgb, &c);
  slots_draw(&m, &font, &t, NULL, &c);
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
  s.volume = 3; s.filter = FILTER_CRT; s.gbc_colours = true; s.fullscreen = true; s.fill = true;
  s.widescreen = true; s.dim_sides = false;
  char buf[1024];
  settings_format(&s, buf, sizeof buf);
  settings_default(&back);
  settings_parse(&back, buf);
  ASSERT_EQ(back.volume, 3);
  ASSERT_EQ(back.filter, FILTER_CRT);
  ASSERT(back.gbc_colours && back.fullscreen && back.fill && back.widescreen && !back.dim_sides);
  settings_parse(&back, "volume=99\nfilter=weird\nunknown=1\n  volume = 7\n");
  ASSERT_EQ(back.volume, 7);                            // 99 rejected, spaced line accepted
  ASSERT_EQ(back.filter, FILTER_CRT);
}

static void settings_menu_changes_values(void) {
  Settings s;
  SettingsMenu m;
  settings_default(&s);
  settings_menu_open(&m);
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
  ASSERT_EQ(m.sel, SET_SYNC);                           // the last row
  ASSERT_EQ(settings_press(&m, &s, UI_ACCEPT, &row), MENU_PICK);
  ASSERT_EQ(row, SET_SYNC);
  settings_press(&m, &s, UI_UP, &row);
  ASSERT_EQ(m.sel, SET_CONTROLS);
  settings_press(&m, &s, UI_UP, &row);
  settings_press(&m, &s, UI_ACCEPT, &row);
  ASSERT(s.four_slots);
  settings_press(&m, &s, UI_UP, &row);
  settings_press(&m, &s, UI_ACCEPT, &row);
  ASSERT(s.quick_swap);
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
  ASSERT_EQ(settings_press(&m, &s, UI_ACCEPT, &row), MENU_PICK);
  ASSERT_EQ(row, SET_CONTROLS);
  ASSERT_EQ(settings_press(&m, &s, UI_BACK, &row), MENU_BACK);
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

static void bindings_remap_and_persist(void) {
  Settings s, back;
  settings_default(&s);
  ASSERT_EQ(bindings_key_action(&s.bindings, 27), ACT_A);        // X
  ASSERT_EQ(bindings_key_action(&s.bindings, 229), ACT_SELECT);  // right shift, a second key
  ASSERT_EQ(bindings_pad_action(&s.bindings, 5), ACT_PAUSE);     // guide
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

static void toast_boxes_a_message_near_the_bottom(void) {
  UiFont font;
  ASSERT(ui_font_load(&font, rom, sizeof rom));
  UiTheme t;
  ui_theme_default(&t, true);
  static UiCanvas c;
  ui_clear(&c, UI_BLACK);
  ui_toast(&c, &font, &t, "STATE SAVED");
  int top, bottom;
  ASSERT(ui_text_ink(&font, "STATE SAVED", &top, &bottom));
  int w = 11 * UI_GLYPH_W + 10, h = bottom - top + 5, x = (UI_W - w) / 2, y = UI_H - h;
  ASSERT_EQ(c.px[y][x][0], t.border.r);
  ASSERT_EQ(c.px[y + 1][x + 1][0], t.panel.r);
  ASSERT_EQ(c.px[y - 1][x][0], 0);
  PauseMenu pm;
  pause_open(&pm, true, true);
  static UiCanvas pc;
  static uint8_t grey[UI_W * UI_H * 3];
  pause_draw(&pm, &font, &t, grey, &pc);
  int box_bottom = 0;                                  // the pause box stays visible above it
  for (int yy = 0; yy < UI_H; yy++) if (!memcmp(pc.px[yy][UI_W / 2], &t.border, 3)) box_bottom = yy;
  ASSERT(y > box_bottom);
  ui_toast(&c, &font, &t, "A VERY LONG MESSAGE THAT DOES NOT FIT");   // clipped to the screen
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
    ASSERT_EQ(shown, a != ACT_SWAP);
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

static void touch_draw_lights_held_buttons(void) {
  TouchLayout l;
  touch_layout(&l, 2400, 1080, (UiRect){0, 0, 2400, 1080}, true, false, false, UI_W);
  UiTheme t;
  ui_theme_default(&t, true);
  uint8_t *img = malloc((size_t)l.w * l.h * 4);
  const UiRect *a = &l.button[ACT_A];
  int px = a->x + 3, py = a->y + a->h / 2;             // inside the ring, clear of the label
  touch_draw(&l, NULL, &t, 0, img);
  uint8_t *p = img + ((size_t)py * l.w + px) * 4;
  ASSERT_EQ(p[0], t.panel.r);
  ASSERT(p[3] > 0 && p[3] < 255);                      // landscape: translucent over the sides
  ASSERT_EQ(img[((size_t)(l.game.y + 72) * l.w + l.game.x + 80) * 4 + 3], 0);
  touch_draw(&l, NULL, &t, 1u << ACT_A, img);
  ASSERT_EQ(p[0], t.highlight.r);
  ASSERT_EQ(p[1], t.highlight.g);
  free(img);
}

static void touch_labels_centre_on_their_ink(void) {
  static UiFont font;
  memset(font.glyph, 0xff, sizeof font.glyph);
  for (const char *p = "START"; *p; p++)
    for (int r = 7; r <= 14; r++) font.glyph[(uint8_t)*p][r] = 0x81;   // ink rows 7-14, low in the cell
  font.loaded = true;
  TouchLayout l;
  touch_layout(&l, 1080, 2400, (UiRect){0, 0, 1080, 2400}, true, false, false, UI_W);
  UiTheme t;
  ui_theme_default(&t, false);
  uint8_t *img = malloc((size_t)l.w * l.h * 4);
  touch_draw(&l, &font, &t, 0, img);
  const UiRect *b = &l.button[ACT_START];
  int first = -1, last = -1, x = b->x + b->w / 2 - 20 + 1;           // a pixel column of the first letter
  for (int y = b->y + 1; y < b->y + b->h - 1; y++) {
    uint8_t *p = img + ((size_t)y * l.w + x) * 4;
    if (p[0] == t.text.r && p[1] == t.text.g && p[2] == t.text.b) { if (first < 0) first = y; last = y; }
  }
  ASSERT_EQ(last - first, 7);
  int above = first - b->y, below = b->y + b->h - 1 - last;
  ASSERT(above - below <= 1 && below - above <= 1);
  free(img);
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
  syncmenu_open(&m, true, "4827-1930-5561-0284", "SYNCED 17:32");
  ASSERT_EQ(m.sel, SYNCROW_NOW);
  syncmenu_press(&m, UI_DOWN, &row);
  ASSERT_EQ(m.sel, SYNCROW_ENTER);                      // on: no create
  syncmenu_press(&m, UI_DOWN, &row);
  ASSERT_EQ(syncmenu_press(&m, UI_ACCEPT, &row), MENU_PICK);
  ASSERT_EQ(row, SYNCROW_OFF);
  ASSERT_EQ(syncmenu_press(&m, UI_BACK, &row), MENU_BACK);
  UiFont font;
  ui_font_load(&font, rom, sizeof rom);
  UiTheme t;
  ui_theme_default(&t, false);
  static UiCanvas c;
  syncmenu_draw(&m, &font, &t, NULL, &c);
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
  snprintf(v.what, sizeof v.what, "SEASONS SAVE");
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
  UiFont font;
  ui_font_load(&font, rom, sizeof rom);
  UiTheme t;
  ui_theme_default(&t, true);
  static UiCanvas c;
  conflict_draw(&v, &font, &t, &c);
  ASSERT_EQ(c.px[34][0][0], t.highlight.r);            // the chosen side's box is lit
  ASSERT_EQ(c.px[34][80][0], t.dim.r);
}

// The launcher draws its ADD ROM screen before a ROM exists, so the built-in font has to stand in
// for the one in the ROM.
static void builtin_font_covers_the_launcher_text(void) {
  UiFont font;
  memset(&font, 0, sizeof font);
  ui_font_builtin(&font);
  ASSERT(font.loaded);
  int top, bottom;
  // Everything the no-ROM launcher puts on screen has to be drawable.
  ASSERT(ui_text_ink(&font, "ADD ROM...", &top, &bottom));
  ASSERT(ui_text_ink(&font, "NOT INSTALLED", &top, &bottom));
  ASSERT(ui_text_ink(&font, "AGES", &top, &bottom));
  ASSERT(ui_text_ink(&font, "SEASONS", &top, &bottom));
  ASSERT(ui_text_ink(&font, "A:PLAY  B:QUIT", &top, &bottom));
  ASSERT(ui_text_ink(&font, "<>:CHANGE B:BACK", &top, &bottom));
  // A space is blank, and every printable ASCII glyph but space carries ink.
  ASSERT(!ui_text_ink(&font, " ", &top, &bottom));
  for (char ch = '!'; ch <= '~'; ch++) {
    const char one[2] = {ch, 0};
    ASSERT(ui_text_ink(&font, one, &top, &bottom));
  }
  // The two non-ASCII glyphs the file rows use.
  const char heart[2] = {UI_CH_HEART, 0}, cursor[2] = {UI_CH_CURSOR, 0};
  ASSERT(ui_text_ink(&font, heart, &top, &bottom));
  ASSERT(ui_text_ink(&font, cursor, &top, &bottom));
}

static void builtin_font_draws_at_the_normal_pitch(void) {
  UiFont font;
  ui_font_builtin(&font);
  static UiCanvas c;
  ui_clear(&c, UI_BLACK);
  ASSERT_EQ(ui_text(&c, &font, 0, 0, "AB", UI_WHITE), 16);
  // 'A' has its apex at column 2 of the first drawn row, 'B' a stem at column 0 of the same row.
  ASSERT_EQ(c.px[4][2][0], UI_WHITE.r);
  ASSERT_EQ(c.px[4][8][0], UI_WHITE.r);
  // Glyphs stay inside their 8-pixel cell.
  ui_clear(&c, UI_BLACK);
  ui_text(&c, &font, 0, 0, "W", UI_WHITE);
  for (int y = 0; y < UI_H; y++) ASSERT_EQ(c.px[y][7][0], UI_BLACK.r);
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

int main(void) {
  RUN(font_loads_from_the_rom_offset);
  RUN(builtin_font_covers_the_launcher_text);
  RUN(builtin_font_draws_at_the_normal_pitch);
  RUN(text_draws_glyph_pixels);
  RUN(save_files_are_read_and_verified);
  RUN(launcher_navigates_and_starts);
  RUN(launcher_offers_resume_and_slots);
  RUN(pause_menu_skips_disabled_items);
  RUN(slots_load_only_used_slots);
  RUN(themes_come_from_the_title_palettes);
  RUN(settings_round_trip);
  RUN(settings_menu_changes_values);
  RUN(filters_keep_pixel_alignment);
  RUN(upscalers_fill_the_whole_picture);
  RUN(upscalers_smooth_a_diagonal);
  RUN(upscalers_keep_whole_pixels_past_their_factor);
  RUN(upscalers_take_the_wide_picture);
  RUN(upscalers_apply_gbc_colours_first);
  RUN(the_two_scalers_differ);
  RUN(upscale_factor_is_capped_at_the_window_scale);
  RUN(bindings_remap_and_persist);
  RUN(touch_layout_fits_screens);
  RUN(toast_boxes_a_message_near_the_bottom);
  RUN(sync_page_shows_the_rows_for_its_state);
  RUN(code_entry_edits_each_digit);
  RUN(conflict_picks_a_side);
  RUN(touch_fill_scales_past_whole_pixels);
  RUN(touch_layout_takes_the_wide_picture);
  RUN(touch_hits_each_control);
  RUN(touch_draw_lights_held_buttons);
  RUN(touch_labels_centre_on_their_ink);
  return 0;
}
