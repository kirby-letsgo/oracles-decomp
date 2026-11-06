#pragma once
// Variable-width font, after Drenn's vwf branch of oracles-disasm (font by Toruzz). Text is
// re-wrapped when it is looked up: the ROM's line breaks are joined where they only wrapped the
// fixed-width font, then the text is wrapped again by pixel width. The re-wrapped text lives in
// slots read through fake ROM banks VWF_BANK and up; characters are drawn shifted into the line's
// tiles instead of one per tile. Only active with features.vwf.
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define VWF_FONT_SIZE 4096
#define VWF_WIDTHS_SIZE 256
#define VWF_BANK 0x80

struct GB;

// font: 256 1bpp 8x16 glyphs (0 is ink), widths: one byte per character including its spacing
bool vwf_set_font(const uint8_t *font, size_t font_size, const uint8_t *widths, size_t widths_size);
bool vwf_font_loaded(void);

// After getTextAddress: points w7ActiveBank at a re-wrapped copy of the text at addr; returns
// the copy's address (addr itself when the text is left alone)
uint16_t vwf_redirect_text(struct GB *gb, uint16_t addr);
bool vwf_is_text_bank(uint8_t bank);
uint8_t vwf_text_byte(uint8_t bank, uint16_t addr);

// drawLineOfText's state, and the character drawing that replaces setLineTextBuffers +
// retrieveTextCharacter
void vwf_begin_line(struct GB *gb);
void vwf_add_char(struct GB *gb, uint8_t ch);
void vwf_add_symbol(struct GB *gb, uint8_t ch);
uint8_t vwf_option_position(struct GB *gb);

// The inventory's scrolling description line (the branch's _inventoryText*Hook)
uint8_t vwf_width(uint8_t ch);
// Centres the item name: sets the first tile from its width, returns w7InvTextSpacesAfterName
uint8_t vwf_inventory_centre(struct GB *gb, int name_px);
void vwf_inventory_reset(void);
// Clears the line's last tile and draws the remainder of a character split over the previous one
void vwf_inventory_begin_tile(struct GB *gb);
// Draws a character into the last tile; true once the tile is full
bool vwf_inventory_add(struct GB *gb, uint8_t ch);

// Text wrapping on its own, for tests: in/out are encoded text (dictionary already expanded)
size_t vwf_wrap(const uint8_t *in, size_t n, uint8_t group, const uint8_t *link_name, const uint8_t *kid_name,
                uint8_t *out, size_t cap);
