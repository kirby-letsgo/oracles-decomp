#pragma once
// Button bindings (keyboard: two keys per action, gamepad: one button) and the remap screen's state.
// Key codes are SDL scancodes (USB HID usage ids) and pad codes SDL gamepad buttons; the library
// only stores and compares them, the app names them.
#include "ui/ui.h"
#include "ui/launcher.h"
#include "ui/menu.h"

typedef enum { ACT_UP, ACT_DOWN, ACT_LEFT, ACT_RIGHT, ACT_A, ACT_B, ACT_START, ACT_SELECT, ACT_PAUSE, ACT_FAST, ACT_SWAP, ACT_ITEM_X, ACT_ITEM_Y, ACT_SAVE_STATE, ACT_LOAD_STATE, ACTIONS } Action;
#define KEYS_PER_ACTION 2
#define NO_BINDING (-1)
#define PAD_RIGHT_TRIGGER 100   // the triggers, axes, stand in as buttons
#define PAD_LEFT_TRIGGER 101
enum { PAD_LS_LEFT = 102, PAD_LS_RIGHT, PAD_LS_UP, PAD_LS_DOWN,
       PAD_RS_LEFT, PAD_RS_RIGHT, PAD_RS_UP, PAD_RS_DOWN };

// Virtual buttons on one axis. Release closer to centre so small fluctuations
// around the press threshold don't repeatedly fire actions such as Load state.
int pad_axis_button(int value, int negative, int positive, int held);

typedef struct {
  int key[ACTIONS][KEYS_PER_ACTION];
  int pad[ACTIONS];
} Bindings;

void bindings_default(Bindings *b);
Action bindings_key_action(const Bindings *b, int key);   // ACTIONS when unbound
Action bindings_pad_action(const Bindings *b, int button);
// Makes key the action's first key (its old first key becomes the second); taken from any other action.
void bindings_bind_key(Bindings *b, Action a, int key);
void bindings_bind_pad(Bindings *b, Action a, int button);
// "keys=82,-1;81,-1;..." and "pads=11;12;..." lines for settings.ini
int bindings_format(const Bindings *b, char *out, size_t size);
void bindings_parse_line(Bindings *b, const char *key, const char *value);

typedef enum { CTRL_RESET = ACTIONS, CTRL_ROWS } ControlsExtraRow;
typedef struct {
  int sel;                      // an Action, or CTRL_RESET
  bool pad_page;                // gamepad bindings, else keyboard
  bool waiting;                 // the next key or button gets bound to sel
} ControlsMenu;

void controls_open(ControlsMenu *m);
MenuAction controls_press(ControlsMenu *m, Bindings *b, UiButton button);
void controls_capture_key(ControlsMenu *m, Bindings *b, int key);
void controls_capture_pad(ControlsMenu *m, Bindings *b, int button);

// Input arrives between frames, so a tap pressed and released before the next frame would never
// reach the game. A release of a button no frame has seen yet waits for tap_frame_ran.
// Each call takes the held Game Boy buttons and returns them updated.
typedef struct {
  uint8_t unseen;               // pressed since the last frame ran
  uint8_t late;                 // released before any frame saw the press
} TapLatch;

uint8_t tap_press(TapLatch *t, uint8_t held, uint8_t bits);
uint8_t tap_release(TapLatch *t, uint8_t held, uint8_t bits);
uint8_t tap_frame_ran(TapLatch *t, uint8_t held);
