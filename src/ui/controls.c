#include "ui/controls.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// SDL scancodes: UP 82, DOWN 81, LEFT 80, RIGHT 79, X 27, Z 29, RETURN 40, ESCAPE 41, BACKSPACE 42,
// TAB 43, RSHIFT 229, C 6, A 4, S 22. SDL gamepad buttons: SOUTH 0, EAST 1, WEST 2, NORTH 3, BACK 4,
// GUIDE 5, START 6, LEFT_STICK 7, RIGHT_STICK 8, LEFT_SHOULDER 9, DPAD 11-14. Save and load state have
// no keys of their own: Cmd/Ctrl+S and Cmd/Ctrl+R are theirs as well.
static const int default_keys[ACTIONS][KEYS_PER_ACTION] = {
  {82, NO_BINDING}, {81, NO_BINDING}, {80, NO_BINDING}, {79, NO_BINDING}, {27, NO_BINDING},
  {29, NO_BINDING}, {40, NO_BINDING}, {42, 229}, {41, NO_BINDING}, {43, NO_BINDING}, {6, NO_BINDING},
  {4, NO_BINDING}, {22, NO_BINDING}, {NO_BINDING, NO_BINDING}, {NO_BINDING, NO_BINDING}, {20, NO_BINDING}, {26, NO_BINDING},
};
static const int default_pads[ACTIONS] = {11, 12, 13, 14, 0, 2, 6, 4, 5, PAD_RIGHT_TRIGGER, 9, 3, 1, 7, 8, PAD_LEFT_TRIGGER, 10};

int pad_axis_button(int value, int negative, int positive, int held) {
  if (value < -16000) return negative;
  if (value > 16000) return positive;
  if (value < -12000 && held == negative) return negative;
  if (value > 12000 && held == positive) return positive;
  return NO_BINDING;
}

void bindings_default(Bindings *b) {
  memcpy(b->key, default_keys, sizeof b->key);
  memcpy(b->pad, default_pads, sizeof b->pad);
}

Action bindings_key_action(const Bindings *b, int key) {
  for (int a = 0; a < ACTIONS; a++)
    for (int k = 0; k < KEYS_PER_ACTION; k++)
      if (b->key[a][k] == key && key != NO_BINDING) return (Action)a;
  return ACTIONS;
}

Action bindings_pad_action(const Bindings *b, int button) {
  for (int a = 0; a < ACTIONS; a++)
    if (b->pad[a] == button && button != NO_BINDING) return (Action)a;
  return ACTIONS;
}

void bindings_bind_key(Bindings *b, Action a, int key) {
  for (int o = 0; o < ACTIONS; o++)
    for (int k = 0; k < KEYS_PER_ACTION; k++)
      if (b->key[o][k] == key) b->key[o][k] = NO_BINDING;
  for (int o = 0; o < ACTIONS; o++)                     // keep each action's keys packed first
    if (b->key[o][0] == NO_BINDING) { b->key[o][0] = b->key[o][1]; b->key[o][1] = NO_BINDING; }
  b->key[a][1] = b->key[a][0];
  b->key[a][0] = key;
}

void bindings_bind_pad(Bindings *b, Action a, int button) {
  for (int o = 0; o < ACTIONS; o++)
    if (b->pad[o] == button) b->pad[o] = NO_BINDING;
  b->pad[a] = button;
}

int bindings_format(const Bindings *b, char *out, size_t size) {
  int n = snprintf(out, size, "keys=");
  for (int a = 0; a < ACTIONS && n < (int)size; a++)
    n += snprintf(out + n, size - n, "%s%d,%d", a ? ";" : "", b->key[a][0], b->key[a][1]);
  if (n < (int)size) n += snprintf(out + n, size - n, "\npads=");
  for (int a = 0; a < ACTIONS && n < (int)size; a++)
    n += snprintf(out + n, size - n, "%s%d", a ? ";" : "", b->pad[a]);
  if (n < (int)size) n += snprintf(out + n, size - n, "\n");
  return n;
}

// Parsed into a copy first, so a malformed line changes nothing. A line that simply ends early (a
// file from before an action existed) keeps the remaining actions' current bindings.
void bindings_parse_line(Bindings *b, const char *key, const char *value) {
  Bindings t = *b;
  const char *p = value;
  for (int a = 0; a < ACTIONS && *p; a++) {
    char *end;
    if (!strcmp(key, "keys")) {
      for (int k = 0; k < KEYS_PER_ACTION; k++) {
        long v = strtol(p, &end, 10);
        if (end == p || v < NO_BINDING || v > 511) return;
        t.key[a][k] = (int)v;
        p = end + (*end == ',' || *end == ';');
      }
    } else if (!strcmp(key, "pads")) {
      long v = strtol(p, &end, 10);
      if (end == p || v < NO_BINDING || v > PAD_ANDROID_BACK) return;
      t.pad[a] = (int)v;
      p = end + (*end == ';');
    } else return;
  }
  // Old profiles can already use the default shoulder or key for another action.
  for (int i = ACT_ITEM_L; i <= ACT_ITEM_R; i++)
    for (int j = 0; j < i; j++) {
      if (!strcmp(key, "pads") && t.pad[i] == t.pad[j]) t.pad[i] = NO_BINDING;
      if (!strcmp(key, "keys"))
        for (int k = 0; k < KEYS_PER_ACTION; k++)
          if (t.key[i][k] == t.key[j][0] || t.key[i][k] == t.key[j][1]) t.key[i][k] = NO_BINDING;
    }
  *b = t;
}

void controls_open(ControlsMenu *m) { m->sel = 0; m->pad_page = false; m->waiting = false; }

MenuAction controls_press(ControlsMenu *m, Bindings *b, UiButton button) {
  if (m->waiting) return MENU_NONE;
  switch (button) {
  case UI_UP: m->sel = (m->sel + CTRL_ROWS - 1) % CTRL_ROWS; break;
  case UI_DOWN: m->sel = (m->sel + 1) % CTRL_ROWS; break;
  case UI_LEFT: case UI_RIGHT: m->pad_page = !m->pad_page; break;
  case UI_BACK: return MENU_BACK;
  case UI_ACCEPT:
    if (m->sel == CTRL_RESET) bindings_default(b);
    else m->waiting = true;
    break;
  }
  return MENU_NONE;
}

void controls_capture_key(ControlsMenu *m, Bindings *b, int key) {
  if (!m->waiting) return;
  bindings_bind_key(b, (Action)m->sel, key);
  m->pad_page = false;
  m->waiting = false;
}

void controls_capture_pad(ControlsMenu *m, Bindings *b, int button) {
  if (!m->waiting) return;
  bindings_bind_pad(b, (Action)m->sel, button);
  m->pad_page = true;
  m->waiting = false;
}

uint8_t tap_press(TapLatch *t, uint8_t held, uint8_t bits) {
  t->unseen |= bits;
  t->late &= (uint8_t)~bits;
  return held | bits;
}

uint8_t tap_release(TapLatch *t, uint8_t held, uint8_t bits) {
  t->late |= bits & t->unseen;
  return held & (uint8_t)~(bits & ~t->unseen);
}

uint8_t tap_frame_ran(TapLatch *t, uint8_t held) {
  held &= (uint8_t)~t->late;
  t->unseen = t->late = 0;
  return held;
}
