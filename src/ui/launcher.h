#pragma once
// The launcher: tabs for Ages, Seasons and the settings (shared by both games); a game's tab shows
// its three save files, Resume, its save states and a way to add its ROM. Pure state machine plus
// drawing; the app feeds it buttons and shows the canvas, and runs the settings list itself.
#include "ui/ui.h"
#include "ui/savefile.h"

typedef enum { UI_UP, UI_DOWN, UI_LEFT, UI_RIGHT, UI_ACCEPT, UI_BACK } UiButton;
typedef enum { UI_GAME_AGES, UI_GAME_SEASONS, UI_GAMES } UiGame;
// LAUNCH_SETTINGS: the button is for the settings list (the settings tab has the focus in it)
typedef enum { LAUNCH_NONE, LAUNCH_PLAY, LAUNCH_ADD_ROM, LAUNCH_SETTINGS, LAUNCH_QUIT } LaunchAction;
typedef enum { ROW_FILE1, ROW_FILE2, ROW_FILE3, ROW_RESUME, ROW_SLOTS, ROW_TITLE } LauncherRow;

#define LAUNCH_TITLE (-1)
#define LAUNCH_RESUME (-2)      // the state written when the game was last quit
#define LAUNCH_SLOTS (-3)       // pick one of the save-state slots

typedef struct {
  bool installed;               // the game's assets are in the cache
  bool has_resume;              // an auto state from the last quit exists
  bool has_slots;               // some save-state slot is used
  UiFileInfo files[UI_FILES];
  UiTheme theme;
} LauncherGame;

typedef struct {
  LauncherGame games[UI_GAMES];
  UiGame game;                  // highlighted game (on the settings tab: the one whose colours it uses)
  LauncherRow row;
  bool settings_tab;
  bool settings_list;           // on the settings tab, the list has the focus (else the tab row)
} Launcher;

typedef struct {
  LaunchAction action;
  UiGame game;
  int file;                     // 0..2, LAUNCH_TITLE, LAUNCH_RESUME or LAUNCH_SLOTS
  UiButton button;              // with LAUNCH_SETTINGS
} LaunchResult;

void launcher_init(Launcher *l, UiGame initial);
LaunchResult launcher_press(Launcher *l, UiButton b);
void launcher_draw(const Launcher *l, const UiFont *font, UiCanvas *c);
// The launcher's body box, where the app draws the settings list on the settings tab.
enum { LAUNCHER_BODY_Y = 28, LAUNCHER_BODY_H = 92 };
