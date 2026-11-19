// Key and gamepad bindings.
#include "gui/widgets.h"

static const char *const control_labels[CTRL_ROWS] = {"Up", "Down", "Left", "Right", "A", "B", "Start", "Select", "Pause",
                                                      "Fast forward", "Swap A/B", "Item X", "Item Y", "Save state", "Load state", "Reset to defaults"};
static const char *const page_labels[2] = {"Keyboard", "Gamepad"};

static const char *section_before(int r) {
  switch (r) {
  case ACT_UP: return "Movement";
  case ACT_A: return "Buttons";
  case ACT_PAUSE: return "More";
  case ACT_SAVE_STATE: return "Slot 1";
  default: return NULL;
  }
}

static void key_chip(const GuiTheme *t, const char *name, bool sel) {
  CLAY({.layout = {.sizing = {CLAY_SIZING_FIT(0), CLAY_SIZING_FIXED(gui_px(28))}, .padding = {gui_px(8), gui_px(8), 0, 0}, .childAlignment = {.y = CLAY_ALIGN_Y_CENTER}},
        .border = {.color = gui_rgb(sel ? t->on_hi : t->line_strong), .width = CLAY_BORDER_OUTSIDE(1)}}) {
    label(name, SMALL, sel ? t->on_hi : t->soft);
  }
}

// What the action is bound to: a chip per key (the first even when unbound), or the pad button.
static void bindings_of(const ControlsMenu *m, const Bindings *b, int r, const char *(*key_name)(int), const char *(*pad_name)(int), const GuiTheme *t, bool sel) {
  CLAY({.layout = {.childGap = gui_px(8), .childAlignment = {.y = CLAY_ALIGN_Y_CENTER}}}) {
    if (m->pad_page) key_chip(t, b->pad[r] == NO_BINDING ? "-" : pad_name(b->pad[r]), sel);
    else
      for (int k = 0; k < KEYS_PER_ACTION; k++)
        if (b->key[r][k] != NO_BINDING || k == 0) key_chip(t, b->key[r][k] == NO_BINDING ? "-" : key_name(b->key[r][k]), sel);
  }
}

int gui_controls(ControlsMenu *m, const Bindings *b, const char *(*key_name)(int), const char *(*pad_name)(int), const GuiTheme *t,
                 const GuiFrame *f, int *page) {
  gui_begin(page_scale(f));
  int tapped = GUI_NONE;
  keep_visible(CLAY_ID("ControlList"), CLAY_IDI("Control", m->sel));
  PAGE(t, f) {
    if (header(t, "Controls", f->hints ? (m->waiting ? (m->pad_page ? "Press a button or move a stick" : "Press the key") : hint_of(f, "set", "back")) : NULL)) tapped = GUI_BACK;
    *page = tab_bar(t, "ControlTab", page_labels, 2, m->pad_page, false);
    LIST("ControlList") {
      for (int r = 0; r < CTRL_ROWS; r++) {
        const char *sec = section_before(r);
        if (sec) section(t, sec);
        if (r == CTRL_RESET) CLAY({.layout = {.sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_FIXED(gui_px(14))}}}) {}
        Clay_ElementId id = CLAY_IDI("Control", r);
        // while a key is awaited, the list stays put
        if (pick(id, !m->waiting, &m->sel, r)) tapped = r;
        bool sel = m->sel == r;
        ROW(t, id, sel) {
          label(control_labels[r], TEXT, row_fg(t, sel, true));
          spacer();
          if (r == CTRL_RESET) continue;
          if (sel && m->waiting) label(m->pad_page ? "Button or stick..." : "Press a key...", TEXT, t->on_hi);
          else bindings_of(m, b, r, key_name, pad_name, t, sel);
        }
      }
    }
  }
  return tapped;
}
