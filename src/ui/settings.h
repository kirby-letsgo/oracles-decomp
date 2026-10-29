#pragma once
// Player settings (kept in settings.ini in the app folder) and the settings list's state machine.
#include "ui/ui.h"
#include "ui/launcher.h"
#include "ui/menu.h"
#include "ui/controls.h"
#include "ui/labels.h"

// XBRZ and HQX are the two pixel-art scalers: they round off the jagged diagonals instead of
// enlarging each pixel into a square. See src/ui/upscale.h.
typedef enum { FILTER_SHARP, FILTER_SCANLINES, FILTER_LCD, FILTER_CRT,
               FILTER_XBRZ, FILTER_HQX, FILTERS } ScreenFilter;

typedef struct {
  int volume;                   // 0..10
  bool fullscreen;
  ScreenFilter filter;
  bool fill;                    // the game fills the width (or height) at a fractional scale
  bool widescreen;              // 256 wide: the neighbouring rooms beside the 160 of the game
  bool dim_sides;               // widescreen's side strips drawn darker than the live room
  bool gbc_colours;             // the Game Boy Color screen's paler, warmer colours
  bool fast_text;               // each line of a text box appears at once
  bool quick_swap;              // the Swap button exchanges the A and B items
  bool fast_menus;              // quick fades into and out of the inventory and map
  int ff_speed;                 // fast-forward runs the game this many times as fast (FF_SPEED_MIN..FF_SPEED_MAX)
  bool four_slots;              // Item X / Item Y buttons, assigned in the inventory
  bool essence_tracker;         // bundled official mod; applied when a game starts
  bool haptics;                 // a tick when a finger presses a control or taps a menu (Android)
  PadStyle pad_style;           // the names on screen for a controller: Auto follows the pad's type
  Bindings bindings;
  char sync_label[12];          // the SYNC row's value, kept by the app (sync.ini, not settings.ini)
  bool portable;                // portable.txt beside the app, kept by the app (not settings.ini)
  char version[16];             // this build's release, "dev" for a local build; kept by the app
  bool error_log;              // a saved error report is available; kept by the app
  char update[16];              // a newer release to download, or empty; kept by the app
} Settings;

enum { FF_SPEED_MIN = 2, FF_SPEED_MAX = 8, FF_SPEEDS = FF_SPEED_MAX - FF_SPEED_MIN + 1 };

typedef enum { SET_VOLUME, SET_SCREEN, SET_SCALE, SET_WIDESCREEN, SET_DIM_SIDES, SET_COLOURS, SET_FULLSCREEN, SET_FAST_TEXT, SET_FAST_MENUS, SET_FF_SPEED, SET_QUICK_SWAP, SET_FOUR_SLOTS, SET_HAPTICS, SET_CONTROLS, SET_PAD_STYLE, SET_SYNC, SET_PORTABLE, SET_VERSION, SET_ERROR_LOG, SET_MODS, SET_ROWS } SettingsRow;

typedef struct {
  SettingsRow sel;
  bool phone;                   // Android: Vibration is shown (nothing else vibrates); Fullscreen
                                // and Portable are not (the app always fills the screen, and has
                                // no folder of its own to keep data in)
} SettingsMenu;

void settings_default(Settings *s);
// Unknown keys and bad values are ignored, so an old or hand-edited file still loads.
void settings_parse(Settings *s, const char *text);
// Writes "key=value" lines; returns the number of bytes (as snprintf).
int settings_format(const Settings *s, char *out, size_t size);

void settings_menu_open(SettingsMenu *m, bool phone);
// The row is in the list (SET_HAPTICS only on a phone, SET_FULLSCREEN and SET_PORTABLE never); keys skip the others.
bool settings_row_shown(const SettingsMenu *m, SettingsRow r);
// Left/right change the highlighted value in s; accept on CONTROLS or SYNC, or on VERSION with an
// update to download, or ERROR_LOG with a saved report, returns MENU_PICK with that row.
MenuAction settings_press(SettingsMenu *m, Settings *s, UiButton b, SettingsRow *picked);
