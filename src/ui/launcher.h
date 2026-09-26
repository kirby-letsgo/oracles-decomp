#pragma once
// The launcher: Ages and Seasons side by side, the chosen game's three save files, Resume, its save states,
// and a way to add a game's ROM. Pure state machine plus drawing; the app feeds it buttons and
// shows the canvas.
#include "ui/ui.h"
#include "ui/savefile.h"

typedef enum { UI_UP, UI_DOWN, UI_LEFT, UI_RIGHT, UI_ACCEPT, UI_BACK } UiButton;
typedef enum { UI_GAME_AGES, UI_GAME_SEASONS, UI_GAMES } UiGame;
typedef enum { LAUNCH_NONE, LAUNCH_PLAY, LAUNCH_ADD_ROM, LAUNCH_SETTINGS, LAUNCH_QUIT } LaunchAction;
typedef enum { ROW_FILE1, ROW_FILE2, ROW_FILE3, ROW_RESUME, ROW_SLOTS, ROW_TITLE, ROW_SETTINGS } LauncherRow;

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
  UiGame game;                  // highlighted game
  LauncherRow row;
} Launcher;

typedef struct {
  LaunchAction action;
  UiGame game;
  int file;                     // 0..2, LAUNCH_TITLE, LAUNCH_RESUME or LAUNCH_SLOTS
} LaunchResult;

void launcher_init(Launcher *l, UiGame initial);
LaunchResult launcher_press(Launcher *l, UiButton b);
void launcher_draw(const Launcher *l, const UiFont *font, UiCanvas *c);
