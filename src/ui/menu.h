#pragma once
// In-game menus: the pause menu over the dimmed game, and the save-state slot picker (also used by
// the launcher's "load state").
#include "ui/ui.h"
#include "ui/launcher.h"

typedef enum { MENU_NONE, MENU_PICK, MENU_BACK } MenuAction;

typedef enum { PAUSE_RESUME, PAUSE_SAVE, PAUSE_LOAD, PAUSE_SETTINGS, PAUSE_QUIT, PAUSE_ITEMS } PauseItem;
typedef struct {
  PauseItem sel;
  bool can_save;                // false when no frame with parked threads was reached
  bool can_load;                // some slot holds a state
} PauseMenu;

void pause_open(PauseMenu *m, bool can_save, bool can_load);
MenuAction pause_press(PauseMenu *m, UiButton b, PauseItem *picked);
void pause_draw(const PauseMenu *m, const UiFont *font, const UiTheme *t, const uint8_t *game_rgb, UiCanvas *c);

#define UI_SLOTS 4
typedef struct {
  bool used;
  char when[16];                // e.g. "SEP26 14:02"
  uint8_t thumb[UI_H][UI_W][3];
} UiSlot;

typedef struct {
  bool save;                    // saving (any slot) or loading (used slots only)
  int sel;
  UiSlot slots[UI_SLOTS];
} SlotsMenu;

void slots_open(SlotsMenu *m, bool save);
MenuAction slots_press(SlotsMenu *m, UiButton b, int *slot);
// game_rgb, when not NULL, is drawn dimmed behind the list (in game); NULL draws the theme background
void slots_draw(const SlotsMenu *m, const UiFont *font, const UiTheme *t, const uint8_t *game_rgb, UiCanvas *c);

// The game's 160x144 RGB24 frame, darkened, as a menu background.
void ui_dim_rgb(UiCanvas *c, const uint8_t *rgb);
// A one-line message in a box near the bottom of the screen, drawn over what the canvas holds.
void ui_toast(UiCanvas *c, const UiFont *font, const UiTheme *t, const char *msg);
