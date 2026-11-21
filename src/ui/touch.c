#include "ui/touch.h"
#include <math.h>
#include <string.h>

#define DPAD 60
#define BUTTON_R 14
#define ITEM_R 11
#define PILL_H 20
#define MARGIN 4
#define CLUSTER_H DPAD
#define CLUSTER_ITEMS_H (DPAD / 2 + 40 + ITEM_R)

// sized for the labels src/gui/pad.c draws in them, at 8 game pixels a character
static const char *const labels[ACTIONS] = {
  [ACT_A] = "A", [ACT_B] = "B", [ACT_START] = "START", [ACT_SELECT] = "SELECT",
  [ACT_PAUSE] = "PAUSE", [ACT_FAST] = ">>", [ACT_ITEM_X] = "X", [ACT_ITEM_Y] = "Y",
};

static bool round_button(Action a) { return a == ACT_A || a == ACT_B || a == ACT_ITEM_X || a == ACT_ITEM_Y; }
static int pill_w(Action a) { return (int)strlen(labels[a]) * 8 + 8; }
static UiRect circle(int cx, int cy, int r) { return (UiRect){cx - r, cy - r, 2 * r, 2 * r}; }
static int imax(int a, int b) { return a > b ? a : b; }
static int imin(int a, int b) { return a < b ? a : b; }
static int ceil_div(int a, int b) { return a <= 0 ? 0 : (a + b - 1) / b; }

// A/B side by side (B lower left, as on the Game Boy), the item buttons above them
static void face_buttons(TouchLayout *l, int cx, int cy, bool item_buttons) {
  l->button[ACT_A] = circle(cx + 19, cy - 8, BUTTON_R);
  l->button[ACT_B] = circle(cx - 19, cy + 8, BUTTON_R);
  if (!item_buttons) return;
  l->button[ACT_ITEM_X] = circle(cx + 19, cy - 40, ITEM_R);
  l->button[ACT_ITEM_Y] = circle(cx - 19, cy - 24, ITEM_R);
}

// The D-pad and face buttons (X/Y reach higher) centred between two rows of pills.
static int cluster_y(int above, int below, bool item_buttons) {
  int reach = item_buttons ? CLUSTER_ITEMS_H - DPAD / 2 : DPAD / 2;
  return (above + below + reach - DPAD / 2) / 2;
}

