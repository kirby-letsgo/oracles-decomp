// Exercise the app's actual save writer and date selection without running game logic.
#define SDL_MAIN_HANDLED
#define main app_main
#include "../src/platform/app.c"
#undef main
#include "unit.h"

static void make_save(uint8_t *file, bool ages, int health) {
  memset(file, 0, UI_FILE_SIZE);
  memcpy(file + 2, ages ? "Z21216-0" : "Z11216-0", 8);
  memcpy(file + 0x52, "LINK", 4);
  file[ages ? 0xfa : 0xf2] = health;
  file[ages ? 0xfb : 0xf3] = health;
  uint16_t sum = ui_file_checksum(file);
  file[0] = sum;
  file[1] = sum >> 8;
}

int main(int argc, char **argv) {
  ASSERT_EQ(argc, 2);
  SDL_SetMainReady();
  ASSERT(SDL_Init(0));
  char cache[1024];
  snprintf(cache, sizeof cache, "%s/", argv[1]);
  for (int variant = 0; variant < 4; variant++) {
    const char *game = known[variant].id;
    bool ages = is_ages(game);
    char dir[1100], sav[1300], meta[1400];
    snprintf(dir, sizeof dir, "%s%s", cache, game);
    ASSERT(SDL_CreateDirectory(dir));
    snprintf(sav, sizeof sav, "%s/sram.sav", dir);
    snprintf(meta, sizeof meta, "%s.dates", sav);
    uint8_t rom[0x151] = {0};
    memcpy(rom + 0x134, ages ? "ZELDA NAYRU" : "ZELDA DIN", ages ? 11 : 9);
    GB *gb = calloc(1, sizeof *gb);
    gb->rom = rom; gb->rom_size = sizeof rom; gb->eram_size = 0x2000;
    make_save(gb->eram + 0x10, ages, 12);
    make_save(gb->eram + 0x1550, ages, 16);  // Slot 2 has only a valid backup.
    save_sram(gb, sav);
    SaveDate dates[UI_FILES];
    save_dates(sav, gb->eram, gb->eram_size, ages, dates);
    ASSERT(dates[0].hash[0] && dates[1].hash[0] && !dates[2].hash[0]);
    ASSERT_EQ(dates[0].time, dates[1].time);
    GameStart gs = {cache, game, LAUNCH_TITLE, false, 0, 0};
    ASSERT(latest_start(&gs));
    ASSERT(!gs.open_file);  // Simultaneous/unknown saves need a choice.

    // Simulate known old per-slot dates, then check an idle flush does not change either file.
    char text[256];
    int len = snprintf(text, sizeof text, "0 %s 1\n1 %s 2\n", dates[0].hash, dates[1].hash);
    ASSERT(write_file(meta, text, len));
    SDL_PathInfo before, after;
    ASSERT(SDL_GetPathInfo(sav, &before));
    save_sram(gb, sav);
    ASSERT(SDL_GetPathInfo(sav, &after));
    ASSERT_EQ(before.modify_time, after.modify_time);
    ASSERT(latest_start(&gs)); ASSERT_EQ(gs.start_file, 1); ASSERT(gs.open_file);

    // Updating slot 1 must preserve slot 2's date even though SRAM itself is rewritten.
    make_save(gb->eram + 0x10, ages, 20);
    save_sram(gb, sav);
    save_dates(sav, gb->eram, gb->eram_size, ages, dates);
    ASSERT(dates[0].time > 2); ASSERT_EQ(dates[1].time, 2);
    ASSERT(latest_start(&gs)); ASSERT_EQ(gs.start_file, 0); ASSERT(gs.open_file);

    // Erasing slot 1 must not make the unchanged remaining slot newer.
    memset(gb->eram + 0x10, 0, UI_FILE_SIZE);
    save_sram(gb, sav);
    save_dates(sav, gb->eram, gb->eram_size, ages, dates);
    ASSERT(!dates[0].hash[0]); ASSERT_EQ(dates[1].time, 2);
    ASSERT(latest_start(&gs)); ASSERT_EQ(gs.start_file, 1); ASSERT(gs.open_file);

    // An imported/replaced slot cannot inherit its predecessor's timestamp.
    make_save(gb->eram + 0x1550, ages, 24);
    ASSERT(write_file(sav, gb->eram, gb->eram_size));
    save_dates(sav, gb->eram, gb->eram_size, ages, dates);
    ASSERT(dates[1].time > 2);

    // A state flushes SRAM first. A later idle flush must leave that state as the newest choice.
    make_save(gb->eram + 0x1550, ages, 28);
    static uint8_t rgb[FB_W * FB_H * 3];
    ASSERT(slot_save(gb, dir, SLOT_AUTO, rgb));
    save_sram(gb, sav);
    ASSERT(latest_start(&gs)); ASSERT_EQ(gs.start_file, LAUNCH_RESUME);
    free(gb);
  }
  SDL_Quit();
  puts("latest saves: per-slot dates, unchanged flush, backup, erase, import and state ordering passed");
  return 0;
}
