#include "ui/settings.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *const filter_names[FILTERS] = {"SHARP", "LINES", "LCD", "CRT"};
static const char *const filter_keys[FILTERS] = {"sharp", "scanlines", "lcd", "crt"};

void settings_default(Settings *s) {
  s->volume = 10;
  s->fullscreen = false;
  s->filter = FILTER_SHARP;
  s->fill = false;
  s->gbc_colours = false;
  s->fast_text = false;
  s->quick_swap = false;
  s->fast_menus = false;
  s->four_slots = false;
  bindings_default(&s->bindings);
}

void settings_parse(Settings *s, const char *text) {
  char key[32], value[256];
  for (const char *p = text; p && *p;) {
    const char *eol = strchr(p, '\n');
    size_t len = eol ? (size_t)(eol - p) : strlen(p);
    char line[300];
    snprintf(line, sizeof line, "%.*s", (int)(len < sizeof line ? len : sizeof line - 1), p);
    if (sscanf(line, " %31[a-z_] = %255s", key, value) == 2) {
      if (!strcmp(key, "volume")) { int v = atoi(value); if (v >= 0 && v <= 10) s->volume = v; }
      else if (!strcmp(key, "fullscreen")) s->fullscreen = !strcmp(value, "on");
      else if (!strcmp(key, "scale")) s->fill = !strcmp(value, "fill");
      else if (!strcmp(key, "gbc_colours")) s->gbc_colours = !strcmp(value, "on");
      else if (!strcmp(key, "fast_text")) s->fast_text = !strcmp(value, "on");
      else if (!strcmp(key, "quick_swap")) s->quick_swap = !strcmp(value, "on");
      else if (!strcmp(key, "fast_menus")) s->fast_menus = !strcmp(value, "on");
      else if (!strcmp(key, "four_slots")) s->four_slots = !strcmp(value, "on");
      else if (!strcmp(key, "filter")) {
        for (int i = 0; i < FILTERS; i++) if (!strcmp(value, filter_keys[i])) s->filter = (ScreenFilter)i;
      }
      else if (!strcmp(key, "keys") || !strcmp(key, "pads")) bindings_parse_line(&s->bindings, key, value);
    }
    p = eol ? eol + 1 : NULL;
  }
}

int settings_format(const Settings *s, char *out, size_t size) {
  int n = snprintf(out, size, "volume=%d\nfullscreen=%s\nfilter=%s\nscale=%s\ngbc_colours=%s\nfast_text=%s\nfast_menus=%s\nquick_swap=%s\nfour_slots=%s\n",
                   s->volume, s->fullscreen ? "on" : "off", filter_keys[s->filter], s->fill ? "fill" : "pixel", s->gbc_colours ? "on" : "off",
                   s->fast_text ? "on" : "off", s->fast_menus ? "on" : "off", s->quick_swap ? "on" : "off", s->four_slots ? "on" : "off");
  if (n < (int)size) n += bindings_format(&s->bindings, out + n, size - n);
  return n;
}

void settings_menu_open(SettingsMenu *m) { m->sel = SET_VOLUME; }

MenuAction settings_press(SettingsMenu *m, Settings *s, UiButton b, SettingsRow *picked) {
  int dir = b == UI_LEFT ? -1 : b == UI_RIGHT ? 1 : 0;
  switch (b) {
  case UI_UP: m->sel = (SettingsRow)((m->sel + SET_ROWS - 1) % SET_ROWS); return MENU_NONE;
  case UI_DOWN: m->sel = (SettingsRow)((m->sel + 1) % SET_ROWS); return MENU_NONE;
  case UI_BACK: return MENU_BACK;
  case UI_ACCEPT:
    if (m->sel == SET_CONTROLS) { *picked = m->sel; return MENU_PICK; }
    dir = 1;
    break;
  default: break;
  }
  switch (m->sel) {
  case SET_VOLUME: s->volume = s->volume + dir < 0 ? 0 : s->volume + dir > 10 ? 10 : s->volume + dir; break;
  case SET_SCREEN: s->filter = (ScreenFilter)((s->filter + dir + FILTERS) % FILTERS); break;
  case SET_SCALE: if (dir) s->fill = !s->fill; break;
  case SET_COLOURS: if (dir) s->gbc_colours = !s->gbc_colours; break;
  case SET_FULLSCREEN: if (dir) s->fullscreen = !s->fullscreen; break;
  case SET_FAST_TEXT: if (dir) s->fast_text = !s->fast_text; break;
  case SET_FAST_MENUS: if (dir) s->fast_menus = !s->fast_menus; break;
  case SET_QUICK_SWAP: if (dir) s->quick_swap = !s->quick_swap; break;
  case SET_FOUR_SLOTS: if (dir) s->four_slots = !s->four_slots; break;
  default: break;
  }
  return MENU_NONE;
}

void settings_draw(const SettingsMenu *m, const Settings *s, const UiFont *font, const UiTheme *t, const uint8_t *game_rgb, UiCanvas *c) {
  static const char *const labels[SET_ROWS] = {"VOLUME", "SCREEN", "SCALE", "COLOURS", "FULLSCREEN", "FAST TEXT", "FAST MENUS", "QUICK SWAP", "4 SLOTS", "CONTROLS"};
  if (game_rgb) ui_dim_rgb(c, game_rgb);
  else ui_clear(c, t->bg);
  ui_box(c, 0, 0, UI_W, UI_H, t->border, t->panel);
  ui_text(c, font, 4, 0, "SETTINGS", t->highlight);
  enum { VISIBLE = 6 };
  int first = (int)m->sel - VISIBLE / 2;
  if (first > SET_ROWS - VISIBLE) first = SET_ROWS - VISIBLE;
  if (first < 0) first = 0;
  for (int i = first; i < first + VISIBLE && i < SET_ROWS; i++) {
    int y = 18 + (i - first) * 17;
    bool on = (int)m->sel == i;
    UiColor fg = on ? t->highlight : t->text;
    if (on) ui_glyph(c, font, 2, y, '>', t->highlight);
    ui_text(c, font, 10, y, labels[i], fg);
    char v[16] = "";
    switch ((SettingsRow)i) {
    case SET_VOLUME: snprintf(v, sizeof v, "%d", s->volume); break;
    case SET_SCREEN: snprintf(v, sizeof v, "%s", filter_names[s->filter]); break;
    case SET_SCALE: snprintf(v, sizeof v, "%s", s->fill ? "FILL" : "PIXEL"); break;
    case SET_COLOURS: snprintf(v, sizeof v, "%s", s->gbc_colours ? "GBC" : "VIVID"); break;
    case SET_FULLSCREEN: snprintf(v, sizeof v, "%s", s->fullscreen ? "ON" : "OFF"); break;
    case SET_FAST_TEXT: snprintf(v, sizeof v, "%s", s->fast_text ? "ON" : "OFF"); break;
    case SET_FAST_MENUS: snprintf(v, sizeof v, "%s", s->fast_menus ? "ON" : "OFF"); break;
    case SET_QUICK_SWAP: snprintf(v, sizeof v, "%s", s->quick_swap ? "ON" : "OFF"); break;
    case SET_FOUR_SLOTS: snprintf(v, sizeof v, "%s", s->four_slots ? "ON" : "OFF"); break;
    default: break;
    }
    if (*v) ui_text(c, font, UI_W - 4 - ui_text_width(v), y, v, fg);
  }
  ui_text(c, font, 4, 124, "<>:CHANGE B:BACK", t->dim);
}
