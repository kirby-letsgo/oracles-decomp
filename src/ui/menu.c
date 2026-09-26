#include "ui/menu.h"
#include <stdio.h>
#include <string.h>

void ui_dim_rgb(UiCanvas *c, const uint8_t *rgb) {
  for (int y = 0; y < UI_H; y++)
    for (int x = 0; x < UI_W; x++)
      for (int k = 0; k < 3; k++) c->px[y][x][k] = rgb[(y * UI_W + x) * 3 + k] / 3;
}

static const char *const pause_labels[PAUSE_ITEMS] = {"RESUME", "SAVE STATE", "LOAD STATE", "QUIT"};

static bool pause_enabled(const PauseMenu *m, PauseItem i) {
  return (i != PAUSE_SAVE || m->can_save) && (i != PAUSE_LOAD || m->can_load);
}

void pause_open(PauseMenu *m, bool can_save, bool can_load) {
  m->sel = PAUSE_RESUME;
  m->can_save = can_save;
  m->can_load = can_load;
}

MenuAction pause_press(PauseMenu *m, UiButton b, PauseItem *picked) {
  switch (b) {
  case UI_UP: case UI_DOWN: {
    int dir = b == UI_UP ? -1 : 1;
    do m->sel = (PauseItem)((m->sel + dir + PAUSE_ITEMS) % PAUSE_ITEMS);
    while (!pause_enabled(m, m->sel));
    return MENU_NONE;
  }
  case UI_ACCEPT: *picked = m->sel; return MENU_PICK;
  case UI_BACK: return MENU_BACK;
  default: return MENU_NONE;
  }
}

void pause_draw(const PauseMenu *m, const UiFont *font, const UiTheme *t, const uint8_t *game_rgb, UiCanvas *c) {
  ui_dim_rgb(c, game_rgb);
  int w = 112, h = 4 + PAUSE_ITEMS * 18 + 22, x = (UI_W - w) / 2, y = (UI_H - h) / 2;
  ui_box(c, x, y, w, h, t->border, t->panel);
  ui_text(c, font, x + (w - ui_text_width("PAUSED")) / 2, y + 2, "PAUSED", t->highlight);
  for (int i = 0; i < PAUSE_ITEMS; i++) {
    int ry = y + 22 + i * 18;
    bool on = (int)m->sel == i, enabled = pause_enabled(m, (PauseItem)i);
    if (on) ui_glyph(c, font, x + 4, ry, '>', t->highlight);
    ui_text(c, font, x + 14, ry, pause_labels[i], !enabled ? t->dim : on ? t->highlight : t->text);
  }
}

void slots_open(SlotsMenu *m, bool save) {
  m->save = save;
  m->sel = 0;
  if (!save)
    for (int i = 0; i < UI_SLOTS; i++)
      if (m->slots[i].used) { m->sel = i; break; }
}

MenuAction slots_press(SlotsMenu *m, UiButton b, int *slot) {
  switch (b) {
  case UI_UP: case UI_DOWN: {
    int dir = b == UI_UP ? -1 : 1, start = m->sel;
    do m->sel = (m->sel + dir + UI_SLOTS) % UI_SLOTS;
    while (!m->save && !m->slots[m->sel].used && m->sel != start);
    return MENU_NONE;
  }
  case UI_ACCEPT:
    if (!m->save && !m->slots[m->sel].used) return MENU_NONE;
    *slot = m->sel;
    return MENU_PICK;
  case UI_BACK: return MENU_BACK;
  default: return MENU_NONE;
  }
}

#define THUMB_DIV 5
#define THUMB_W (UI_W / THUMB_DIV)
#define THUMB_H (UI_H / THUMB_DIV)

static void draw_thumb(UiCanvas *c, int x0, int y0, const UiSlot *s, const UiTheme *t) {
  ui_box(c, x0 - 1, y0 - 1, THUMB_W + 2, THUMB_H + 2, t->border, t->bg);
  if (!s->used) return;
  for (int y = 0; y < THUMB_H; y++)
    for (int x = 0; x < THUMB_W; x++) {
      const uint8_t *p = s->thumb[y * THUMB_DIV + THUMB_DIV / 2][x * THUMB_DIV + THUMB_DIV / 2];
      memcpy(c->px[y0 + y][x0 + x], p, 3);
    }
}

void slots_draw(const SlotsMenu *m, const UiFont *font, const UiTheme *t, const uint8_t *game_rgb, UiCanvas *c) {
  if (game_rgb) ui_dim_rgb(c, game_rgb);
  else ui_clear(c, t->bg);
  ui_box(c, 0, 0, UI_W, UI_H, t->border, t->panel);
  ui_text(c, font, 4, 0, m->save ? "SAVE STATE" : "LOAD STATE", t->highlight);
  for (int i = 0; i < UI_SLOTS; i++) {
    const UiSlot *s = &m->slots[i];
    int y = 18 + i * 31;
    bool on = m->sel == i, usable = m->save || s->used;
    if (on) ui_glyph(c, font, 2, y + 6, '>', t->highlight);
    draw_thumb(c, 12, y + 1, s, t);
    char label[24];
    snprintf(label, sizeof label, "%d %s", i + 1, s->used ? s->when : "EMPTY");
    ui_text(c, font, 48, y + 6, label, !usable ? t->dim : on ? t->highlight : t->text);
  }
}
