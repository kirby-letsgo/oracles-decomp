#include "unit.h"
#include "game/vwf.h"

// Every character 6px wide and a 4px space, so 129px holds 21 characters
static void load_font(void) {
  static unsigned char font[VWF_FONT_SIZE], widths[VWF_WIDTHS_SIZE];
  memset(widths, 6, sizeof widths);
  widths[' '] = 4;
  ASSERT(vwf_set_font(font, sizeof font, widths, sizeof widths));
}

static const unsigned char no_name[] = "Link";

// '\n' stands for the newline byte; want includes the terminating 0
static void expect_n(const char *text, size_t len, unsigned char group, const char *want, size_t want_len) {
  unsigned char in[512], out[512];
  for (size_t i = 0; i < len; i++) in[i] = text[i] == '\n' ? 0x01 : (unsigned char)text[i];
  size_t n = vwf_wrap(in, len, group, no_name, no_name, out, sizeof out);
  for (size_t i = 0; i < n; i++) if (out[i] == 0x01) out[i] = '\n';
  if (n != want_len || memcmp(out, want, n)) {
    fprintf(stderr, "wrap of \"%.*s\"\n got \"%.*s\"\nwant \"%.*s\"\n", (int)len, text, (int)n, out, (int)want_len, want);
    exit(1);
  }
}
#define expect(text, group, want) expect_n(text, sizeof(text) - 1, group, want, sizeof(want))

static void soft_breaks_are_joined(void) {
  expect("You got\nthe item!", 0x10, "You got the item!");
  expect("Have you\nseen it?", 0x10, "Have you seen it?");
}

static void a_split_word_is_rejoined(void) {
  expect("some-\nthing", 0x10, "something");
  expect("Jabu-\nJabu", 0x10, "Jabu-Jabu");
  expect("Wait--\nno", 0x10, "Wait-- no");
}

static void author_breaks_are_kept(void) {
  expect("Hi!\x0c\x18\nBye", 0x10, "Hi!\x0c\x18\nBye");
  expect("Pay?\n \x0c\x10Yes \x0c\x10No", 0x10, "Pay?\n \x0c\x10Yes \x0c\x10No");
  expect("Fairy   100\nBlue     30", 0x10, "Fairy   100\nBlue     30");
  expect("Ah!\n........", 0x10, "Ah!\n........");
  expect("Bomb\nBlows things up.", 0x09, "Bomb\nBlows things up.");
}

static void long_lines_wrap_at_the_last_space(void) {
  expect("aaaa bbbb cccc dddd eeee", 0x10, "aaaa bbbb cccc dddd\neeee");
}

static void red_and_blue_never_share_a_tile(void) {
  expect("\x09\x02" "a" "\x09\x03" "b", 0x10, "\x09\x02" "a " "\x09\x03" "b");
}

static void white_after_blue_is_colour_4(void) {
  expect("\x09\x03" "a" "\x09" "\x00" "b", 0x10, "\x09\x03" "a" "\x09\x04" "b");
}

int main(void) {
  load_font();
  RUN(soft_breaks_are_joined);
  RUN(a_split_word_is_rejoined);
  RUN(author_breaks_are_kept);
  RUN(long_lines_wrap_at_the_last_space);
  RUN(red_and_blue_never_share_a_tile);
  RUN(white_after_blue_is_colour_4);
  return 0;
}
