// The settings list: its own page from the pause menu, and the main menu's settings tab.
#include "gui/widgets.h"

static const char *const filter_labels[FILTERS] = {"Sharp", "Lines", "LCD", "CRT", "xBRZ", "HQX"};
static const char *const pad_style_labels[STYLES] = {"Auto", "Nintendo", "Xbox", "PlayStation"};
static const char *const setting_labels[SET_ROWS] = {"Volume", "Screen", "Scale", "Widescreen", "Dim sides", "Colours", "Fullscreen",
                                                     "Fast text", "Fast menus", "Fast-forward", "Quick swap", "Item X/Y buttons", "Vibration", "Controls", "Pad button names", "Save sync", "Portable", "Version", "Copy error log"};

static const char *section_before(int r) {
  switch (r) {
  case SET_VOLUME: return "Sound";
  case SET_SCREEN: return "Picture";
  case SET_FAST_TEXT: return "Play";
  case SET_CONTROLS: return "System";
  case SET_VERSION: return "About";
  default: return NULL;
  }
}

// 1 On, 2 Off, 0 not an On/Off row
static int toggle_of(const Settings *s, int r) {
  bool v;
  switch (r) {
  case SET_WIDESCREEN: v = s->widescreen; break;
  case SET_DIM_SIDES: v = s->dim_sides; break;
  case SET_FULLSCREEN: v = s->fullscreen; break;
  case SET_FAST_TEXT: v = s->fast_text; break;
  case SET_FAST_MENUS: v = s->fast_menus; break;
  case SET_QUICK_SWAP: v = s->quick_swap; break;
  case SET_FOUR_SLOTS: v = s->four_slots; break;
  case SET_HAPTICS: v = s->haptics; break;
  case SET_PORTABLE: v = s->portable; break;
  default: return 0;
  }
  return v ? 1 : 2;
}

static Clay_String value_of(const Settings *s, const char *sync_label, int r, int toggle) {
  switch (r) {
  case SET_SCREEN: return gui_str("< %s >", filter_labels[s->filter]);
  case SET_SCALE: return gui_str("< %s >", s->fill ? "Fill" : "Pixel");
  case SET_COLOURS: return gui_str("< %s >", s->gbc_colours ? "GBC" : "Vivid");
  case SET_FF_SPEED: return gui_str("< %dx >", s->ff_speed);
  case SET_CONTROLS: return CLAY_STRING(">");
  case SET_PAD_STYLE: return gui_str("< %s >", pad_style_labels[s->pad_style < STYLES ? s->pad_style : STYLE_AUTO]);
  case SET_SYNC: return gui_str("%s  >", sync_label && sync_label[0] ? sync_label : "Off");
  case SET_ERROR_LOG: return s->error_log ? CLAY_STRING(">") : CLAY_STRING("No error log");
  case SET_VERSION: return s->update[0] ? gui_str("%s  Get %s  >", s->version, s->update) : gui_str("%s", s->version);
  default: return toggle ? gui_str("%s", toggle == 1 ? "On" : "Off") : (Clay_String){0};
  }
}

// Volume: - and + either side of ten bars. On a phone a bar is a small target, so the buttons are
// the easy way down and up; a bar still sets that level directly.
enum { VOL_NONE, VOL_DOWN, VOL_UP, VOL_BAR };   // VOL_BAR + n: bar n (0..9)

static int volume_step(const GuiTheme *t, const char *id, const char *sign, bool sel) {
  Clay_ElementId bid = Clay_GetElementId((Clay_String){.length = (int32_t)SDL_strlen(id), .chars = id});
  bool tapped = gui_tapped(bid);
  CLAY({.id = bid, .layout = {.sizing = {CLAY_SIZING_FIXED(gui_px(40)), CLAY_SIZING_FIXED(gui_px(ROW_H))},
                              .childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER}}}) {
    CLAY({.layout = {.sizing = {CLAY_SIZING_FIXED(gui_px(30)), CLAY_SIZING_FIXED(gui_px(30))}, .childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER}},
          .border = {.color = gui_rgb(sel ? t->on_hi : t->line_strong), .width = CLAY_BORDER_OUTSIDE(1)}}) {
      label(sign, TEXT, sel ? t->on_hi : t->paper);
    }
  }
  return tapped;
}

