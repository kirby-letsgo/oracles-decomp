#include "game/game.h"
#include "game/features.h"
#include "game/vwf.h"
#include <string.h>

static uint8_t font[VWF_FONT_SIZE];
static uint8_t widths[VWF_WIDTHS_SIZE];
static bool loaded;

bool vwf_set_font(const uint8_t *f, size_t font_size, const uint8_t *w, size_t widths_size) {
  if (!f || !w || font_size != VWF_FONT_SIZE || widths_size != VWF_WIDTHS_SIZE) return false;
  memcpy(font, f, sizeof font);
  memcpy(widths, w, sizeof widths);
  loaded = true;
  return true;
}

bool vwf_font_loaded(void) { return loaded; }

// ---- re-wrapping ----

enum {
  TC_END = 0x00, TC_NEWLINE = 0x01, TC_SYMBOL = 0x06, TC_JUMP = 0x07, TC_COLOR = 0x09, TC_NAME = 0x0a,
  TC_MISC = 0x0c, TC_CALL = 0x0f,
  MISC_NUM1 = 1 << 3, MISC_OPT = 2 << 3, MISC_STOP = 3 << 3, MISC_POS = 4 << 3, MISC_HEARTPIECE = 5 << 3,
  MISC_NUM2 = 6 << 3,
  CH_HEART = 0x14, CH_UP = 0x15, CH_RIGHT = 0x18,
  MAX_LINE_WIDTH = 16 * 8 + 1,
  GROUP_INVENTORY = 0x09,
};

typedef struct { uint8_t c, p; } Tok;
#define MAX_TOKS 4096
#define MAX_LINES 512

static bool two_byte(uint8_t c) { return c >= 0x02 && c < 0x10; }
static bool is_misc(Tok t, uint8_t action) { return t.c == TC_MISC && (t.p & 0xf8) == action; }
static bool is_char(Tok t, uint8_t ch) { return t.c == ch; }
static bool is_lower(uint8_t c) { return c >= 'a' && c <= 'z'; }

typedef struct { const Tok *t; int n; } Line;

static Line strip_spaces(Line l) {
  while (l.n && is_char(l.t[0], ' ')) { l.t++; l.n--; }
  while (l.n && is_char(l.t[l.n - 1], ' ')) l.n--;
  return l;
}

static bool has_opt(Line l) {
  for (int i = 0; i < l.n; i++) if (is_misc(l.t[i], MISC_OPT)) return true;
  return false;
}

static bool only_command(Line l) { return l.n == 1 && (l.t[0].c == TC_CALL || l.t[0].c == TC_JUMP); }

static bool has_column_gap(Line l) {
  l = strip_spaces(l);
  for (int i = 0; i + 2 < l.n; i++)
    if (is_char(l.t[i], ' ') && is_char(l.t[i + 1], ' ') && is_char(l.t[i + 2], ' ')) return true;
  return false;
}

static bool dots_line(Line l, bool allow_stop) {
  int n = l.n;
  if (allow_stop && n && is_misc(l.t[n - 1], MISC_STOP)) n--;
  if (n < 4) return false;
  for (int i = 0; i < n; i++) if (!is_char(l.t[i], '.') && !is_char(l.t[i], ' ')) return false;
  return true;
}

// Where the ROM's line break is the author's, not just the fixed-width font running out
static bool hard_break(Line a, Line b, int index, uint8_t group) {
  if (a.n && (is_misc(a.t[a.n - 1], MISC_STOP) || is_misc(a.t[a.n - 1], MISC_HEARTPIECE))) return true;
  if (has_opt(a) || has_opt(b)) return true;
  if (only_command(a) || only_command(b)) return true;
  if (b.n && is_misc(b.t[0], MISC_POS)) return true;
  if (b.n && b.t[0].c >= CH_UP && b.t[0].c <= CH_RIGHT) return true;
  if (has_column_gap(a) || has_column_gap(b)) return true;
  if (dots_line(b, true) || dots_line(a, false)) return true;
  if (group == GROUP_INVENTORY && index == 0) return true;
  return false;
}

static int split_lines(const Tok *t, int n, Line *lines) {
  int count = 0, start = 0;
  for (int i = 0; i <= n && count < MAX_LINES; i++)
    if (i == n || t[i].c == TC_NEWLINE) { lines[count++] = (Line){t + start, i - start}; start = i + 1; }
  return count;
}