void touch_layout(TouchLayout *l, int screen_w, int screen_h, UiRect safe, bool overlay, int item_buttons, bool fill, int pic_w) {
  memset(l, 0, sizeof *l);
  l->screen_w = screen_w;
  l->screen_h = screen_h;
  l->pic_w = pic_w;
  l->overlay = overlay;
  l->portrait = overlay && screen_h > screen_w;
  if (safe.w <= 0 || safe.h <= 0) safe = (UiRect){0, 0, screen_w, screen_h};
  // Safe areas include the rounded corners, which the game never reaches: in portrait it takes the
  // full width and keeps clear of the top and bottom insets, in landscape the other way round.
  bool tall = screen_h > screen_w;
  int fit_w = tall ? screen_w : safe.w, fit_h = tall ? safe.h : screen_h;
  // The controls keep the size they have beside the 160-pixel picture: a wide picture takes a
  // smaller scale of its own instead of shrinking them with it.
  int grid_w = overlay ? UI_W : pic_w;
  int s = imax(imin(fit_w / grid_w, fit_h / UI_H), 1);
  // the largest scale that leaves room for the controls: below the game in portrait, beside it
  // (between the top and bottom rows of pills) in landscape
  int cluster = item_buttons ? CLUSTER_ITEMS_H : CLUSTER_H;
  int need = l->portrait ? UI_H + 6 + 2 * PILL_H + 2 * 4 + cluster : 2 * PILL_H + 2 * 4 + cluster;
  while (overlay && s > 1 && ceil_div(screen_h, s) - ceil_div(safe.y, s) - ceil_div(screen_h - safe.y - safe.h, s) - 2 * MARGIN < need) s--;
  l->scale = s;
  l->w = ceil_div(screen_w, s);
  l->h = ceil_div(screen_h, s);
  int st = ceil_div(safe.y, s), sb = ceil_div(screen_h - safe.y - safe.h, s);
  int sl = ceil_div(safe.x, s), sr = ceil_div(screen_w - safe.x - safe.w, s);
  int left = sl + MARGIN, right = l->w - sr - MARGIN, top = st + MARGIN, bottom = l->h - sb - MARGIN;
  int ps = imax(imin(s, imin(fit_w / pic_w, fit_h / UI_H)), 1);
  // fill: the largest fractional scale that fits, in portrait still leaving the controls their room
  float zoom = ps;
  if (fill) {
    zoom = fminf((float)fit_w / pic_w, (float)fit_h / UI_H);
    if (l->portrait) zoom = fminf((float)screen_w / pic_w, (float)(bottom - top - (need - UI_H)) * s / UI_H);
    zoom = fmaxf(zoom, (float)ps);
  }
  // 16:10 is 230.4 pixels wide at 144 lines. Use that aspect for Fill,
  // but keep the actual 230 squares per row for integer Pixel scaling.
  if (fill && pic_w == UI_W_16_10) zoom = fminf(zoom, (float)fit_w / (UI_H * 1.6f));
  int pw = (int)(pic_w * zoom), ph = (int)(UI_H * zoom);
  if (fill && pic_w == UI_W_16_10) {
    pw = (int)lroundf(UI_H * 1.6f * zoom);
    ph = (int)lroundf(UI_H * zoom);
  }
  int gx = tall ? imax(screen_w - pw, 0) / 2 : safe.x + imax(safe.w - pw, 0) / 2;
  int gy = l->portrait ? top * s : tall ? safe.y + imax(safe.h - ph, 0) / 2 : imax(screen_h - ph, 0) / 2;
  if (!fill && ps == s) { gx -= gx % s; gy -= gy % s; }
  // In portrait the controls follow the game. Reserve a row above it only if
  // the controls below still have their full space at the selected scale.
  if (l->portrait && item_buttons == 4 && pic_w == UI_W) {
    int bar = ceil_div(ph * UI_ITEM_BAR_H, UI_H);
    int next_y = gy + ceil_div(bar, s) * s;
    if (ceil_div(next_y + ph, s) + need - UI_H <= bottom) gy = next_y;
  }
  l->game_px = (UiRect){gx, gy, pw, ph};
  l->game = (UiRect){gx / s, gy / s, ceil_div(gx + pw, s) - gx / s, ceil_div(gy + ph, s) - gy / s};
  if (!overlay) return;
  if (item_buttons == 4) {
    int y = l->portrait ? l->game.y + l->game.h + 6 : top;
    int width = 22;
    l->button[ACT_ITEM_L] = (UiRect){left + pill_w(ACT_FAST) + 6, y, width, PILL_H};
    l->button[ACT_ITEM_R] = (UiRect){right - pill_w(ACT_PAUSE) - 6 - width, y, width, PILL_H};
  }
  if (l->portrait) {
    int row = l->game.y + l->game.h + 6, pills = bottom - PILL_H;
    l->button[ACT_FAST] = (UiRect){left, row, pill_w(ACT_FAST), PILL_H};
    l->button[ACT_PAUSE] = (UiRect){right - pill_w(ACT_PAUSE), row, pill_w(ACT_PAUSE), PILL_H};
    l->button[ACT_SELECT] = (UiRect){l->w / 2 - 4 - pill_w(ACT_SELECT), pills, pill_w(ACT_SELECT), PILL_H};
    l->button[ACT_START] = (UiRect){l->w / 2 + 4, pills, pill_w(ACT_START), PILL_H};
    int cy = cluster_y(row + PILL_H, pills, item_buttons), width = right - left;
    l->dpad = circle(left + imax(DPAD / 2 + 2, width * 22 / 100), cy, DPAD / 2);
    face_buttons(l, right - imax(19 + BUTTON_R + 2, width * 22 / 100), cy, item_buttons);
    return;
  }
  int side = imax(l->game.x - sl - MARGIN, 0), cy = cluster_y(top + PILL_H, bottom - PILL_H, item_buttons);
  int dpad_x = left + imax(DPAD / 2 + 2, side / 2), face_x = right - imax(19 + BUTTON_R + 2, side / 2);
  l->dpad = circle(dpad_x, cy, DPAD / 2);
  face_buttons(l, face_x, cy, item_buttons);
  l->button[ACT_FAST] = (UiRect){left, top, pill_w(ACT_FAST), PILL_H};
  l->button[ACT_PAUSE] = (UiRect){right - pill_w(ACT_PAUSE), top, pill_w(ACT_PAUSE), PILL_H};
  l->button[ACT_SELECT] = (UiRect){dpad_x - pill_w(ACT_SELECT) / 2, bottom - PILL_H, pill_w(ACT_SELECT), PILL_H};
  l->button[ACT_START] = (UiRect){face_x - pill_w(ACT_START) / 2, bottom - PILL_H, pill_w(ACT_START), PILL_H};
}

