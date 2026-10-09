#include "platform/archive.h"
#include "platform/setup.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// Players keep their ROMs in .zip or .gz as often as loose, so both are unpacked here rather than
// asking them to extract first. DEFLATE is decoded in this file: the apps carry no library but SDL,
// and a ROM is small enough that a plain, obvious inflate is all this needs. 7z is a different
// format with its own compressor (LZMA), far more code than it would be worth, so it is reported
// as unsupported instead.

// A ROM is a megabyte; this only bounds what a corrupt or hostile archive can make us allocate.
#define MAX_OUT (32u * 1024 * 1024)

typedef struct {
  const uint8_t *in;
  size_t in_size, in_pos;
  uint32_t bits;
  int bit_count;
  uint8_t *out;
  size_t out_size, out_cap;
  bool failed;
} Inflate;

static int bit(Inflate *s) {
  if (!s->bit_count) {
    if (s->in_pos >= s->in_size) { s->failed = true; return 0; }
    s->bits = s->in[s->in_pos++];
    s->bit_count = 8;
  }
  int b = s->bits & 1;
  s->bits >>= 1;
  s->bit_count--;
  return b;
}

static uint32_t take(Inflate *s, int n) {
  uint32_t v = 0;
  for (int i = 0; i < n; i++) v |= (uint32_t)bit(s) << i;
  return v;
}

static bool emit(Inflate *s, uint8_t byte) {
  if (s->out_size == s->out_cap) {
    size_t cap = s->out_cap ? s->out_cap * 2 : 1 << 16;
    if (cap > MAX_OUT) cap = MAX_OUT;
    if (cap == s->out_cap) { s->failed = true; return false; }
    uint8_t *grown = realloc(s->out, cap);
    if (!grown) { s->failed = true; return false; }
    s->out = grown;
    s->out_cap = cap;
  }
  s->out[s->out_size++] = byte;
  return true;
}

// Canonical Huffman decoding: counts of codes per length, and the symbols in code order.
typedef struct { uint16_t count[16]; uint16_t symbol[288]; } Huffman;

static void huffman_build(Huffman *h, const uint8_t *lengths, int n) {
  memset(h->count, 0, sizeof h->count);
  for (int i = 0; i < n; i++) h->count[lengths[i]]++;
  h->count[0] = 0;
  uint16_t offset[16] = {0};
  for (int len = 1; len < 16; len++) offset[len] = (uint16_t)(offset[len - 1] + h->count[len - 1]);
  for (int i = 0; i < n; i++)
    if (lengths[i]) h->symbol[offset[lengths[i]]++] = (uint16_t)i;
}

static int huffman_decode(Inflate *s, const Huffman *h) {
  int code = 0, first = 0, index = 0;
  for (int len = 1; len < 16; len++) {
    code |= bit(s);
    if (s->failed) return -1;
    int count = h->count[len];
    if (code - first < count) return h->symbol[index + (code - first)];
    index += count;
    first = (first + count) << 1;
    code <<= 1;
  }
  s->failed = true;
  return -1;
}

static const uint16_t len_base[29] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};
static const uint8_t len_extra[29] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
static const uint16_t dist_base[30] = {1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};
static const uint8_t dist_extra[30] = {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};

static bool inflate_block(Inflate *s, const Huffman *lit, const Huffman *dist) {
  for (;;) {
    int sym = huffman_decode(s, lit);
    if (sym < 0) return false;
    if (sym == 256) return true;
    if (sym < 256) {
      if (!emit(s, (uint8_t)sym)) return false;
      continue;
    }
    sym -= 257;
    if (sym >= 29) { s->failed = true; return false; }
    size_t length = len_base[sym] + take(s, len_extra[sym]);
    int dsym = huffman_decode(s, dist);
    if (dsym < 0 || dsym >= 30) { s->failed = true; return false; }
    size_t back = dist_base[dsym] + take(s, dist_extra[dsym]);
    if (s->failed || back > s->out_size) { s->failed = true; return false; }
    for (size_t i = 0; i < length; i++)
      if (!emit(s, s->out[s->out_size - back])) return false;
  }
}

static void fixed_tables(Huffman *lit, Huffman *dist) {
  uint8_t lengths[288];
  for (int i = 0; i < 144; i++) lengths[i] = 8;
  for (int i = 144; i < 256; i++) lengths[i] = 9;
  for (int i = 256; i < 280; i++) lengths[i] = 7;
  for (int i = 280; i < 288; i++) lengths[i] = 8;
  huffman_build(lit, lengths, 288);
  for (int i = 0; i < 30; i++) lengths[i] = 5;
  huffman_build(dist, lengths, 30);
}

