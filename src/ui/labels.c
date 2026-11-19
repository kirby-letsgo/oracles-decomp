#include "ui/labels.h"
#include <stdbool.h>
#include <stddef.h>

const char *const pad_style_keys[STYLES] = {"auto", "nintendo", "xbox", "playstation"};

// SDL gamepad buttons by position: SOUTH 0, EAST 1, WEST 2, NORTH 3, BACK 4, GUIDE 5, START 6,
// LEFT_STICK 7, RIGHT_STICK 8, LEFT_SHOULDER 9, RIGHT_SHOULDER 10.
enum { PAD_SOUTH, PAD_EAST, PAD_WEST, PAD_NORTH, PAD_BACK, PAD_GUIDE, PAD_START, PAD_LSTICK, PAD_RSTICK, PAD_LB, PAD_RB, PAD_NAMED, PAD_FACES = PAD_NORTH + 1 };

PadStyle pad_style_resolve(PadStyle setting, PadStyle detected) {
  PadStyle s = setting == STYLE_AUTO ? detected : setting;
  return s == STYLE_AUTO ? STYLE_XBOX : s;
}

void menu_hint_keys(InputDevice d, PadStyle resolved, const char **ok, const char **back) {
  if (d != DEVICE_PAD) { *ok = "Enter"; *back = "Esc"; }
  else if (resolved == STYLE_PLAYSTATION) { *ok = "Cross"; *back = "Circle"; }
  else { *ok = "A"; *back = "B"; }     // Xbox's SOUTH and EAST; Nintendo keeps the Game Boy names
}

const char *pad_style_name(PadStyle resolved, int button) {
  static const char *const xbox[PAD_NAMED] = {"A", "B", "X", "Y", "VIEW", "GUIDE", "MENU", "LS", "RS", "LB", "RB"};
  static const char *const ps[PAD_NAMED] = {"CROSS", "CIRCLE", "SQUARE", "TRIANGLE", "SHARE", "PS", "OPTIONS", "L3", "R3", "L1", "R1"};
  static const char *const stick_names[] = {"LS LEFT", "LS RIGHT", "LS UP", "LS DOWN",
                                           "RS LEFT", "RS RIGHT", "RS UP", "RS DOWN"};
  if (button >= PAD_LS_LEFT && button <= PAD_RS_DOWN) return stick_names[button - PAD_LS_LEFT];
  if (resolved == STYLE_NINTENDO) return NULL;
  bool p = resolved == STYLE_PLAYSTATION;
  if (button == PAD_RIGHT_TRIGGER) return p ? "R2" : "RT";
  if (button == PAD_LEFT_TRIGGER) return p ? "L2" : "LT";
  if (button < 0 || button >= PAD_NAMED) return NULL;
  return p ? ps[button] : xbox[button];
}

static const Action slot_action[HUD_SLOTS] = {ACT_ITEM_X, ACT_ITEM_Y, ACT_B, ACT_A};
static const char slot_letter[HUD_SLOTS] = {'X', 'Y', 'B', 'A'};

// SDL scancodes: A-Z 4-29, 1-9 30-38, 0 39.
static uint8_t key_glyph(int sc) {
  if (sc >= 4 && sc <= 29) return (uint8_t)('A' + sc - 4);
  if (sc >= 30 && sc <= 38) return (uint8_t)('1' + sc - 30);
  if (sc == 39) return '0';
  return 0;
}

static uint8_t pad_glyph(PadStyle resolved, int button) {
  static const uint8_t xbox[PAD_FACES] = {'A', 'B', 'X', 'Y'};
  static const uint8_t ps[PAD_FACES] = {HUD_GLYPH_CROSS, HUD_GLYPH_CIRCLE, HUD_GLYPH_SQUARE, HUD_GLYPH_TRIANGLE};
  if (button < PAD_SOUTH || button >= PAD_FACES) return 0;  // only the face buttons fit the bar
  return resolved == STYLE_PLAYSTATION ? ps[button] : xbox[button];
}

uint8_t hud_slot_glyph(InputDevice d, PadStyle resolved, const Bindings *b, HudSlot slot) {
  Action a = slot_action[slot];
  uint8_t g = 0;
  if (d == DEVICE_KEYBOARD) g = key_glyph(b->key[a][0]);
  else if (d == DEVICE_PAD && resolved != STYLE_NINTENDO) g = pad_glyph(resolved, b->pad[a]);
  return g ? g : (uint8_t)slot_letter[slot];
}
