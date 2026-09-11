#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x12, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x12, (from), (to), true)

static void underwater_add_double_index_to_hl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void checkLinkCanSurface_isUnderwater_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x78e4, 0x78e7); A = W8(wActiveGroup);
  CYC(0x78e7, 0x78ea); SET_HL(0x795e);
  CYC(0x78ea, 0x78eb); underwater_add_double_index_to_hl_from_rst(gb, 0x78eb);
  CYC(0x78eb, 0x78ec); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x78ec, 0x78ed); H = mem_rd(gb, HL);
  CYC(0x78ed, 0x78ee); L = A;
  CYC(0x78ee, 0x78f1); A = W8(wActiveRoom);
  CYC(0x78f1, 0x78f2); B = A;

  for (;;) {
    CYC(0x78f2, 0x78f3); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x78f3, 0x78f4); alu_or(gb, A);
    if (F & FZ) {
      CYCT(0x78f4, 0x78f6);
      goto no_room_entry;
    }
    CYC(0x78f4, 0x78f6);
    CYC(0x78f6, 0x78f7); alu_cp(gb, B);
    if (F & FZ) {
      CYCT(0x78f7, 0x78f9);
      break;
    }
    CYC(0x78f7, 0x78f9);
    CYC(0x78f9, 0x78fa); SET_HL(HL + 1);
    CYC(0x78fa, 0x78fb); SET_HL(HL + 1);
    CYC(0x78fb, 0x78fd);
  }

  CYC(0x78fd, 0x78fe); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x78fe, 0x78ff); H = mem_rd(gb, HL);
  CYC(0x78ff, 0x7900); L = A;
  CYC(0x7900, 0x7903); A = W8(wTilesetFlags);
  CYC(0x7903, 0x7905); alu_and(gb, 0x01);
  if (F & FZ) {
    CYCT(0x7905, 0x7907);
    goto check_jabu;
  }
  CYC(0x7905, 0x7907);
  CYC(0x7907, 0x7908); B = mem_rd(gb, HL);
  CYC(0x7908, 0x7909); A = B;
  CYC(0x7909, 0x790b); alu_and(gb, 0x03);
  if (F & FZ) {
    CYCT(0x790b, 0x790d);
    goto check_tile;
  }
  CYC(0x790b, 0x790d);
  CYC(0x790d, 0x790e); push_effect(gb, HL);
  CYC(0x790e, 0x7910); A = 0x30;
  CALL_C(0x7910, checkGlobalFlag_hook, 0x31f3, 0x7913);
  CYC(0x7913, 0x7914); SET_HL(pop_effect(gb));
  if (F & FZ) {
    CYCT(0x7914, 0x7916);
    goto check_tile;
  }
  CYC(0x7914, 0x7916);
  CYC(0x7916, 0x7918); alu_bit(gb, 0, B);
  if (!(F & FZ)) {
    CYCT(0x7918, 0x791a);
    goto no_room_entry;
  }
  CYC(0x7918, 0x791a);
  CYC(0x791a, 0x791c); A = 0x08;
  CYC(0x791c, 0x791d); underwater_add_double_index_to_hl_from_rst(gb, 0x791d);
  CYC(0x791d, 0x791f);
  goto check_tile;

check_jabu:
  CYC(0x791f, 0x7922); A = W8(wDungeonIndex);
  CYC(0x7922, 0x7924); alu_cp(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(0x7924, 0x7926);
    goto check_tile;
  }
  CYC(0x7924, 0x7926);
  CYC(0x7926, 0x7929); A = W8(wJabuWaterLevel);
  CYC(0x7929, 0x792b); alu_and(gb, 0x03);
  CYC(0x792b, 0x792d); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(0x792d, 0x792f);
    goto check_tile;
  }
  CYC(0x792d, 0x792f);
  CYC(0x792f, 0x7932); A = W8(wActiveRoom);
  CYC(0x7932, 0x7934); alu_cp(gb, 0x4c);
  if (F & FZ) {
    CYCT(0x7934, 0x7936);
    goto jabu_adjustment;
  }
  CYC(0x7934, 0x7936);
  CYC(0x7936, 0x7938); alu_cp(gb, 0x4d);
  if (!(F & FZ)) {
    CYCT(0x7938, 0x793a);
    goto check_tile;
  }
  CYC(0x7938, 0x793a);

jabu_adjustment:
  CYC(0x793a, 0x793c); A = 0x0b;
  CYC(0x793c, 0x793d); underwater_add_double_index_to_hl_from_rst(gb, 0x793d);

check_tile:
  CYC(0x793d, 0x7940); A = W8(wActiveTilePos);
  CYC(0x7940, 0x7941); B = A;
  CYC(0x7941, 0x7943); A = alu_swap(gb, A);
  CYC(0x7943, 0x7945); alu_and(gb, 0x0f);
  CYC(0x7945, 0x7946); underwater_add_double_index_to_hl_from_rst(gb, 0x7946);
  CYC(0x7946, 0x7947); A = B;
  CYC(0x7947, 0x7949); alu_and(gb, 0x0f);
  CYC(0x7949, 0x794b); alu_xor(gb, 0x0f);
  CALL_C(0x794b, checkFlag_hook, 0x0205, 0x794e);
  if (!(F & FZ)) {
    CYCT(0x794e, 0x7950);
    goto done;
  }
  CYC(0x794e, 0x7950);
  CYC(0x7950, 0x7951); alu_scf(gb);
  CYC(0x7951, 0x7953);
  goto done;

no_room_entry:
  CYC(0x7953, 0x7956); A = W8(wTilesetFlags);
  CYC(0x7956, 0x7958); alu_and(gb, 0x01);
  if (F & FZ) {
    CYCT(0x7958, 0x795a);
    goto done;
  }
  CYC(0x7958, 0x795a);
  CYC(0x795a, 0x795b); alu_scf(gb);

done:
  CYC(0x795b, 0x795d); C = alu_rl(gb, C);
  CYC(0x795d, 0x795e); ret_effect(gb);
}
