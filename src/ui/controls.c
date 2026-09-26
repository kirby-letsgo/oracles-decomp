#include "ui/controls.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// SDL scancodes: UP 82, DOWN 81, LEFT 80, RIGHT 79, X 27, Z 29, RETURN 40, ESCAPE 41, BACKSPACE 42,
// TAB 43, RSHIFT 229, C 6, A 4, S 22. SDL gamepad buttons: SOUTH 0, EAST 1, WEST 2, NORTH 3, BACK 4,
// GUIDE 5, START 6, LEFT_SHOULDER 9, DPAD 11-14.
static const int default_keys[ACTIONS][KEYS_PER_ACTION] = {
  {82, NO_BINDING}, {81, NO_BINDING}, {80, NO_BINDING}, {79, NO_BINDING}, {27, NO_BINDING},
  {29, NO_BINDING}, {40, NO_BINDING}, {42, 229}, {41, NO_BINDING}, {43, NO_BINDING}, {6, NO_BINDING},
  {4, NO_BINDING}, {22, NO_BINDING},
};
static const int default_pads[ACTIONS] = {11, 12, 13, 14, 0, 2, 6, 4, 5, PAD_RIGHT_TRIGGER, 9, 3, 1};

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
      if (end == p || v < NO_BINDING || v > PAD_RIGHT_TRIGGER) return;
      t.pad[a] = (int)v;
      p = end + (*end == ';');
    } else return;
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
  if (!m->waiting || m->pad_page) return;
  bindings_bind_key(b, (Action)m->sel, key);
  m->waiting = false;
}

void controls_capture_pad(ControlsMenu *m, Bindings *b, int button) {
  if (!m->waiting || !m->pad_page) return;
  bindings_bind_pad(b, (Action)m->sel, button);
  m->waiting = false;
}

#define VISIBLE 6

void controls_draw(const ControlsMenu *m, const Bindings *b, const UiFont *font, const UiTheme *t, const uint8_t *game_rgb,
                   const char *(*key_name)(int), const char *(*pad_name)(int), UiCanvas *c) {
  static const char *const labels[CTRL_ROWS] = {"UP", "DOWN", "LEFT", "RIGHT", "A", "B", "START", "SELECT", "PAUSE", "FAST", "SWAP", "ITEM X", "ITEM Y", "RESET"};
  if (game_rgb) ui_dim_rgb(c, game_rgb);
  else ui_clear(c, t->bg);
  ui_box(c, 0, 0, UI_W, UI_H, t->border, t->panel);
  ui_text(c, font, 4, 0, m->pad_page ? "GAMEPAD" : "KEYBOARD", t->highlight);
  ui_text(c, font, UI_W - 4 - ui_text_width("<>"), 0, "<>", t->dim);
  int first = m->sel - VISIBLE / 2;
  if (first > CTRL_ROWS - VISIBLE) first = CTRL_ROWS - VISIBLE;
  if (first < 0) first = 0;
  for (int i = first; i < first + VISIBLE && i < CTRL_ROWS; i++) {
    int y = 18 + (i - first) * 17;
    bool on = m->sel == i;
    UiColor fg = on ? t->highlight : t->text;
    if (on) ui_glyph(c, font, 2, y, '>', t->highlight);
    ui_text(c, font, 10, y, labels[i], fg);
    if (i == CTRL_RESET) continue;
    char v[24];
    if (on && m->waiting) snprintf(v, sizeof v, "PRESS...");
    else if (m->pad_page) snprintf(v, sizeof v, "%s", b->pad[i] == NO_BINDING ? "-" : pad_name(b->pad[i]));
    else snprintf(v, sizeof v, "%s", b->key[i][0] == NO_BINDING ? "-" : key_name(b->key[i][0]));
    v[10] = 0;
    ui_text(c, font, UI_W - 4 - ui_text_width(v), y, v, fg);
  }
  ui_text(c, font, 4, 124, m->waiting ? "PRESS A KEY" : "A:SET B:BACK", t->dim);
}
