// The on-screen controls, drawn at the screen's resolution in the menus' style: thin square
// outlines (round for the face buttons), VT323 labels, the game's colour while held. Where they
// are and what a finger presses stay in src/ui/touch.c.
#include "gui/widgets.h"
#include <math.h>
#include <stdlib.h>

static const char *const labels[ACTIONS] = {
  [ACT_A] = "A", [ACT_B] = "B", [ACT_START] = "Start", [ACT_SELECT] = "Select",
  [ACT_PAUSE] = "Pause", [ACT_FAST] = ">>", [ACT_ITEM_X] = "X", [ACT_ITEM_Y] = "Y", [ACT_ITEM_L] = "L", [ACT_ITEM_R] = "R",
};

typedef struct { SDL_Renderer *ren; float s, line; } Pen;

static SDL_FColor fcolor(UiColor c, float a) { return (SDL_FColor){c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, a}; }

// A star-shaped polygon (every point seen from its centre), filled as a fan.
static void fill_poly(const Pen *p, float cx, float cy, const SDL_FPoint *pts, int n, SDL_FColor c) {
  SDL_Vertex v[64];
  int idx[192], k = 0;
  v[0] = (SDL_Vertex){{cx, cy}, c, {0, 0}};
  for (int i = 0; i < n; i++) {
    v[i + 1] = (SDL_Vertex){pts[i], c, {0, 0}};
    idx[k++] = 0, idx[k++] = i + 1, idx[k++] = (i + 1) % n + 1;
  }
  SDL_RenderGeometry(p->ren, NULL, v, n + 1, idx, k);
}

// The band between a closed outline and the same outline moved inward.
static void stroke_poly(const Pen *p, const SDL_FPoint *outer, const SDL_FPoint *inner, int n, SDL_FColor c) {
  SDL_Vertex v[128];
  int idx[384], k = 0;
  for (int i = 0; i < n; i++) {
    v[2 * i] = (SDL_Vertex){outer[i], c, {0, 0}};
    v[2 * i + 1] = (SDL_Vertex){inner[i], c, {0, 0}};
    int j = (i + 1) % n;
    idx[k++] = 2 * i, idx[k++] = 2 * j, idx[k++] = 2 * i + 1;
    idx[k++] = 2 * i + 1, idx[k++] = 2 * j, idx[k++] = 2 * j + 1;
  }
  SDL_RenderGeometry(p->ren, NULL, v, 2 * n, idx, k);
}

#define ROUND 48

static void circle_pts(float cx, float cy, float r, SDL_FPoint *pts) {
  for (int i = 0; i < ROUND; i++) {
    float a = (float)i / ROUND * 2 * SDL_PI_F;
    pts[i] = (SDL_FPoint){cx + r * cosf(a), cy + r * sinf(a)};
  }
}

// A rectangle, or with round set the circle inside it.
static void shape(const Pen *p, SDL_FRect b, bool round, SDL_FColor fill, SDL_FColor line) {
  SDL_FPoint out[ROUND], in[ROUND];
  float cx = b.x + b.w / 2, cy = b.y + b.h / 2;
  int n = 4;
  if (round) {
    n = ROUND;
    circle_pts(cx, cy, b.w / 2, out);
    circle_pts(cx, cy, b.w / 2 - p->line, in);
  } else {
    float l = p->line;
    SDL_FPoint o[4] = {{b.x, b.y}, {b.x + b.w, b.y}, {b.x + b.w, b.y + b.h}, {b.x, b.y + b.h}};
    SDL_FPoint i[4] = {{b.x + l, b.y + l}, {b.x + b.w - l, b.y + l}, {b.x + b.w - l, b.y + b.h - l}, {b.x + l, b.y + b.h - l}};
    SDL_memcpy(out, o, sizeof o);
    SDL_memcpy(in, i, sizeof i);
  }
  fill_poly(p, cx, cy, in, n, fill);
  stroke_poly(p, out, in, n, line);
}