static int volume_row(const GuiTheme *t, Clay_ElementId id, int volume, bool sel) {
  int hit = VOL_NONE;
  ROW(t, id, sel) {
    label("Volume", TEXT, row_fg(t, sel, true));
    spacer();
    if (volume_step(t, "VolumeDown", "-", sel)) hit = VOL_DOWN;
    CLAY({.layout = {.childAlignment = {.y = CLAY_ALIGN_Y_CENTER}}}) {
      for (int i = 0; i < 10; i++) {
        Clay_ElementId bar = CLAY_IDI("Volume", i);
        if (gui_tapped(bar)) hit = VOL_BAR + i;
        UiColor c = i < volume ? (sel ? t->on_hi : t->accent) : (sel ? t->hi_dim : t->line);
        // each bar is its own target: tall enough to hit, drawn as a short block
        CLAY({.id = bar, .layout = {.sizing = {CLAY_SIZING_FIXED(gui_px(11)), CLAY_SIZING_FIXED(gui_px(ROW_H))}, .childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER}}}) {
          CLAY({.layout = {.sizing = {CLAY_SIZING_FIXED(gui_px(8)), CLAY_SIZING_FIXED(gui_px(16))}}, .backgroundColor = gui_rgb(c)}) {}
        }
      }
    }
    if (volume_step(t, "VolumeUp", "+", sel)) hit = VOL_UP;
  }
  return hit;
}

void settings_rows(SettingsMenu *m, const Settings *s, const char *sync_label, const GuiTheme *t, bool focused, GuiSettingsTap *tap) {
  tap->row = GUI_NONE;
  tap->volume = -1;
  int sel = (int)m->sel;
  for (int r = 0; r < SET_ROWS; r++) {
    if (!settings_row_shown(m, (SettingsRow)r)) continue;
    const char *sec = section_before(r);
    if (sec) section(t, sec);
    Clay_ElementId id = CLAY_IDI("Setting", r);
    if (r == SET_VOLUME) {
      // - and + step it as Left and Right do, a bar sets its level, the rest of the row just selects it
      bool row_tapped = pick(id, true, &sel, r);
      int hit = volume_row(t, id, s->volume, focused && sel == r);
      if (hit != VOL_NONE || row_tapped) { sel = r; tap->row = r; tap->button = (UiButton)GUI_NONE; }
      if (hit == VOL_DOWN) tap->button = UI_LEFT;
      if (hit == VOL_UP) tap->button = UI_RIGHT;
      if (hit >= VOL_BAR) tap->volume = hit - VOL_BAR + 1;
      continue;
    }
    int tg = toggle_of(s, r);
    bool enabled = r != SET_ERROR_LOG || s->error_log;
    if (pick(id, enabled, &sel, r)) { tap->row = r; tap->button = UI_ACCEPT; }
    // unfocused (the main menu's tab row has the keys), nothing in the list shows as selected
    row(t, (Row){.id = id, .label = gui_str("%s", setting_labels[r]), .value = value_of(s, sync_label, r, tg), .enabled = enabled, .toggle = tg,
                 .selected = focused && sel == r});
  }
  m->sel = (SettingsRow)sel;
}

bool gui_settings(SettingsMenu *m, const Settings *s, const char *sync_label, const GuiTheme *t, const GuiFrame *f, GuiSettingsTap *tap) {
  gui_begin(page_scale(f));
  bool back = false;
  keep_visible(CLAY_ID("SettingsList"), CLAY_IDI("Setting", m->sel));
  PAGE(t, f) {
    back = header(t, "Settings", f->hints ? hint_of(f, "change", "back") : NULL);
    LIST("SettingsList") { settings_rows(m, s, sync_label, t, true, tap); }
  }
  return back;
}
