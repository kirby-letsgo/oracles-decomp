#include "ui/settings.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *const filter_keys[FILTERS] = {"sharp", "scanlines", "lcd", "crt", "xbrz", "hqx"};

void settings_default(Settings *s) {
  s->volume = 10;
  s->fullscreen = false;
  s->filter = FILTER_SHARP;
  s->fill = false;
  s->widescreen = false;
  s->dim_sides = true;
  s->gbc_colours = false;
  s->fast_text = false;
  s->quick_swap = false;
  s->fast_menus = false;
  s->ff_speed = 3;
  s->four_slots = false;
  s->haptics = true;
  s->essence_tracker = false;
  s->pad_style = STYLE_AUTO;
  s->portable = false;
  s->error_log = false;
  s->version[0] = s->update[0] = 0;
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
      else if (!strcmp(key, "widescreen")) s->widescreen = !strcmp(value, "on");
      else if (!strcmp(key, "dim_sides")) s->dim_sides = !strcmp(value, "on");
      else if (!strcmp(key, "gbc_colours")) s->gbc_colours = !strcmp(value, "on");
      else if (!strcmp(key, "fast_text")) s->fast_text = !strcmp(value, "on");
      else if (!strcmp(key, "quick_swap")) s->quick_swap = !strcmp(value, "on");
      else if (!strcmp(key, "fast_menus")) s->fast_menus = !strcmp(value, "on");
      else if (!strcmp(key, "fast_forward")) { int v = atoi(value); if (v >= FF_SPEED_MIN && v <= FF_SPEED_MAX) s->ff_speed = v; }
      else if (!strcmp(key, "four_slots")) s->four_slots = !strcmp(value, "on");
      else if (!strcmp(key, "essence_tracker")) s->essence_tracker = !strcmp(value, "on");
      else if (!strcmp(key, "haptics")) s->haptics = !strcmp(value, "on");
      else if (!strcmp(key, "filter")) {
        for (int i = 0; i < FILTERS; i++) if (!strcmp(value, filter_keys[i])) s->filter = (ScreenFilter)i;
      }
      else if (!strcmp(key, "controller_style")) {
        for (int i = 0; i < STYLES; i++) if (!strcmp(value, pad_style_keys[i])) s->pad_style = (PadStyle)i;
      }
      else if (!strcmp(key, "keys") || !strcmp(key, "pads")) bindings_parse_line(&s->bindings, key, value);
    }
    p = eol ? eol + 1 : NULL;
  }
}

int settings_format(const Settings *s, char *out, size_t size) {
  int n = snprintf(out, size, "volume=%d\nfullscreen=%s\nfilter=%s\nscale=%s\nwidescreen=%s\ndim_sides=%s\ngbc_colours=%s\nfast_text=%s\nfast_menus=%s\nfast_forward=%d\nquick_swap=%s\nfour_slots=%s\nhaptics=%s\ncontroller_style=%s\n",
                   s->volume, s->fullscreen ? "on" : "off", filter_keys[s->filter], s->fill ? "fill" : "pixel", s->widescreen ? "on" : "off", s->dim_sides ? "on" : "off", s->gbc_colours ? "on" : "off",
                   s->fast_text ? "on" : "off", s->fast_menus ? "on" : "off", s->ff_speed, s->quick_swap ? "on" : "off", s->four_slots ? "on" : "off",
                   s->haptics ? "on" : "off", pad_style_keys[s->pad_style < STYLES ? s->pad_style : STYLE_AUTO]);
  if (n < (int)size) n += snprintf(out + n, size - n, "essence_tracker=%s\n", s->essence_tracker ? "on" : "off");
  if (n < (int)size) n += bindings_format(&s->bindings, out + n, size - n);
  return n;
}

void settings_menu_open(SettingsMenu *m, bool phone) {
  m->sel = SET_VOLUME;
  m->phone = phone;
}

bool settings_row_shown(const SettingsMenu *m, SettingsRow r) {
  if (r == SET_HAPTICS) return m->phone;
  if (r == SET_FULLSCREEN || r == SET_PORTABLE) return !m->phone;
  return true;
}

static void step(SettingsMenu *m, int dir) {
  do m->sel = (SettingsRow)((m->sel + dir + SET_ROWS) % SET_ROWS);
  while (!settings_row_shown(m, m->sel));
}

MenuAction settings_press(SettingsMenu *m, Settings *s, UiButton b, SettingsRow *picked) {
  int dir = b == UI_LEFT ? -1 : b == UI_RIGHT ? 1 : 0;
  switch (b) {
  case UI_UP: step(m, -1); return MENU_NONE;
  case UI_DOWN: step(m, 1); return MENU_NONE;
  case UI_BACK: return MENU_BACK;
  case UI_ACCEPT:
    if (m->sel == SET_MODS || m->sel == SET_CONTROLS || m->sel == SET_SYNC || (m->sel == SET_VERSION && s->update[0]) || (m->sel == SET_ERROR_LOG && s->error_log)) { *picked = m->sel; return MENU_PICK; }
    dir = 1;
    break;
  default: break;
  }
  switch (m->sel) {
  case SET_VOLUME: s->volume = s->volume + dir < 0 ? 0 : s->volume + dir > 10 ? 10 : s->volume + dir; break;
  case SET_SCREEN: s->filter = (ScreenFilter)((s->filter + dir + FILTERS) % FILTERS); break;
  case SET_SCALE: if (dir) s->fill = !s->fill; break;
  case SET_WIDESCREEN: if (dir) s->widescreen = !s->widescreen; break;
  case SET_DIM_SIDES: if (dir) s->dim_sides = !s->dim_sides; break;
  case SET_COLOURS: if (dir) s->gbc_colours = !s->gbc_colours; break;
  case SET_FULLSCREEN: if (dir) s->fullscreen = !s->fullscreen; break;
  case SET_FAST_TEXT: if (dir) s->fast_text = !s->fast_text; break;
  case SET_FAST_MENUS: if (dir) s->fast_menus = !s->fast_menus; break;
  case SET_FF_SPEED: s->ff_speed = FF_SPEED_MIN + (s->ff_speed - FF_SPEED_MIN + dir + FF_SPEEDS) % FF_SPEEDS; break;
  case SET_QUICK_SWAP: if (dir) s->quick_swap = !s->quick_swap; break;
  case SET_FOUR_SLOTS: if (dir) s->four_slots = !s->four_slots; break;
  case SET_HAPTICS: if (dir) s->haptics = !s->haptics; break;
  case SET_PAD_STYLE: s->pad_style = (PadStyle)((s->pad_style + dir + STYLES) % STYLES); break;
  case SET_PORTABLE: if (dir) s->portable = !s->portable; break;
  default: break;
  }
  return MENU_NONE;
}
