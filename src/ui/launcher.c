#include "ui/launcher.h"
#include <math.h>
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
  l->settings_tab = l->settings_list = false;
  select_game(l, l->games[initial].installed || !l->games[!initial].installed ? initial : (UiGame)!initial);
}

static void step(Launcher *l, int dir) {
  do l->row = (LauncherRow)((l->row + dir + ROW_TITLE + 1) % (ROW_TITLE + 1));
  while (!row_shown(l, l->row));
}

// Tabs in order: Ages, Seasons, settings (wrapping)
static void switch_tab(Launcher *l, int dir) {
  int tab = l->settings_tab ? UI_GAMES : (int)l->game;
  tab = (tab + dir + UI_GAMES + 1) % (UI_GAMES + 1);
  l->settings_list = false;
  l->settings_tab = tab == UI_GAMES;
  if (!l->settings_tab) select_game(l, (UiGame)tab);
}

LaunchResult launcher_press(Launcher *l, UiButton b) {
  LaunchResult r = {LAUNCH_NONE, l->game, LAUNCH_TITLE, b};
  if (l->settings_tab && l->settings_list) {
    if (b == UI_BACK) l->settings_list = false;
    else r.action = LAUNCH_SETTINGS;
    return r;
  }
  if (l->settings_tab) {
    if (b == UI_LEFT || b == UI_RIGHT) switch_tab(l, b == UI_LEFT ? -1 : 1);
    else if (b == UI_DOWN || b == UI_ACCEPT) l->settings_list = true;
    else if (b == UI_BACK) r.action = LAUNCH_QUIT;
    return r;
  }
  switch (b) {
  case UI_LEFT: case UI_RIGHT: switch_tab(l, b == UI_LEFT ? -1 : 1); break;
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

// A pixel gear for the settings tab: a ring with eight teeth, drawn from its geometry.
static void draw_gear(UiCanvas *c, int cx, int cy, UiColor color) {
  for (int dy = -6; dy <= 6; dy++)
    for (int dx = -6; dx <= 6; dx++) {
      double d = sqrt((double)(dx * dx + dy * dy)), a = atan2(dy, dx) * 8 / 6.283185307179586;
      bool ring = d >= 2.0 && d < 4.2, tooth = d >= 4.2 && d < 6.2 && fabs(a - round(a)) < 0.22;
      if (ring || tooth) ui_fill(c, cx + dx, cy + dy, 1, 1, color);
    }
}

void launcher_draw(const Launcher *l, const UiFont *font, UiCanvas *c) {
  const LauncherGame *g = &l->games[l->game];
  const UiTheme *t = &g->theme;
  ui_clear(c, t->bg);
  int x = 4;
  for (int i = 0; i < UI_GAMES; i++) {
    const UiTheme *ti = &l->games[i].theme;
    int w = ui_text_width(game_names[i]) + 8;
    bool on = !l->settings_tab && (UiGame)i == l->game;
    ui_box(c, x, 4, w, 20, on ? ti->border : ti->dim, on ? ti->panel : t->bg);
    ui_text(c, font, x + 4, 6, game_names[i], on ? ti->text : ti->dim);
    x += w + 4;
  }
  int gw = 24, gx = UI_W - 4 - gw;
  bool header = l->settings_tab && !l->settings_list;
  ui_box(c, gx, 4, gw, 20, l->settings_tab ? t->border : t->dim, l->settings_tab ? t->panel : t->bg);
  draw_gear(c, gx + gw / 2, 14, header ? t->highlight : l->settings_tab ? t->text : t->dim);
  ui_box(c, 0, LAUNCHER_BODY_Y, UI_W, LAUNCHER_BODY_H, t->border, t->panel);
  if (l->settings_tab) {
    ui_text(c, font, 4, 124, l->settings_list ? "<>:CHANGE B:BACK" : "A:OPEN  B:QUIT", t->dim);
    return;
  }
  if (g->installed) {
    int rows = 4 + g->has_resume + g->has_slots, step = 88 / rows, y = 30;
    for (int i = 0; i < UI_FILES; i++, y += step) draw_file(c, font, t, y, i, &g->files[i], l->row == (LauncherRow)i);
    if (g->has_resume) { draw_row(c, font, t, y, "RESUME", l->row == ROW_RESUME); y += step; }
    if (g->has_slots) { draw_row(c, font, t, y, "LOAD STATE", l->row == ROW_SLOTS); y += step; }
    draw_row(c, font, t, y, "TITLE SCREEN", l->row == ROW_TITLE);
  } else {
    ui_text(c, font, 12, 48, "NOT INSTALLED", t->dim);
    draw_row(c, font, t, 80, "ADD ROM...", true);
  }
  ui_text(c, font, 4, 124, "A:PLAY  B:QUIT", t->dim);
}
