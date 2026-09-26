#include "unit.h"
#include "ui/ui.h"
#include "ui/savefile.h"
#include "ui/launcher.h"
#include "ui/menu.h"

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
  ASSERT_EQ(m.sel, PAUSE_QUIT);                         // save and load are unavailable
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

int main(void) {
  RUN(font_loads_from_the_rom_offset);
  RUN(text_draws_glyph_pixels);
  RUN(save_files_are_read_and_verified);
  RUN(launcher_navigates_and_starts);
  RUN(launcher_offers_resume_and_slots);
  RUN(pause_menu_skips_disabled_items);
  RUN(slots_load_only_used_slots);
  RUN(themes_come_from_the_title_palettes);
  return 0;
}