static bool dynamic_tables(Inflate *s, Huffman *lit, Huffman *dist) {
  static const uint8_t order[19] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
  int nlit = (int)take(s, 5) + 257, ndist = (int)take(s, 5) + 1, ncode = (int)take(s, 4) + 4;
  if (s->failed || nlit > 286 || ndist > 30) return false;
  uint8_t lengths[320] = {0};
  for (int i = 0; i < ncode; i++) lengths[order[i]] = (uint8_t)take(s, 3);
  if (s->failed) return false;
  Huffman code_lengths;
  huffman_build(&code_lengths, lengths, 19);
  memset(lengths, 0, sizeof lengths);
  for (int i = 0; i < nlit + ndist;) {
    int sym = huffman_decode(s, &code_lengths);
    if (sym < 0) return false;
    if (sym < 16) { lengths[i++] = (uint8_t)sym; continue; }
    int repeat, value = 0;
    if (sym == 16) {
      if (!i) return false;
      value = lengths[i - 1];
      repeat = 3 + (int)take(s, 2);
    } else if (sym == 17) {
      repeat = 3 + (int)take(s, 3);
    } else {
      repeat = 11 + (int)take(s, 7);
    }
    if (s->failed || i + repeat > nlit + ndist) return false;
    while (repeat--) lengths[i++] = (uint8_t)value;
  }
  huffman_build(lit, lengths, nlit);
  huffman_build(dist, lengths + nlit, ndist);
  return true;
}

// Raw DEFLATE (no zlib or gzip wrapper) into a fresh buffer.
static uint8_t *inflate(const uint8_t *in, size_t in_size, size_t *out_size) {
  Inflate s = {0};
  s.in = in;
  s.in_size = in_size;
  for (;;) {
    int last = bit(&s);
    int type = (int)take(&s, 2);
    if (s.failed) break;
    if (type == 0) {
      s.bit_count = 0;
      if (s.in_pos + 4 > s.in_size) { s.failed = true; break; }
      size_t n = s.in[s.in_pos] | ((size_t)s.in[s.in_pos + 1] << 8);
      s.in_pos += 4;
      if (s.in_pos + n > s.in_size) { s.failed = true; break; }
      for (size_t i = 0; i < n; i++)
        if (!emit(&s, s.in[s.in_pos + i])) break;
      s.in_pos += n;
    } else if (type == 1 || type == 2) {
      Huffman lit, dist;
      if (type == 1) fixed_tables(&lit, &dist);
      else if (!dynamic_tables(&s, &lit, &dist)) break;
      if (!inflate_block(&s, &lit, &dist)) break;
    } else {
      s.failed = true;
      break;
    }
    if (s.failed || last) break;
  }
  if (s.failed) { free(s.out); return NULL; }
  *out_size = s.out_size;
  return s.out ? s.out : calloc(1, 1);
}

static uint32_t le32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }
static uint16_t le16(const uint8_t *p) { return (uint16_t)(p[0] | (p[1] << 8)); }

static bool has_suffix(const char *s, const char *suffix) {
  size_t n = strlen(s), m = strlen(suffix);
  if (m > n) return false;
  for (size_t i = 0; i < m; i++) {
    char a = s[n - m + i], b = suffix[i];
    if (a >= 'A' && a <= 'Z') a = (char)(a - 'A' + 'a');
    if (a != b) return false;
  }
  return true;
}

static bool looks_like_rom(const char *name) { return has_suffix(name, ".gbc") || has_suffix(name, ".gb"); }

static uint8_t *copy_of(const uint8_t *p, size_t n) {
  uint8_t *out = malloc(n ? n : 1);
  if (out) memcpy(out, p, n);
  return out;
}