static int reflow(const Tok *in, int n, uint8_t group, Tok *out) {
  static Line lines[MAX_LINES];
  int count = split_lines(in, n, lines), m = 0;
  for (int i = 0; i < count; i++) {
    Line b = lines[i];
    if (i) {
      Line a = lines[i - 1];
      bool hard = group == 0x02 || group == 0x03 || hard_break(a, b, i - 1, group);
      if (hard) out[m++] = (Tok){TC_NEWLINE, 0};
      else {
        while (b.n && is_char(b.t[0], ' ')) { b.t++; b.n--; }
        while (m && is_char(out[m - 1], ' ')) m--;
        bool hyphen = m && is_char(out[m - 1], '-');
        if (hyphen && m > 1 && is_lower(out[m - 2].c) && b.n && is_lower(b.t[0].c)) m--;
        else if (hyphen && !(m > 1 && is_char(out[m - 2], '-'))) {}
        else out[m++] = (Tok){' ', 0};
      }
    }
    for (int k = 0; k < b.n && m < MAX_TOKS; k++) out[m++] = b.t[k];
  }
  return m;
}

static int name_width(const uint8_t *name) {
  int w = 0;
  for (int i = 0; name && i < 6 && name[i]; i++) w += widths[name[i]];
  return w;
}

static int floor_div8(int v) { return v >= 0 ? v / 8 : -((-v + 7) / 8); }

typedef struct {
  uint8_t *out;
  size_t n, cap;
  int width, width_to_space, color, tile_color;
  long last_space;
} Wrap;

static void put(Wrap *w, uint8_t b) { if (w->n < w->cap) w->out[w->n] = b; w->n++; }

static void add_width(Wrap *w, int px) {
  int old = w->width;
  w->width += px;
  if (w->width > MAX_LINE_WIDTH && w->last_space >= 0) {
    if ((size_t)w->last_space < w->cap) w->out[w->last_space] = TC_NEWLINE;
    w->last_space = -1;
    w->width -= w->width_to_space;
    w->tile_color = w->color;
  }
  if (floor_div8(old - 2) != w->width / 8) w->tile_color = w->color;
}

static bool colors_clash(int x, int y) { return x && y && x < 4 && y < 4 && x != y; }

// The branch's parseText.py --vwf pass over the joined text: wraps at the last space before 129px
// and keeps red and blue out of one 8x8 tile (one palette per tile).
static size_t wrap_tokens(const Tok *t, int n, const uint8_t *link_name, const uint8_t *kid_name, uint8_t *out, size_t cap) {
  Wrap w = {out, 0, cap, 0, 0, 4, 4, -1};
  int space = widths[' '];
  for (int i = 0; i < n; i++) {
    Tok k = t[i];
    if (k.c == TC_NEWLINE) {
      put(&w, k.c);
      w.width = 0;
      w.last_space = -1;
      w.tile_color = w.color;
    } else if (k.c >= 0x10) {
      if (k.c == CH_HEART && w.color >= 2) { put(&w, TC_COLOR); put(&w, 0); w.color = 0; }
      if (k.c == ' ') { w.last_space = (long)w.n; w.width_to_space = w.width + space; }
      put(&w, k.c);
      add_width(&w, widths[k.c]);
    } else if (k.c == TC_NAME) {
      put(&w, k.c); put(&w, k.p);
      add_width(&w, k.p == 0 ? name_width(link_name) : k.p == 1 ? name_width(kid_name) : 8 * 5);
    } else if (k.c == TC_MISC) {
      put(&w, k.c); put(&w, k.p);
      uint8_t a = k.p & 0xf8;
      if (a == MISC_OPT) add_width(&w, 8);
      else if (a == MISC_NUM1 || a == MISC_NUM2 || a == MISC_HEARTPIECE) add_width(&w, 16);
    } else if (k.c == TC_SYMBOL && (k.p & 0x80)) {
      if (w.n >= 3 && w.n <= w.cap && out[w.n - 3] == ' ' && out[w.n - 2] == TC_COLOR && out[w.n - 1] >= 0x80 &&
          w.width >= space && (w.width & 7) < 4) {
        out[w.n - 3] = out[w.n - 2];
        out[w.n - 2] = out[w.n - 1];
        w.n--;
        w.width -= space;
        if (w.last_space >= (long)w.n) w.last_space = -1;
      }
      if (w.width & 7) w.width = (w.width & ~7) + 8;
      add_width(&w, 8);
      put(&w, k.c); put(&w, k.p);
    } else if (k.c == TC_SYMBOL) {
      put(&w, k.c); put(&w, k.p);
      add_width(&w, 8);
    } else if (k.c == TC_COLOR) {
      int p = k.p;
      if (colors_clash(w.tile_color, p)) { add_width(&w, space); put(&w, ' '); }
      if (w.color == 3 && p == 0) p = 4;
      put(&w, k.c); put(&w, (uint8_t)p);
      w.color = p;
    } else {
      put(&w, k.c);
      if (two_byte(k.c)) put(&w, k.p);
    }
  }
  put(&w, TC_END);
  return w.n;
}

