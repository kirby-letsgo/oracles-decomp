// What sits over the game picture: the pause menu, a message, and the wait while a sync runs.
#include "gui/widgets.h"
#include <math.h>

int gui_error(int *sel, bool has_log, const GuiTheme *t, const GuiFrame *f) {
  static const char *const actions[] = {"Create issue", "Copy error log", "Back to menu"};
  gui_begin(page_scale(f));
  int tapped = GUI_NONE;
  keep_visible(CLAY_ID("ErrorList"), CLAY_IDI("Error", *sel));
  PAGE(t, f) {
    if (header(t, "Game stopped", f->hints ? hint_of(f, "select", "back") : NULL)) tapped = ERROR_BACK;
    LIST("ErrorList") {
      paragraph(CLAY_STRING("Sorry this shouldn't have crashed"), TEXT, t->paper, 16, 12);
      paragraph(CLAY_STRING("Please create an issue and attach the error log"), SMALL, t->muted, 0, 24);
      for (int i = 0; i < ERROR_ACTIONS; i++) {
        if (list_row(t, (Row){.id = CLAY_IDI("Error", i), .label = gui_str("%s", actions[i]),
                            .enabled = i != ERROR_COPY || has_log}, sel, i)) tapped = i;
      }
    }
  }
  return tapped;
}

static const char *const pause_labels[PAUSE_ITEMS] = {"Resume", "Save state", "Load state", "Settings", "Quit to menu"};

int gui_pause(PauseMenu *m, const GuiTheme *t, const GuiFrame *f) {
  // fits the title, five rows, the hint and room for a message under them inside the picture;
  // a big picture grows it up to half again the display's scale
  float need_h = 20 + 14 + PAUSE_ITEMS * ROW_H + (f->hints ? 36 : 0) + TOAST_ROOM + 24, need_w = 240;
  gui_begin(fminf(gui_display_scale() * 1.5f, fminf(f->game.h / need_h, f->game.w / need_w)));
  int tapped = GUI_NONE, sel = (int)m->sel;
  CLAY({.id = CLAY_ID("Dim"), .layout = {.sizing = {CLAY_SIZING_FIXED(f->w), CLAY_SIZING_FIXED(f->h)}}, .backgroundColor = gui_rgba(t->ink, 200)}) {
    CLAY({.floating = {.attachTo = CLAY_ATTACH_TO_ROOT, .offset = {f->game.x, f->game.y}},
          .layout = {.sizing = {CLAY_SIZING_FIXED(f->game.w), CLAY_SIZING_FIXED(f->game.h)}, .layoutDirection = CLAY_TOP_TO_BOTTOM,
                     .padding = {0, 0, 0, gui_px(TOAST_ROOM)},
                     .childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER}, .childGap = gui_px(14)}}) {
      CLAY_TEXT(CLAY_STRING("Paused"), text_spaced(SMALL, t->paper, 2));
      CLAY({.layout = {.sizing = {CLAY_SIZING_FIXED(gui_px(216))}, .layoutDirection = CLAY_TOP_TO_BOTTOM}}) {
        for (int i = 0; i < PAUSE_ITEMS; i++) {
          Clay_ElementId id = CLAY_IDI("Pause", i);
          if (list_row(t, (Row){.id = id, .label = gui_str("%s", pause_labels[i]), .enabled = pause_enabled(m, (PauseItem)i)}, &sel, i)) tapped = i;
        }
      }
      if (f->hints) label(hint_of(f, "select", "resume"), TAG, t->muted);
    }
  }
  m->sel = (PauseItem)sel;
  return tapped;
}

bool gui_toast(const char *msg, const char *action, const GuiTheme *t, const GuiFrame *f) {
  Clay_ElementId u = Clay_GetElementId((Clay_String){.length = action ? (int32_t)SDL_strlen(action) : 0, .chars = action ? action : ""});
  bool tapped = action && gui_tapped(u);
  CLAY({.id = CLAY_ID("Toast"),
        .floating = {.attachTo = CLAY_ATTACH_TO_ROOT, .zIndex = 100,
                     .offset = {f->game.x + f->game.w / 2, f->game.y + f->game.h - gui_px(14)},
                     .attachPoints = {.element = CLAY_ATTACH_POINT_CENTER_BOTTOM, .parent = CLAY_ATTACH_POINT_LEFT_TOP}},
        .layout = {.sizing = {CLAY_SIZING_FIT(0), CLAY_SIZING_FIXED(gui_px(ROW_H))}, .padding = {gui_px(16), gui_px(action ? 4 : 16), 0, 0},
                   .childGap = gui_px(14), .childAlignment = {.y = CLAY_ALIGN_Y_CENTER}},
        .backgroundColor = gui_rgb(t->ink),
        .border = {.color = gui_rgb(t->line_strong), .width = CLAY_BORDER_OUTSIDE(1)}}) {
    mark(true, t->accent);
    label(msg, 22, t->paper);
    if (action)
      CLAY({.id = u, .layout = {.sizing = {CLAY_SIZING_FIT(0), CLAY_SIZING_GROW(0)}, .padding = {gui_px(12), gui_px(12), 0, 0}, .childAlignment = {.y = CLAY_ALIGN_Y_CENTER}}}) {
        label(action, 22, t->accent);
      }
  }
  return tapped;
}

void gui_wait(const char *msg, uint64_t ticks, const GuiTheme *t, const GuiFrame *f) {
  // Over the whole screen, so nothing under it can be hovered or tapped while it waits.
  CLAY({.id = CLAY_ID("Wait"),
        .floating = {.attachTo = CLAY_ATTACH_TO_ROOT, .zIndex = 200},
        .layout = {.sizing = {CLAY_SIZING_FIXED(f->w), CLAY_SIZING_FIXED(f->h)}, .childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER}},
        .backgroundColor = gui_rgba(t->ink, 200)}) {
    CLAY({.layout = {.sizing = {CLAY_SIZING_FIT(gui_px(200))}, .layoutDirection = CLAY_TOP_TO_BOTTOM, .childGap = gui_px(14),
                     .padding = {gui_px(24), gui_px(24), gui_px(18), gui_px(20)}, .childAlignment = {.x = CLAY_ALIGN_X_CENTER}},
          .backgroundColor = gui_rgb(t->ink),
          .border = {.color = gui_rgb(t->line_strong), .width = CLAY_BORDER_OUTSIDE(1)}}) {
      label(msg, TEXT, t->paper);
      // three squares, lit in turn: it is working, without a percentage it cannot know
      int lit = (int)(ticks / 300 % 3);
      CLAY({.layout = {.childGap = gui_px(10)}}) {
        for (int i = 0; i < 3; i++) mark(i == lit, i == lit ? t->accent : t->line_strong);
      }
    }
  }
}
