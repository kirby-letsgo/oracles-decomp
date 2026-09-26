#include "ui/launcher.h"
#include <stdio.h>
#include <string.h>

static const char *const game_names[UI_GAMES] = {"AGES", "SEASONS"};

static bool row_shown(const Launcher *l, LauncherRow r) {
  const LauncherGame *g = &l->games[l->game];
  return (r != ROW_RESUME || g->has_resume) && (r != ROW_SLOTS || g->has_slots);
}

// Resume if there is one, else the first file with data, else the first file
static void select_game(Launcher *l, UiGame g) {
  l->game = g;
  l->row = ROW_FILE1;
  if (l->games[g].has_resume) { l->row = ROW_RESUME; return; }
  for (int i = 0; i < UI_FILES; i++)
    if (l->games[g].files[i].valid) { l->row = (LauncherRow)i; break; }
}

void launcher_init(Launcher *l, UiGame initial) {
  select_game(l, l->games[initial].installed || !l->games[!initial].installed ? initial : (UiGame)!initial);
}

static void step(Launcher *l, int dir) {
  do l->row = (LauncherRow)((l->row + dir + ROW_TITLE + 1) % (ROW_TITLE + 1));
  while (!row_shown(l, l->row));
}

LaunchResult launcher_press(Launcher *l, UiButton b) {
  LaunchResult r = {LAUNCH_NONE, l->game, LAUNCH_TITLE};
  switch (b) {
  case UI_LEFT: case UI_RIGHT: select_game(l, (UiGame)!l->game); break;
  case UI_UP: step(l, -1); break;
  case UI_DOWN: step(l, 1); break;
  case UI_BACK: r.action = LAUNCH_QUIT; break;
  case UI_ACCEPT:
    if (!l->games[l->game].installed) { r.action = LAUNCH_ADD_ROM; break; }
    r.action = LAUNCH_PLAY;
    r.file = l->row <= ROW_FILE3 ? (int)l->row : l->row == ROW_RESUME ? LAUNCH_RESUME : l->row == ROW_SLOTS ? LAUNCH_SLOTS : LAUNCH_TITLE;
    break;
  }
  return r;
}

static void cursor(UiCanvas *c, const UiFont *font, const UiTheme *t, int y) { ui_glyph(c, font, 4, y, '>', t->highlight); }

static void draw_file(UiCanvas *c, const UiFont *font, const UiTheme *t, int y, int i, const UiFileInfo *f, bool selected) {
  UiColor fg = selected ? t->highlight : t->text;
  char buf[24];
  if (selected) cursor(c, font, t, y);
  snprintf(buf, sizeof buf, "%d", i + 1);
  ui_text(c, font, 12, y, buf, fg);
  if (!f->valid) { ui_text(c, font, 28, y, "-----", t->dim); return; }
  ui_text(c, font, 28, y, f->name[0] ? f->name : "?", fg);
  ui_glyph(c, font, 76, y, UI_CH_HEART, UI_RED);
  snprintf(buf, sizeof buf, "%d", f->max_health / 4);
  ui_text(c, font, 84, y, buf, fg);
  if (f->essence_count) {
    ui_glyph(c, font, 108, y, '*', t->accent);
    snprintf(buf, sizeof buf, "%d", f->essence_count);
    ui_text(c, font, 116, y, buf, fg);
  }
  if (f->completed) ui_text(c, font, 136, y, "!", t->highlight);
}

static void draw_row(UiCanvas *c, const UiFont *font, const UiTheme *t, int y, const char *label, bool selected) {
  if (selected) cursor(c, font, t, y);
  ui_text(c, font, 12, y, label, selected ? t->highlight : t->text);
}

void launcher_draw(const Launcher *l, const UiFont *font, UiCanvas *c) {
  const LauncherGame *g = &l->games[l->game];
  const UiTheme *t = &g->theme;
  ui_clear(c, t->bg);
  for (int i = 0; i < UI_GAMES; i++) {
    const UiTheme *ti = &l->games[i].theme;
    int w = ui_text_width(game_names[i]) + 8, x = i == 0 ? 8 : UI_W - 8 - w;
    bool on = (UiGame)i == l->game;
    ui_box(c, x, 4, w, 20, on ? ti->border : ti->dim, on ? ti->panel : t->bg);
    ui_text(c, font, x + 4, 6, game_names[i], on ? ti->text : ti->dim);
  }
  ui_box(c, 0, 28, UI_W, 92, t->border, t->panel);
  if (g->installed) {
    int step = g->has_resume && g->has_slots ? 15 : 18, y = 30;
    for (int i = 0; i < UI_FILES; i++, y += step) draw_file(c, font, t, y, i, &g->files[i], l->row == (LauncherRow)i);
    if (g->has_resume) { draw_row(c, font, t, y, "RESUME", l->row == ROW_RESUME); y += step; }
    if (g->has_slots) { draw_row(c, font, t, y, "LOAD STATE", l->row == ROW_SLOTS); y += step; }
    draw_row(c, font, t, y, "TITLE SCREEN", l->row == ROW_TITLE);
  } else {
    ui_text(c, font, 12, 40, "NOT INSTALLED", t->dim);
    draw_row(c, font, t, 72, "ADD ROM...", true);
  }
  ui_text(c, font, 4, 124, "A:PLAY  B:QUIT", t->dim);
}
