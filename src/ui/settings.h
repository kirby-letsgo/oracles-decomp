#pragma once
// Player settings (kept in settings.ini in the app folder) and the settings screen.
#include "ui/ui.h"
#include "ui/launcher.h"
#include "ui/menu.h"
#include "ui/controls.h"

typedef enum { FILTER_SHARP, FILTER_SCANLINES, FILTER_LCD, FILTER_CRT, FILTERS } ScreenFilter;

typedef struct {
  int volume;                   // 0..10
  bool fullscreen;
  ScreenFilter filter;
  bool gbc_colours;             // the Game Boy Color screen's paler, warmer colours
  Bindings bindings;
} Settings;

typedef enum { SET_VOLUME, SET_SCREEN, SET_COLOURS, SET_FULLSCREEN, SET_CONTROLS, SET_ROWS } SettingsRow;

typedef struct {
  SettingsRow sel;
} SettingsMenu;

void settings_default(Settings *s);
// Unknown keys and bad values are ignored, so an old or hand-edited file still loads.
void settings_parse(Settings *s, const char *text);
// Writes "key=value" lines; returns the number of bytes (as snprintf).
int settings_format(const Settings *s, char *out, size_t size);

void settings_menu_open(SettingsMenu *m);
// Left/right change the highlighted value in s; accept on CONTROLS returns MENU_PICK with that row.
MenuAction settings_press(SettingsMenu *m, Settings *s, UiButton b, SettingsRow *picked);
void settings_draw(const SettingsMenu *m, const Settings *s, const UiFont *font, const UiTheme *t, const uint8_t *game_rgb, UiCanvas *c);