// The cross: a held arm takes the game's colour, each arm points its way with a small triangle.
static void dpad(const Pen *p, const TouchLayout *l, const GuiTheme *t, uint32_t held, float alpha) {
  float s = p->s, R = l->dpad.w * s / 2, a = l->dpad.w * s / 6, cx = l->dpad.x * s + R, cy = l->dpad.y * s + R, w = p->line;
  static const int sx[12] = {-1, 1, 1, 2, 2, 1, 1, -1, -1, -2, -2, -1}, sy[12] = {-2, -2, -1, -1, 1, 1, 2, 2, 1, 1, -1, -1};
  SDL_FPoint out[12], in[12];
  for (int i = 0; i < 12; i++) {
    // 1: half the arm's width, 2: the arm's reach; the inner outline is both less the line
    float ox = abs(sx[i]) == 1 ? a : R, oy = abs(sy[i]) == 1 ? a : R;
    out[i] = (SDL_FPoint){cx + (sx[i] > 0 ? ox : -ox), cy + (sy[i] > 0 ? oy : -oy)};
    in[i] = (SDL_FPoint){cx + (sx[i] > 0 ? ox - w : -(ox - w)), cy + (sy[i] > 0 ? oy - w : -(oy - w))};
  }
  fill_poly(p, cx, cy, in, 12, fcolor(t->ink, alpha));
  static const Action dirs[4] = {ACT_UP, ACT_RIGHT, ACT_DOWN, ACT_LEFT};
  static const float dx[4] = {0, 1, 0, -1}, dy[4] = {-1, 0, 1, 0};
  for (int i = 0; i < 4; i++) {
    bool lit = held >> dirs[i] & 1;
    if (lit) {
      // the arm from the centre square's edge out
      float x0 = dx[i] ? cx + dx[i] * a : cx - a + w, x1 = dx[i] ? cx + dx[i] * (R - w) : cx + a - w;
      float y0 = dy[i] ? cy + dy[i] * a : cy - a + w, y1 = dy[i] ? cy + dy[i] * (R - w) : cy + a - w;
      SDL_SetRenderDrawColorFloat(p->ren, t->hi.r / 255.0f, t->hi.g / 255.0f, t->hi.b / 255.0f, 1);
      SDL_RenderFillRect(p->ren, &(SDL_FRect){fminf(x0, x1), fminf(y0, y1), fabsf(x1 - x0), fabsf(y1 - y0)});
    }
    // a triangle two thirds of the way out, pointing away from the centre
    float tip = (R + a) / 2 + a * 0.35f, base = tip - a * 0.6f, half = a * 0.45f;
    SDL_FPoint tri[3] = {{cx + dx[i] * tip, cy + dy[i] * tip},
                         {cx + dx[i] * base - dy[i] * half, cy + dy[i] * base - dx[i] * half},
                         {cx + dx[i] * base + dy[i] * half, cy + dy[i] * base + dx[i] * half}};
    SDL_FColor c = lit ? fcolor(t->on_hi, 1) : fcolor(t->soft, alpha);
    SDL_Vertex v[3] = {{tri[0], c, {0, 0}}, {tri[1], c, {0, 0}}, {tri[2], c, {0, 0}}};
    SDL_RenderGeometry(p->ren, NULL, v, 3, NULL, 0);
  }
  stroke_poly(p, out, in, 12, fcolor(t->line_strong, alpha));
}

void gui_pad(const TouchLayout *l, uint32_t held, const GuiTheme *t) {
  if (!l->overlay) return;
  SDL_Renderer *ren = gui_renderer();
  if (!ren) return;
  // in portrait they have the space below the game to themselves; in landscape the picture may
  // run under them, so they let some of it through
  float alpha = l->portrait ? 1 : 0.75f;
  Pen p = {ren, (float)l->scale, fmaxf(1, roundf(1.5f * gui_display_scale()))};
  SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
  dpad(&p, l, t, held, alpha);
  for (int a = 0; a < ACTIONS; a++) {
    const UiRect *r = &l->button[a];
    if (!r->w) continue;
    bool lit = held >> a & 1, round = a == ACT_A || a == ACT_B || a == ACT_ITEM_X || a == ACT_ITEM_Y;
    SDL_FRect b = {r->x * p.s, r->y * p.s, r->w * p.s, r->h * p.s};
    shape(&p, b, round, lit ? fcolor(t->hi, 1) : fcolor(t->ink, alpha), lit ? fcolor(t->hi, 1) : fcolor(t->line_strong, alpha));
    float size = round ? b.w * 0.6f : b.h * 0.62f;
    gui_label(labels[a], b.x + b.w / 2, b.y + b.h / 2, size, lit ? t->on_hi : round ? t->paper : t->soft, (uint8_t)(lit ? 255 : 255 * alpha));
  }
}
