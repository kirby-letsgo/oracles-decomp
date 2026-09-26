#include "ui/syncui.h"
#include <stdio.h>
#include <string.h>

static const char *const sync_labels[SYNCROWS] = {"SYNC NOW", "CREATE ACCOUNT", "ENTER CODE", "TURN OFF"};

static bool row_shown(const SyncMenu *m, SyncRow r) {
  return m->on ? r != SYNCROW_CREATE : r == SYNCROW_CREATE || r == SYNCROW_ENTER;
}

void syncmenu_open(SyncMenu *m, bool on, const char *code, const char *status) {
  memset(m, 0, sizeof *m);
  m->on = on;
  snprintf(m->code, sizeof m->code, "%s", code ? code : "");
  snprintf(m->status, sizeof m->status, "%s", status ? status : "");
  m->sel = on ? SYNCROW_NOW : SYNCROW_CREATE;
}

MenuAction syncmenu_press(SyncMenu *m, UiButton b, SyncRow *picked) {
  switch (b) {
  case UI_UP: case UI_DOWN: {
    int dir = b == UI_UP ? -1 : 1;
    do m->sel = (SyncRow)((m->sel + dir + SYNCROWS) % SYNCROWS);
    while (!row_shown(m, m->sel));
    return MENU_NONE;
  }
  case UI_ACCEPT: *picked = m->sel; return MENU_PICK;
  case UI_BACK: return MENU_BACK;
  default: return MENU_NONE;
  }
}

void syncmenu_draw(const SyncMenu *m, const UiFont *font, const UiTheme *t, const uint8_t *game_rgb, UiCanvas *c) {
  if (game_rgb) ui_dim_rgb(c, game_rgb);
  else ui_clear(c, t->bg);
  ui_box(c, 0, 0, UI_W, UI_H, t->border, t->panel);
  ui_text(c, font, 4, 0, "SYNC", t->highlight);
  if (m->on) {
    ui_text(c, font, 4, 18, "YOUR CODE", t->dim);
    ui_text(c, font, (UI_W - ui_text_width(m->code)) / 2, 34, m->code, t->text);
  } else {
    ui_text(c, font, 4, 22, "KEEP SAVES IN", t->text);
    ui_text(c, font, 4, 38, "STEP ON DEVICES", t->text);
  }
  int y = 58;
  for (int r = 0; r < SYNCROWS; r++) {
    if (!row_shown(m, (SyncRow)r)) continue;
    bool on = (int)m->sel == r;
    if (on) ui_glyph(c, font, 2, y, '>', t->highlight);
    ui_text(c, font, 10, y, sync_labels[r], on ? t->highlight : t->text);
    y += 17;
  }
  ui_text(c, font, 4, 124, m->status[0] ? m->status : "B:BACK", t->dim);
}

void codeentry_open(CodeEntry *e, const char *digits) {
  memset(e, 0, sizeof *e);
  for (int i = 0; i < 16; i++) e->digits[i] = digits && strlen(digits) == 16 ? digits[i] : '0';
}

MenuAction codeentry_press(CodeEntry *e, UiButton b) {
  char *d = &e->digits[e->pos];
  switch (b) {
  case UI_UP: *d = *d == '9' ? '0' : (char)(*d + 1); return MENU_NONE;
  case UI_DOWN: *d = *d == '0' ? '9' : (char)(*d - 1); return MENU_NONE;
  case UI_LEFT: e->pos = (e->pos + 15) % 16; return MENU_NONE;
  case UI_RIGHT: e->pos = (e->pos + 1) % 16; return MENU_NONE;
  case UI_ACCEPT: return MENU_PICK;
  case UI_BACK: return MENU_BACK;
  }
  return MENU_NONE;
}

// Digit i sits in group i/4, groups separated by a dash: x = 4 + 8 * (i + i / 4)
static int digit_x(int i) { return 4 + UI_GLYPH_W * (i + i / 4); }

void codeentry_draw(const CodeEntry *e, const UiFont *font, const UiTheme *t, UiCanvas *c) {
  ui_clear(c, t->bg);
  ui_box(c, 0, 0, UI_W, UI_H, t->border, t->panel);
  ui_text(c, font, 4, 0, "ENTER CODE", t->highlight);
  ui_text(c, font, 4, 22, "FROM YOUR OTHER", t->text);
  ui_text(c, font, 4, 38, "DEVICE'S SYNC PAGE", t->text);
  for (int i = 0; i < 16; i++) {
    bool on = i == e->pos;
    char ch[2] = {e->digits[i], 0};
    if (on) ui_fill(c, digit_x(i) - 1, 62, UI_GLYPH_W + 1, UI_GLYPH_H + 2, t->highlight);
    ui_text(c, font, digit_x(i), 63, ch, on ? t->panel : t->text);
    if (i % 4 == 3 && i < 15) ui_text(c, font, digit_x(i) + UI_GLYPH_W, 63, "-", t->dim);
  }
  ui_text(c, font, 4, 92, "A:DONE  B:CANCEL", t->dim);
}

MenuAction conflict_press(ConflictView *v, UiButton b) {
  switch (b) {
  case UI_LEFT: v->sel = 0; return MENU_NONE;
  case UI_RIGHT: v->sel = 1; return MENU_NONE;
  case UI_ACCEPT: return MENU_PICK;
  case UI_BACK: return MENU_BACK;
  default: return MENU_NONE;
  }
}

// A state's picture at a third of its size (53x48), nearest neighbour.
static void draw_thumb(UiCanvas *c, const uint8_t *rgb, int x0, int y0) {
  for (int y = 0; y < UI_H / 3; y++)
    for (int x = 0; x < UI_W / 3; x++) {
      const uint8_t *p = rgb + ((size_t)(y * 3) * UI_W + (size_t)x * 3) * 3;
      ui_fill(c, x0 + x, y0 + y, 1, 1, (UiColor){p[0], p[1], p[2]});
    }
}

void conflict_draw(const ConflictView *v, const UiFont *font, const UiTheme *t, UiCanvas *c) {
  ui_clear(c, t->bg);
  ui_text(c, font, 4, 0, "KEEP WHICH?", t->highlight);
  ui_text(c, font, 4, 16, v->what, t->text);
  for (int s = 0; s < 2; s++) {
    const ConflictSide *side = &v->side[s];
    int x = s * 80, sel = v->sel == s;
    ui_box(c, x, 34, 80, 86, sel ? t->highlight : t->dim, t->panel);
    ui_text(c, font, x + 4, 34, side->label, sel ? t->highlight : t->text);
    ui_text(c, font, x + 4, 50, side->missing ? "NONE" : side->when, t->dim);
    if (side->missing) continue;
    if (side->has_thumb) draw_thumb(c, side->thumb, x + 13, 68);
    if (side->has_files) {
      for (int i = 0; i < UI_FILES; i++) {
        char line[12];
        const UiFileInfo *f = &side->files[i];
        if (f->valid) snprintf(line, sizeof line, "%.5s", f->name[0] ? f->name : "?");
        else snprintf(line, sizeof line, "-");
        ui_text(c, font, x + 4, 66 + i * 17, line, t->text);
        if (f->valid) {
          char hearts[4];
          ui_glyph(c, font, x + 50, 66 + i * 17, UI_CH_HEART, UI_RED);
          snprintf(hearts, sizeof hearts, "%d", f->max_health / 4);
          ui_text(c, font, x + 58, 66 + i * 17, hearts, t->text);
        }
      }
    }
  }
  ui_text(c, font, 4, 124, "<> A:KEEP B:LATER", t->dim);
}