// Area and dungeon titles keep their lines; leading spaces that centred them in 8px characters are
// recounted so the line keeps its centre.
static int recentre(const Tok *in, int n, Tok *out) {
  static Line lines[MAX_LINES];
  int count = split_lines(in, n, lines), m = 0, space = widths[' '] ? widths[' '] : 1;
  for (int i = 0; i < count; i++) {
    Line l = lines[i];
    if (i) out[m++] = (Tok){TC_NEWLINE, 0};
    int lead = 0;
    while (lead < l.n && is_char(l.t[lead], ' ')) lead++;
    if (!lead) { for (int k = 0; k < l.n; k++) out[m++] = l.t[k]; continue; }
    int chars = 0, px = 0;
    for (int k = lead; k < l.n; k++)
      if (l.t[k].c >= 0x10) { chars++; px += widths[l.t[k].c]; }
    int centre = lead * 8 + chars * 4, pad = (centre - px / 2 + space / 2) / space;
    for (int k = 0; k < pad && m < MAX_TOKS; k++) out[m++] = (Tok){' ', 0};
    for (int k = lead; k < l.n; k++) out[m++] = l.t[k];
  }
  return m;
}

static int tokenize(const uint8_t *in, size_t n, Tok *t) {
  int m = 0;
  for (size_t i = 0; i < n && m < MAX_TOKS; i++) {
    uint8_t c = in[i];
    if (c == TC_END) break;
    Tok k = {c, 0};
    if (two_byte(c) && i + 1 < n) k.p = in[++i];
    t[m++] = k;
  }
  return m;
}

size_t vwf_wrap(const uint8_t *in, size_t n, uint8_t group, const uint8_t *link_name, const uint8_t *kid_name,
                uint8_t *out, size_t cap) {
  static Tok toks[MAX_TOKS], joined[MAX_TOKS * 2];
  int count = tokenize(in, n, toks);
  int m = group == 0x02 || group == 0x03 ? recentre(toks, count, joined) : reflow(toks, count, group, joined);
  return wrap_tokens(joined, m, link_name, kid_name, out, cap);
}

// ---- text lookup ----

#define SLOTS 8
#define SLOT_SIZE 0x1000
static uint8_t slot_text[SLOTS][SLOT_SIZE];
static int next_slot;

bool vwf_is_text_bank(uint8_t bank) { return bank >= VWF_BANK && bank < VWF_BANK + SLOTS; }

uint8_t vwf_text_byte(uint8_t bank, uint16_t addr) {
  if (!vwf_is_text_bank(bank) || addr < 0x4000 || addr >= 0x4000 + SLOT_SIZE) return TC_END;
  return slot_text[bank - VWF_BANK][addr - 0x4000];
}

static uint8_t rom_byte(GB *gb, uint8_t bank, uint16_t addr) {
  size_t at = addr < 0x4000 ? addr : (size_t)bank * 0x4000 + (addr & 0x3fff);
  return at < gb->rom_size ? gb->rom[at] : 0;
}

static uint8_t table_byte(GB *gb, uint16_t *addr) {
  uint8_t bank = W8(w7TextTableBank);
  if (*addr & 0x8000) { *addr = (uint16_t)((*addr & 0x7fff) | 0x4000); bank++; }
  return rom_byte(gb, bank, (*addr)++);
}

// TEXT_OFFSET_SPLIT_INDEX: the operand of getTextAddress's "ld a,(wTextIndexH) / cp n"
static uint8_t split_index(GB *gb) {
  static uint8_t cached;
  if (cached) return cached;
  uint16_t at = SYM(getTextAddress);
  for (int i = 0; i < 0x60; i++)
    if (gb->rom[at + i] == 0xfa && gb->rom[at + i + 1] == (wTextIndexH & 0xff) && gb->rom[at + i + 2] == wTextIndexH >> 8 &&
        gb->rom[at + i + 3] == 0xfe)
      return cached = gb->rom[at + i + 4];
  return cached = 0x2c;
}

