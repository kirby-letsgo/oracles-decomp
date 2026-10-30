#pragma once
// The menu screens, one file each in src/gui (built from src/gui/widgets.h). Each begins a gui
// frame at the scale that fits it and declares its elements; the caller may add the toast or the
// wait and then ends the frame (gui_end draws it). Keys and buttons still go through the src/ui
// state machines; these return what was tapped.
#include "gui/gui.h"
#include "ui/home.h"
#include "ui/menu.h"
#include "ui/settings.h"
#include "ui/controls.h"
#include "ui/syncui.h"
#include "ui/touch.h"
#include "mods/mods.h"

typedef struct {
  SDL_FRect game;               // the game picture on the screen, render pixels
  SDL_FRect safe;               // the part of the screen clear of notches and system bars
  float w, h;                   // the whole screen
  bool hints;                   // keyboard hints ("Enter select   Esc back"); off on touch screens
  const char *ok_key, *back_key; // what the hints call accept and back ("Enter"/"Esc", "A"/"B", "Cross"/"Circle"); NULL = keyboard names
} GuiFrame;

#define GUI_NONE (-1)
#define GUI_BACK (-2)
#define GUI_UPDATE (-3)

// The pause menu over the dimmed game, inside the game picture. Returns the PauseItem tapped.
int gui_pause(PauseMenu *m, const GuiTheme *t, const GuiFrame *f);
enum { ERROR_ISSUE, ERROR_COPY, ERROR_BACK, ERROR_ACTIONS };
// A stopped game's report actions. The page stays open after copying or opening GitHub.
int gui_error(int *sel, bool has_log, const GuiTheme *t, const GuiFrame *f);
// The slot list, full screen: the slot tapped, GUI_BACK, or GUI_NONE. stamp changes when the
// thumbnails do.
int gui_slots(SlotsMenu *m, const GuiTheme *t, const GuiFrame *f, uint32_t stamp);

// A tap in a settings list: the row (or GUI_NONE) and the button it amounts to (GUI_NONE when it
// only selects the row); a tap on a volume bar sets volume (1..10) instead.
typedef struct { int row; UiButton button; int volume; } GuiSettingsTap;

// The settings page from the pause menu. Returns true when Back was tapped.
bool gui_settings(SettingsMenu *m, const Settings *s, const char *sync_label, const GuiTheme *t, const GuiFrame *f, GuiSettingsTap *tap);
// The main menu (with the settings list on its settings tab). A tap moves the selection (or changes
// tab) and returns the button it amounts to (UI_ACCEPT, UI_BACK), GUI_UPDATE for the newer release's
// notice (s->update), or GUI_NONE; settings rows report in tap.
int gui_home(Home *h, const GuiTheme *t, const GuiFrame *f, SettingsMenu *sm, const Settings *s, const char *sync_label, GuiSettingsTap *tap);
// Key and pad bindings. A row tap returns the row; *page is set to the tab tapped (0 keyboard,
// 1 gamepad) or -1. GUI_BACK for Back.
int gui_controls(ControlsMenu *m, const Bindings *b, const char *(*key_name)(int), const char *(*pad_name)(int), const GuiTheme *t,
                 const GuiFrame *f, int *page);
// The save sync page: the row tapped, or GUI_BACK. *reveal toggles when the masked code is tapped.
int gui_sync(SyncMenu *m, const GuiTheme *t, const GuiFrame *f, bool *reveal);
#define GUI_CONNECT 100
// Entering a sync code: the digit tapped (0..15), GUI_CONNECT, or GUI_BACK.
int gui_code(CodeEntry *e, const GuiTheme *t, const GuiFrame *f);
// A save changed on two devices: the side tapped (0 here, 1 the other), or GUI_BACK (decide later).
int gui_conflict(ConflictView *v, const GuiTheme *t, const GuiFrame *f);
// The message near the bottom of the game picture; true when its Undo was tapped.
bool gui_toast(const char *msg, const char *action, const GuiTheme *t, const GuiFrame *f);
// Something the player has to wait for (a sync), over everything: a dimmed screen, the message,
// and three squares lit in turn by ticks (milliseconds). Taps and hover don't reach what is under it.
void gui_wait(const char *msg, uint64_t ticks, const GuiTheme *t, const GuiFrame *f);
// The touch controls where the layout puts them (src/gui/pad.c), drawn at once; held actions lit.
void gui_pad(const TouchLayout *l, uint32_t held, const GuiTheme *t);
// "Today 14:02:37", "Yesterday 09:15:12", "Tue 21:40:05", "Sep 26 21:40".
void gui_when(int64_t time, int64_t now, char *out, size_t size);

// Official mod catalogue; returns the tapped entry, Back, or None.
int gui_mods(const ModInfo *official, const bool *enabled, int selected, bool playing, const ModInstalled *installed, int installed_count, const char *status, const GuiTheme *t, const GuiFrame *f);
