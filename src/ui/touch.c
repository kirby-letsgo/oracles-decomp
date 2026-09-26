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

static const char *const labels[ACTIONS] = {
  [ACT_A] = "A", [ACT_B] = "B", [ACT_START] = "START", [ACT_SELECT] = "SELECT",
  [ACT_PAUSE] = "PAUSE", [ACT_FAST] = ">>", [ACT_ITEM_X] = "X", [ACT_ITEM_Y] = "Y",
};

static bool round_button(Action a) { return a == ACT_A || a == ACT_B || a == ACT_ITEM_X || a == ACT_ITEM_Y; }
static int pill_w(Action a) { return (int)strlen(labels[a]) * UI_GLYPH_W + 8; }
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

void touch_layout(TouchLayout *l, int screen_w, int screen_h, UiRect safe, bool overlay, bool item_buttons) {
  memset(l, 0, sizeof *l);
  l->screen_w = screen_w;
  l->screen_h = screen_h;
  l->overlay = overlay;
  l->portrait = overlay && screen_h > screen_w;
  if (safe.w <= 0 || safe.h <= 0) safe = (UiRect){0, 0, screen_w, screen_h};
  int s = imax(imin(safe.w / UI_W, safe.h / UI_H), 1);
  // portrait: the largest scale that leaves room below the game for the controls
  int controls_h = 6 + PILL_H + 2 * 4 + (item_buttons ? CLUSTER_ITEMS_H : CLUSTER_H) + PILL_H;
  while (l->portrait && s > 1 && ceil_div(screen_h, s) - ceil_div(safe.y, s) - ceil_div(screen_h - safe.y - safe.h, s) - 2 * MARGIN - UI_H < controls_h) s--;
  l->scale = s;
  l->w = ceil_div(screen_w, s);
  l->h = ceil_div(screen_h, s);
  int st = ceil_div(safe.y, s), sb = ceil_div(screen_h - safe.y - safe.h, s);
  int sl = ceil_div(safe.x, s), sr = ceil_div(screen_w - safe.x - safe.w, s);
  l->game = (UiRect){(safe.x + imax(safe.w - UI_W * s, 0) / 2) / s, (safe.y + imax(safe.h - UI_H * s, 0) / 2) / s, UI_W, UI_H};
  if (!overlay) return;
  int left = sl + MARGIN, right = l->w - sr - MARGIN, top = st + MARGIN, bottom = l->h - sb - MARGIN;
  if (l->portrait) {
    l->game.y = top;
    int row = l->game.y + UI_H + 6, pills = bottom - PILL_H;
    l->button[ACT_FAST] = (UiRect){left, row, pill_w(ACT_FAST), PILL_H};
    l->button[ACT_PAUSE] = (UiRect){right - pill_w(ACT_PAUSE), row, pill_w(ACT_PAUSE), PILL_H};
    l->button[ACT_SELECT] = (UiRect){l->w / 2 - 4 - pill_w(ACT_SELECT), pills, pill_w(ACT_SELECT), PILL_H};
    l->button[ACT_START] = (UiRect){l->w / 2 + 4, pills, pill_w(ACT_START), PILL_H};
    // the D-pad and the face buttons (X/Y reach higher) centred between the two rows of pills
    int cluster_top = item_buttons ? CLUSTER_ITEMS_H - DPAD / 2 : DPAD / 2;
    int cy = (row + PILL_H + pills + cluster_top - DPAD / 2) / 2, width = right - left;
    l->dpad = circle(left + imax(DPAD / 2 + 2, width * 22 / 100), cy, DPAD / 2);
    face_buttons(l, right - imax(19 + BUTTON_R + 2, width * 22 / 100), cy, item_buttons);
    return;
  }
  int side = imax(l->game.x - sl - MARGIN, 0), cy = l->h * 3 / 5;
  int dpad_x = left + imax(DPAD / 2 + 2, side / 2), face_x = right - imax(19 + BUTTON_R + 2, side / 2);
  l->dpad = circle(dpad_x, cy, DPAD / 2);
  face_buttons(l, face_x, cy, item_buttons);
  l->button[ACT_FAST] = (UiRect){left, top, pill_w(ACT_FAST), PILL_H};
  l->button[ACT_PAUSE] = (UiRect){right - pill_w(ACT_PAUSE), top, pill_w(ACT_PAUSE), PILL_H};
  l->button[ACT_SELECT] = (UiRect){dpad_x - pill_w(ACT_SELECT) / 2, bottom - PILL_H, pill_w(ACT_SELECT), PILL_H};
  l->button[ACT_START] = (UiRect){face_x - pill_w(ACT_START) / 2, bottom - PILL_H, pill_w(ACT_START), PILL_H};
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

typedef struct { const TouchLayout *l; uint8_t *rgba; uint8_t alpha; } Ink;

static void put(const Ink *k, int x, int y, UiColor c) {
  if (x < 0 || y < 0 || x >= k->l->w || y >= k->l->h) return;
  uint8_t *p = k->rgba + ((size_t)y * k->l->w + x) * 4;
  p[0] = c.r; p[1] = c.g; p[2] = c.b; p[3] = k->alpha;
}

static bool glyph_row_inked(const UiFont *font, uint8_t ch, int r) { return ch < 128 && font->glyph[ch][r] != 0xff; }

// Centred on its ink, not its cell: the font's letters sit low in their 16-pixel cells.
static void text(const Ink *k, const UiFont *font, int cx, int cy, const char *s, UiColor c) {
  if (!font || !font->loaded) return;
  int top = UI_GLYPH_H, bottom = -1;
  for (const char *p = s; *p; p++)
    for (int r = 0; r < UI_GLYPH_H; r++)
      if (glyph_row_inked(font, (uint8_t)*p, r)) { top = imin(top, r); bottom = imax(bottom, r); }
  if (bottom < 0) return;
  int x = cx - (int)strlen(s) * UI_GLYPH_W / 2, y = cy - (top + bottom + 1) / 2;
  for (; *s; s++, x += UI_GLYPH_W)
    for (int r = 0; r < UI_GLYPH_H; r++)
      for (int b = 0; b < UI_GLYPH_W; b++)
        if ((uint8_t)*s < 128 && !(font->glyph[(uint8_t)*s][r] & (0x80 >> b))) put(k, x + b, y + r, c);
}

static void draw_circle(const Ink *k, const UiRect *b, UiColor border, UiColor fill) {
  float r = b->w / 2.0f, cx = b->x + r, cy = b->y + r;
  for (int y = b->y; y < b->y + b->h; y++)
    for (int x = b->x; x < b->x + b->w; x++) {
      float dx = x + 0.5f - cx, dy = y + 0.5f - cy, d2 = dx * dx + dy * dy;
      if (d2 <= r * r) put(k, x, y, d2 > (r - 1.2f) * (r - 1.2f) ? border : fill);
    }
}

static void draw_pill(const Ink *k, const UiRect *b, UiColor border, UiColor fill) {
  for (int y = b->y; y < b->y + b->h; y++)
    for (int x = b->x; x < b->x + b->w; x++) {
      bool edge_x = x == b->x || x == b->x + b->w - 1, edge_y = y == b->y || y == b->y + b->h - 1;
      if (edge_x && edge_y) continue;
      put(k, x, y, edge_x || edge_y ? border : fill);
    }
}

// The D-pad: a cross with a border; each held arm is lit and carries a small arrow.
static void draw_dpad(const Ink *k, const UiTheme *t, uint32_t held) {
  const UiRect *d = &k->l->dpad;
  int arm = d->w / 3, cx = d->x + d->w / 2, cy = d->y + d->h / 2;
  for (int y = d->y; y < d->y + d->h; y++)
    for (int x = d->x; x < d->x + d->w; x++) {
      int ax = x - cx, ay = y - cy;
      bool vert = ax >= -arm / 2 && ax < arm - arm / 2, horiz = ay >= -arm / 2 && ay < arm - arm / 2;
      if (!vert && !horiz) continue;
      bool edge = (vert && !horiz && (ax == -arm / 2 || ax == arm - arm / 2 - 1 || y == d->y || y == d->y + d->h - 1)) ||
                  (horiz && !vert && (ay == -arm / 2 || ay == arm - arm / 2 - 1 || x == d->x || x == d->x + d->w - 1));
      Action dir = ACTIONS;
      if (vert && !horiz) dir = ay < 0 ? ACT_UP : ACT_DOWN;
      if (horiz && !vert) dir = ax < 0 ? ACT_LEFT : ACT_RIGHT;
      bool lit = dir < ACTIONS && (held >> dir & 1);
      put(k, x, y, edge ? t->border : lit ? t->highlight : t->panel);
    }
  for (int i = 0; i < 4; i++)
    for (int j = -i; j <= i; j++) {
      put(k, cx + j, d->y + 4 + i, t->text);
      put(k, cx + j, d->y + d->h - 5 - i, t->text);
      put(k, d->x + 4 + i, cy + j, t->text);
      put(k, d->x + d->w - 5 - i, cy + j, t->text);
    }
}

void touch_draw(const TouchLayout *l, const UiFont *font, const UiTheme *t, uint32_t held, uint8_t *rgba) {
  memset(rgba, 0, (size_t)l->w * l->h * 4);
  if (!l->overlay) return;
  Ink k = {l, rgba, l->portrait ? 255 : 176};
  draw_dpad(&k, t, held);
  for (int a = 0; a < ACTIONS; a++) {
    const UiRect *b = &l->button[a];
    if (!b->w) continue;
    UiColor fill = (held >> a & 1) ? t->highlight : t->panel;
    if (round_button((Action)a)) draw_circle(&k, b, t->border, fill);
    else draw_pill(&k, b, t->border, fill);
    text(&k, font, b->x + b->w / 2, b->y + b->h / 2, labels[a], t->text);
  }
}