// Walks the central directory and takes the member that looks like a ROM, else the largest one -- a
// ROM zip usually holds the ROM plus a readme. A wrong guess is caught by the sha1 check that follows.
static uint8_t *read_zip(const uint8_t *zip, size_t size, size_t *out_size, char *err, size_t err_size) {
  size_t eocd = 0;
  bool found = false;
  for (size_t i = size < 22 ? 0 : size - 22;; i--) {
    if (i + 4 <= size && le32(zip + i) == 0x06054b50) { eocd = i; found = true; break; }
    if (!i) break;
  }
  if (!found) { snprintf(err, err_size, "NOT A VALID ZIP"); return NULL; }
  int count = le16(zip + eocd + 10);
  size_t dir = le32(zip + eocd + 16);
  size_t best_offset = 0, best_size = 0, best_compressed = 0;
  int best_method = -1;
  bool best_named = false;
  for (int i = 0; i < count && dir + 46 <= size; i++) {
    if (le32(zip + dir) != 0x02014b50) break;
    uint16_t method = le16(zip + dir + 10);
    uint32_t compressed = le32(zip + dir + 20), uncompressed = le32(zip + dir + 24);
    uint16_t name_len = le16(zip + dir + 28), extra_len = le16(zip + dir + 30), comment_len = le16(zip + dir + 32);
    uint32_t local = le32(zip + dir + 42);
    char name[256];
    size_t copy = name_len < sizeof name - 1 ? name_len : sizeof name - 1;
    if (dir + 46 + copy > size) break;
    memcpy(name, zip + dir + 46, copy);
    name[copy] = 0;
    bool named = looks_like_rom(name);
    // A named .gbc/.gb always wins; among unnamed members the biggest one is the best guess.
    if ((named && !best_named) || (named == best_named && uncompressed > best_size)) {
      best_named = named;
      best_size = uncompressed;
      best_compressed = compressed;
      best_offset = local;
      best_method = method;
    }
    dir += 46u + name_len + extra_len + comment_len;
  }
  if (best_method < 0) { snprintf(err, err_size, "ZIP IS EMPTY"); return NULL; }
  if (best_method != 0 && best_method != 8) { snprintf(err, err_size, "BAD ZIP METHOD"); return NULL; }
  if (best_size > MAX_OUT) { snprintf(err, err_size, "FILE TOO BIG"); return NULL; }
  // The local header repeats the name and extra fields, so the data starts past both.
  if (best_offset + 30 > size || le32(zip + best_offset) != 0x04034b50) { snprintf(err, err_size, "DAMAGED ZIP"); return NULL; }
  size_t data = best_offset + 30u + le16(zip + best_offset + 26) + le16(zip + best_offset + 28);
  if (data + best_compressed > size) { snprintf(err, err_size, "DAMAGED ZIP"); return NULL; }
  if (best_method == 0) {
    *out_size = best_compressed;
    uint8_t *out = copy_of(zip + data, best_compressed);
    if (!out) snprintf(err, err_size, "OUT OF MEMORY");
    return out;
  }
  uint8_t *out = inflate(zip + data, best_compressed, out_size);
  if (!out) snprintf(err, err_size, "CANNOT UNPACK ZIP");
  return out;
}

static uint8_t *read_gzip(const uint8_t *gz, size_t size, size_t *out_size, char *err, size_t err_size) {
  if (size < 18 || gz[0] != 0x1f || gz[1] != 0x8b || gz[2] != 8) { snprintf(err, err_size, "NOT A VALID GZ"); return NULL; }
  uint8_t flags = gz[3];
  size_t p = 10;
  if (flags & 4) {                                  // FEXTRA
    if (p + 2 > size) { snprintf(err, err_size, "DAMAGED GZ"); return NULL; }
    p += 2u + le16(gz + p);
  }
  if (flags & 8)                                    // FNAME
    while (p < size && gz[p++]) {}
  if (flags & 16)                                   // FCOMMENT
    while (p < size && gz[p++]) {}
  if (flags & 2) p += 2;                            // FHCRC
  if (p >= size) { snprintf(err, err_size, "DAMAGED GZ"); return NULL; }
  uint8_t *out = inflate(gz + p, size - p, out_size);
  if (!out) snprintf(err, err_size, "CANNOT UNPACK GZ");
  return out;
}

static const uint8_t sevenzip_magic[6] = {0x37, 0x7a, 0xbc, 0xaf, 0x27, 0x1c};

uint8_t *archive_rom_from_memory(const uint8_t *raw, size_t raw_size, size_t *out_size, char *err, size_t err_size) {
  *err = 0;
  // Trust the magic over the extension: a plain ROM named .zip, or the reverse, still works.
  bool zip = raw_size >= 4 && le32(raw) == 0x04034b50;
  bool gzip = raw_size >= 2 && raw[0] == 0x1f && raw[1] == 0x8b;
  if (raw_size >= sizeof sevenzip_magic && !memcmp(raw, sevenzip_magic, sizeof sevenzip_magic)) {
    snprintf(err, err_size, "7Z: UNZIP IT FIRST");
    return NULL;
  }
  if (!zip && !gzip) {
    *out_size = raw_size;
    return copy_of(raw, raw_size);
  }
  return zip ? read_zip(raw, raw_size, out_size, err, err_size) : read_gzip(raw, raw_size, out_size, err, err_size);
}

uint8_t *archive_read_rom(const char *path, size_t *size, char *err, size_t err_size) {
  *err = 0;
  size_t raw_size;
  uint8_t *raw = oracles_read_file(path, &raw_size);
  if (!raw) {
    // Nothing was read, so only the name is left to go on.
    snprintf(err, err_size, has_suffix(path, ".7z") ? "7Z: UNZIP IT FIRST" : "CANNOT READ FILE");
    return NULL;
  }
  uint8_t *out = archive_rom_from_memory(raw, raw_size, size, err, err_size);
  free(raw);
  return out;
}
