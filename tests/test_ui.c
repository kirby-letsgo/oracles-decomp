#include "unit.h"
#include "ui/ui.h"
#include "ui/savefile.h"
#include "ui/launcher.h"
#include "ui/menu.h"
#include "ui/settings.h"
#include "ui/filter.h"
#include "ui/touch.h"

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
  ASSERT_EQ(l.row, ROW_SETTINGS);
  ASSERT_EQ(launcher_press(&l, UI_ACCEPT).action, LAUNCH_SETTINGS);
  launcher_press(&l, UI_DOWN);
  ASSERT_EQ(l.row, 0);                                  // wraps
  launcher_press(&l, UI_LEFT);
  ASSERT_EQ(l.game, UI_GAME_AGES);
  ASSERT_EQ(launcher_press(&l, UI_ACCEPT).action, LAUNCH_ADD_ROM);
  ASSERT_EQ(launcher_press(&l, UI_BACK).action, LAUNCH_QUIT);
  static UiCanvas c;
  UiFont font;
  ui_font_load(&font, rom, sizeof rom);
  launcher_draw(&l, &font, &c);                         // draws without the game art too
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
  s.volume = 3; s.filter = FILTER_CRT; s.gbc_colours = true; s.fullscreen = true;
  char buf[256];
  settings_format(&s, buf, sizeof buf);
  settings_default(&back);
  settings_parse(&back, buf);
  ASSERT_EQ(back.volume, 3);
  ASSERT_EQ(back.filter, FILTER_CRT);
  ASSERT(back.gbc_colours && back.fullscreen);
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
  ASSERT_EQ(s.filter, FILTER_CRT);                      // wraps backwards
  settings_press(&m, &s, UI_UP, &row);
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
  ui_filter(src, false, FILTER_SHARP, 4, dst);
  ASSERT_EQ(dst[0], 200);
  ASSERT_EQ(dst[(3 * UI_W * 4 + 3) * 3], 200);
  ui_filter(src, false, FILTER_SCANLINES, 4, dst);
  ASSERT_EQ(dst[0], 200);
  ASSERT_EQ(dst[(3 * UI_W * 4) * 3], 100);              // the last row of each pixel is darker
  ui_filter(src, false, FILTER_LCD, 4, dst);
  ASSERT_EQ(dst[3 * 3], 150);                           // the last column too
  ui_filter(src, false, FILTER_CRT, 4, dst);
  ASSERT_EQ(dst[0], 0);                                 // the curved screen leaves the corner black
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
  controls_capture_pad(&m, &s.bindings, 3);                      // keyboard page: ignored
  controls_capture_key(&m, &s.bindings, 22);                     // S
  ASSERT(!m.waiting);
  ASSERT_EQ(bindings_key_action(&s.bindings, 22), ACT_DOWN);
  ASSERT_EQ(s.bindings.key[ACT_DOWN][1], 81);
  m.sel = CTRL_RESET;
  controls_press(&m, &s.bindings, UI_ACCEPT);
  ASSERT_EQ(bindings_key_action(&s.bindings, 27), ACT_A);
}

static bool overlaps(UiRect a, UiRect b) { return a.x < b.x + b.w && b.x < a.x + a.w && a.y < b.y + b.h && b.y < a.y + a.h; }
static bool inside(UiRect a, int w, int h) { return a.x >= 0 && a.y >= 0 && a.x + a.w <= w && a.y + a.h <= h; }

static void touch_layout_fits_screens(void) {
  static const int sizes[][2] = {{1080, 2400}, {720, 1280}, {1440, 3200}, {1600, 2560}, {1080, 1920}, {2400, 1080}, {1280, 720}, {2560, 1600}, {1920, 1080}};
  for (size_t i = 0; i < sizeof sizes / sizeof sizes[0]; i++)
    for (int items = 0; items < 2; items++) {
      int w = sizes[i][0], h = sizes[i][1];
      TouchLayout l;
      touch_layout(&l, w, h, (UiRect){0, 90, w, h - 90 - 60}, true, items);
      ASSERT(l.scale >= 3);
      ASSERT(inside(l.game, w / l.scale, h / l.scale));
      ASSERT(l.game.y * l.scale >= 90);
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
  touch_layout(&l, 1080, 2400, (UiRect){0, 0, 1080, 2400}, true, false);
  ASSERT(l.portrait);
  ASSERT_EQ(l.scale, 6);
  ASSERT_EQ(l.game.x, 10);
  touch_layout(&l, 1080, 2400, (UiRect){0, 0, 1080, 2400}, false, false);
  ASSERT(!l.portrait);
  ASSERT_EQ(l.game.y, (400 - 144) / 2);                // no overlay: centred, nothing to press
  ASSERT_EQ(touch_hit(&l, 100, 300), 0);
}

static void touch_hits_each_control(void) {
  TouchLayout l;
  touch_layout(&l, 1080, 2400, (UiRect){0, 0, 1080, 2400}, true, true);
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
  touch_layout(&l, 1080, 2400, (UiRect){0, 0, 1080, 2400}, true, false);
  ASSERT(!touch_point(&l, ACT_ITEM_X, &x, &y));
}

static void touch_draw_lights_held_buttons(void) {
  TouchLayout l;
  touch_layout(&l, 2400, 1080, (UiRect){0, 0, 2400, 1080}, true, false);
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
  touch_layout(&l, 1080, 2400, (UiRect){0, 0, 1080, 2400}, true, false);
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

int main(void) {
  RUN(font_loads_from_the_rom_offset);
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
  RUN(bindings_remap_and_persist);
  RUN(touch_layout_fits_screens);
  RUN(touch_hits_each_control);
  RUN(touch_draw_lights_held_buttons);
  RUN(touch_labels_centre_on_their_ink);
  return 0;
}