// Return the first row below any controls that this HUD rectangle would cover.
// Touch controls are measured on the unscaled input grid, the HUD in screen pixels.
static int clear_hud_y(const TouchLayout *l, UiRect rect) {
  int next = rect.y;
  if (!l->overlay) return next;
  for (int i = 0; i <= ACTIONS; i++) {
    UiRect b = i == ACTIONS ? l->dpad : l->button[i];
    if (!b.w) continue;
    int gap = 2 * l->scale;
    int left = b.x * l->scale - gap, right = (b.x + b.w) * l->scale + gap;
    int top = b.y * l->scale - gap, bottom = (b.y + b.h) * l->scale + gap;
    if (rect.x < right && left < rect.x + rect.w && rect.y < bottom && top < rect.y + rect.h)
      next = imax(next, bottom);
  }
  return next;
}

bool touch_item_margins(const TouchLayout *l, UiRect safe, int w, int h, UiRect slots[2]) {
  UiRect g = l->game_px;
  int left = g.x - w, right = g.x + g.w;
  if (left < safe.x || right + w > safe.x + safe.w) return false;
  int end = imin(g.y + g.h, safe.y + safe.h);
  for (int y = imax(safe.y, g.y); y + h <= end;) {
    slots[0] = (UiRect){left, y, w, h};
    slots[1] = (UiRect){right, y, w, h};
    int next = imax(clear_hud_y(l, slots[0]), clear_hud_y(l, slots[1]));
    if (next == y) return true;
    y = next;
  }
  return false;
}

bool touch_top_item_bar(TouchLayout *l, UiRect safe, UiRect *bar) {
  UiRect *g = &l->game_px;
  int x = imax(g->x, safe.x), w = imin(g->x + g->w, safe.x + safe.w) - x;
  if (w <= 0) return false;
  int h = ceil_div(w * UI_ITEM_BAR_H, UI_W);
  int y = imax(g->y, safe.y + h);
  // Portrait controls already follow the reserved row. A desktop or landscape
  // picture can use lower letterboxing if the entire room still fits.
  if ((l->portrait && y != g->y) || y + g->h > l->screen_h) return false;
  if (y > g->y && y + g->h > safe.y + safe.h) return false;
  *bar = (UiRect){x, y - h, w, h};
  if (clear_hud_y(l, *bar) != bar->y) return false;
  g->y = y;
  l->game.y = y / l->scale;
  l->game.h = ceil_div(y + g->h, l->scale) - l->game.y;
  return true;
}

uint32_t touch_hit(const TouchLayout *l, float x, float y) {
  if (!l->overlay) return 0;
  uint32_t m = 0;
  float r = l->dpad.w / 2.0f, dx = x - (l->dpad.x + r), dy = y - (l->dpad.y + r), d2 = dx * dx + dy * dy;
  if (d2 <= r * r * 1.7f && d2 >= r * r * 0.02f) {
    // eight 45-degree sectors: a direction holds within 67.5 degrees of its axis
    if (dx < -fabsf(dy) * 0.414f) m |= 1u << ACT_LEFT;
    if (dx > fabsf(dy) * 0.414f) m |= 1u << ACT_RIGHT;
    if (dy < -fabsf(dx) * 0.414f) m |= 1u << ACT_UP;
    if (dy > fabsf(dx) * 0.414f) m |= 1u << ACT_DOWN;
  }
  for (int a = 0; a < ACTIONS; a++) {
    const UiRect *b = &l->button[a];
    if (!b->w) continue;
    if (round_button((Action)a)) {
      float br = b->w / 2.0f, bx = x - (b->x + br), by = y - (b->y + br);
      if (bx * bx + by * by <= br * br * 1.7f) m |= 1u << a;
    } else if (x >= b->x - 3 && x < b->x + b->w + 3 && y >= b->y - 3 && y < b->y + b->h + 3) m |= 1u << a;
  }
  return m;
}

bool touch_point(const TouchLayout *l, Action a, float *x, float *y) {
  if (!l->overlay || a >= ACTIONS) return false;
  if (a <= ACT_RIGHT) {
    float r = l->dpad.w / 2.0f, o = r * 0.7f;
    *x = l->dpad.x + r + (a == ACT_LEFT ? -o : a == ACT_RIGHT ? o : 0);
    *y = l->dpad.y + r + (a == ACT_UP ? -o : a == ACT_DOWN ? o : 0);
    return true;
  }
  const UiRect *b = &l->button[a];
  *x = b->x + b->w / 2.0f;
  *y = b->y + b->h / 2.0f;
  return b->w > 0;
}