// getTextAddress in C, for the dictionary entries inside a text
static void text_address(GB *gb, uint8_t index_h, uint8_t index_l, uint8_t *bank, uint16_t *addr) {
  uint16_t table = (uint16_t)(W8(w7TextTableAddr) | WP(w7TextTableAddr)[1] << 8);
  uint16_t hl = (uint16_t)(table + index_h * 2);
  uint16_t group = table_byte(gb, &hl);
  group |= (uint16_t)(table_byte(gb, &hl) << 8);
  hl = (uint16_t)(table + group + index_l * 2);
  uint16_t offset = table_byte(gb, &hl);
  offset |= (uint16_t)(table_byte(gb, &hl) << 8);
  uint16_t entry = (uint16_t)((index_h < split_index(gb) ? SYM(textOffset1Table) : SYM(textOffset2Table)) + W8(wActiveLanguage) * 4);
  uint8_t tb = (uint8_t)SYMBANK(textOffset1Table);
  uint8_t e = rom_byte(gb, tb, entry);
  uint32_t sum = (uint32_t)(rom_byte(gb, tb, (uint16_t)(entry + 1)) | rom_byte(gb, tb, (uint16_t)(entry + 2)) << 8) + offset;
  *bank = sum >= 0x10000 ? (uint8_t)(e + 4) : (uint8_t)(e + ((sum >> 14) & 3));
  *addr = (uint16_t)((sum & 0x3fff) | 0x4000);
}

static size_t expand(GB *gb, uint8_t bank, uint16_t addr, uint8_t *out, size_t n, size_t cap, int depth) {
  for (int guard = 0; guard < SLOT_SIZE && n < cap; guard++) {
    uint8_t c = rom_byte(gb, bank, addr);
    if (++addr == 0x8000) { addr = 0x4000; bank++; }
    if (c == TC_END) break;
    if (!two_byte(c)) { out[n++] = c; continue; }
    uint8_t p = rom_byte(gb, bank, addr);
    if (++addr == 0x8000) { addr = 0x4000; bank++; }
    if (c >= 0x02 && c <= 0x05 && depth < 4) {
      uint8_t b2;
      uint16_t a2;
      text_address(gb, (uint8_t)(c - 0x02), p, &b2, &a2);
      n = expand(gb, b2, a2, out, n, cap, depth + 1);
      continue;
    }
    out[n++] = c;
    if (n < cap) out[n++] = p;
    if (c == TC_JUMP) break;
  }
  return n;
}

uint16_t vwf_redirect_text(GB *gb, uint16_t addr) {
  uint8_t index_h = W8(wTextIndexH);
  if (!features.vwf || index_h < 4) return addr;
  uint8_t group = (uint8_t)(index_h - 4);
  if (group == GROUP_INVENTORY) return addr;
  static uint8_t raw[SLOT_SIZE];
  size_t n = expand(gb, W8(w7ActiveBank), addr, raw, 0, sizeof raw, 0);
  int slot = next_slot;
  next_slot = (next_slot + 1) % SLOTS;
  size_t m = vwf_wrap(raw, n, group, WP(wLinkName), WP(wKidName), slot_text[slot], SLOT_SIZE - 1);
  if (m >= SLOT_SIZE) slot_text[slot][SLOT_SIZE - 1] = TC_END;
  W8(w7ActiveBank) = (uint8_t)(VWF_BANK + slot);
  return 0x4000;
}

// ---- drawing ----

static uint8_t tmp[32];
static uint8_t buf_pos, char_off, char_index, unfinished;

static uint8_t *gfx_buffer(GB *gb) { return WP(w7TextGfxBuffer); }

// setLineTextBuffers without clearing w7SoundEffect (the branch clears it on each tile instead)
static void set_line_buffers(GB *gb, uint8_t pos, uint8_t ch) {
  uint8_t *line = WP(w7LineTextBuffer) + pos, *attr = WP(w7TextAttribute);
  line[0x00] = ch;
  line[0x10] = attr[0];
  line[0x20] = attr[-2];
  line[0x30] = attr[-3];
  line[0x40] = attr[-4];
}

