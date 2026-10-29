#include "gui/widgets.h"

int gui_mods(const ModInfo *info, bool tracker, bool playing, const char *status, const GuiTheme *t, const GuiFrame *f) {
  gui_begin(page_scale(f));
  int tapped = GUI_NONE;
  PAGE(t, f) {
    if (header(t, "Mods", f->hints ? hint_of(f, "change", "back") : NULL)) tapped = GUI_BACK;
    LIST("ModsList") {
      section(t, "Official mods");
      CLAY({.layout = {.sizing = {CLAY_SIZING_GROW(0)}, .layoutDirection = CLAY_TOP_TO_BOTTOM},
            .border = {.color = gui_rgb(t->line_strong), .width = CLAY_BORDER_OUTSIDE(1)}}) {
        if (tap_row(t, (Row){.id = CLAY_IDI("OfficialMod", 0), .label = gui_str("%s", info->name),
            .value = tracker ? CLAY_STRING("On") : CLAY_STRING("Off"), .enabled = true,
            .selected = true, .toggle = tracker ? 1 : 2})) tapped = 0;
        if (info->description[0]) paragraph(gui_str("%s", info->description), SMALL, t->paper, 12, 12);
        const char *games = info->ages && info->seasons ? "Ages + Seasons" : info->ages ? "Ages" : "Seasons";
        paragraph(gui_str("%s  /  %s", games, info->changes_game ? "Changes gameplay" : "Overlay only"), TAG, t->soft, 0, 8);
        if (info->author[0] || info->version[0])
          paragraph(gui_str("%s%s%s%s", info->author, info->author[0] && info->version[0] ? "  /  " : "",
                            info->version[0] ? "v" : "", info->version), TAG, t->muted, 0, 12);
      }
      paragraph(playing ? CLAY_STRING("Restart the game from the main menu to apply changes.")
                        : CLAY_STRING("Ready to use. No download needed. Your selection is saved."), SMALL, t->soft, 16, 0);
      if (status && status[0]) paragraph(gui_str("%s", status), SMALL, t->error, 12, 0);
    }
  }
  return tapped;
}