static void glyph(GB *gb, uint8_t ch, uint8_t *out) {
  uint8_t source = W8(w7TextGfxSource);
  const uint8_t *src = NULL;
  uint8_t rom_tile[32];
  if (source == 0) src = font + ch * 16;
  else {
    uint16_t base = (uint16_t)(rom_byte(gb, 0, (uint16_t)(textGfxSourceTable + source * 2)) |
                               rom_byte(gb, 0, (uint16_t)(textGfxSourceTable + source * 2 + 1)) << 8);
    for (int i = 0; i < 32; i++) rom_tile[i] = rom_byte(gb, 0x1c, (uint16_t)(base + ch * 16 + i));
    src = rom_tile;
  }
  uint8_t mode = W8(wTextGfxColorIndex) & 0x0f;
  if (source == 0 && ch == CH_HEART) mode = 1;
  if (mode == 3 && source != 0) {
    memcpy(out, src, 32);
    W8(wTextGfxColorIndex) = (uint8_t)(W8(wTextGfxColorIndex) >> 4);
  } else
    for (int r = 0; r < 16; r++) {
      out[r * 2] = mode == 1 ? 0xff : src[r];
      out[r * 2 + 1] = mode == 2 ? 0xff : src[r];
    }
  W8(w7TextGfxSource) = 0;
}

// _addShiftedCharacter: ANDs the glyph, shifted right with 1s (background) coming in, into a tile
static void add_shifted(uint8_t *dst, const uint8_t *src, int shift) {
  for (int i = 0; i < 32; i++) dst[i] &= (uint8_t)(shift ? (src[i] >> shift) | (0xff << (8 - shift)) : src[i]);
}

// _drawShiftedCharacter: the glyph's remainder, shifted left, at the start of the next tile
static void draw_shifted(uint8_t *dst, const uint8_t *src, int shift) {
  for (int i = 0; i < 32; i++) dst[i] = (uint8_t)(shift ? (src[i] << shift) | ((1 << shift) - 1) : src[i]);
}

static void place(GB *gb, int width) {
  if (buf_pos >= 0x10) return;
  uint8_t *dst = gfx_buffer(gb) + buf_pos * 32;
  add_shifted(dst, tmp, char_off);
  set_line_buffers(gb, buf_pos, char_index);
  int room = 8 - char_off;
  if (width > room) {
    if (buf_pos + 1 >= 0x10) return;
    set_line_buffers(gb, (uint8_t)(buf_pos + 1), char_index);
    draw_shifted(dst + 32, tmp, room);
  }
  int off = char_off + width;
  if (off >= 8) {
    off -= 8;
    buf_pos++;
    W8(w7SoundEffect) = 0;
  }
  char_off = (uint8_t)off;
}

void vwf_begin_line(GB *gb) {
  buf_pos = char_off = 0;
  W8(w7SoundEffect) = 0;
}

void vwf_add_char(GB *gb, uint8_t ch) {
  char_index = ch;
  glyph(gb, ch, tmp);
  place(gb, widths[ch]);
}

void vwf_add_symbol(GB *gb, uint8_t ch) {
  if (W8(w7TextGfxSource) == 2 && char_off) { char_off = 0; buf_pos++; }
  char_index = 0x06;
  glyph(gb, ch, tmp);
  place(gb, 8);
}

uint8_t vwf_option_position(GB *gb) {
  char_off = 0;
  set_line_buffers(gb, buf_pos, char_index);
  return buf_pos++;
}

uint8_t vwf_width(uint8_t ch) { return widths[ch]; }

uint8_t vwf_inventory_centre(GB *gb, int name_px) {
  (void)gb;
  char_off = 0;
  int px = name_px + 6;
  buf_pos = (uint8_t)(8 - (px >> 4));
  return (uint8_t)(0x11 - ((px + 2) >> 3) - buf_pos);
}

void vwf_inventory_reset(void) { char_off = unfinished = 0; }

static uint8_t *last_tile(GB *gb) { return gfx_buffer(gb) + 0x1e0; }

void vwf_inventory_begin_tile(GB *gb) {
  memset(last_tile(gb), 0xff, 32);
  if (!unfinished) return;
  glyph(gb, unfinished, tmp);
  draw_shifted(last_tile(gb), tmp, widths[unfinished] - char_off);
  unfinished = 0;
}

bool vwf_inventory_add(GB *gb, uint8_t ch) {
  char_index = ch;
  glyph(gb, ch, tmp);
  add_shifted(last_tile(gb), tmp, char_off);
  int off = char_off + widths[ch];
  if (off < 8) { char_off = (uint8_t)off; return false; }
  char_off = (uint8_t)(off - 8);
  if (char_off) unfinished = ch;
  return true;
}
