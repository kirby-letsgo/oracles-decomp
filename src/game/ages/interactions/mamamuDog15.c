#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void writeFlagsTocddb_hook(GB *gb);

// ref/oracles-disasm/scripts/ages/scriptHelper.s (INTERAC_MAMAMU_DOG), bank 0x15.

void mamamuDog_hop_hook(GB *gb);
void mamamuDog_setZPositionTo0_hook(GB *gb);

static void mamamuDog15_addAToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
    return;
  }
  burn_rom(gb, 0x00, 0x0012, 0x0013, false);
  burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
  burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
}

// Reverse direction if x-position gets too high or low.
void mamamuDog_checkReverseDirection_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5e94, objectApplySpeed_hook, 0x201d, 0x5e97);
  CYC(0x5e97, 0x5e99); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x5e99, 0x5e9a); A = mem_rd(gb, DE);
  CYC(0x5e9a, 0x5e9c); alu_sub(gb, 0x18);
  CYC(0x5e9c, 0x5e9e); alu_cp(gb, 0x70);
  if (F & FC) { RET_TAKEN(0x5e9e); return; }
  CYC(0x5e9e, 0x5e9f);
  CYC(0x5e9f, 0x5ea0); H = D;
  CYC(0x5ea0, 0x5ea2); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x5ea2, 0x5ea3); A = mem_rd(gb, HL);
  CYC(0x5ea3, 0x5ea5); alu_xor(gb, 0x10);
  CYC(0x5ea5, 0x5ea6); mem_wr(gb, HL, A);
  CYC(0x5ea6, 0x5ea8); B = 0x01;
  CYC(0x5ea8, 0x5eaa); // jr $5eac -- shared tail duplicated below (matches mamamuDog_reverseDirection)
  CYC(0x5eac, 0x5ead); H = D;
  CYC(0x5ead, 0x5eaf); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x5eaf, 0x5eb0); A = mem_rd(gb, HL);
  CYC(0x5eb0, 0x5eb1); alu_xor(gb, B);
  CYC(0x5eb1, 0x5eb2); mem_wr(gb, HL, A);
  CYC(0x5eb2, 0x5eb5); interactionSetAnimation_hook(gb); return; // jp
}

void mamamuDog_reverseDirection_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x5eaa, 0x5eac); B = 0x02;
  CYC(0x5eac, 0x5ead); H = D;
  CYC(0x5ead, 0x5eaf); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x5eaf, 0x5eb0); A = mem_rd(gb, HL);
  CYC(0x5eb0, 0x5eb1); alu_xor(gb, B);
  CYC(0x5eb1, 0x5eb2); mem_wr(gb, HL, A);
  CYC(0x5eb2, 0x5eb5); interactionSetAnimation_hook(gb); return; // jp
}

void mamamuDog_setCounterRandomly_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5eb5, getRandomNumber_hook, 0x043e, 0x5eb8);
  CYC(0x5eb8, 0x5eba); alu_and(gb, 0x07);
  CYC(0x5eba, 0x5ebd); SET_HL(0x5ecc); // mamamuDog_randomCounterValues
  CYC(0x5ebd, 0x5ebe); mamamuDog15_addAToHl_from_rst(gb, 0x5ebe);
  CYC(0x5ebe, 0x5ebf); A = mem_rd(gb, HL);
  CYC(0x5ebf, 0x5ec1); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x5ec1, 0x5ec2); mem_wr(gb, DE, A);
  CALL_C(0x5ec2, mamamuDog_hop_hook, 0x5eda, 0x5ec5);
  mamamuDog_setZPositionTo0_hook(gb); return; // fallthrough
}

void mamamuDog_setZPositionTo0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x5ec5, 0x5ec6); H = D;
  CYC(0x5ec6, 0x5ec8); L = INTERACTION_BASE + OBJ_Z;
  CYC(0x5ec8, 0x5ec9); alu_xor(gb, A);
  CYC(0x5ec9, 0x5eca); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5eca, 0x5ecb); mem_wr(gb, HL, A);
  RET(0x5ecb); return;
}

void mamamuDog_updateSpeedZ_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5ed4, 0x5ed6); C = 0x20;
  CALL_C(0x5ed6, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5ed9);
  if (!(F & FZ)) { RET_TAKEN(0x5ed9); return; }
  CYC(0x5ed9, 0x5eda);
  mamamuDog_hop_hook(gb); return; // fallthrough
}

void mamamuDog_hop_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x5eda, 0x5edd); SET_BC(0xff40); // -$c0
  CYC(0x5edd, 0x5ee0); objectSetSpeedZ_hook(gb); return; // jp
}

void mamamuDog_decCounter_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x5ee0, 0x5ee1); H = D;
  CYC(0x5ee1, 0x5ee3); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x5ee3, 0x5ee4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x5ee4, 0x5ee7); writeFlagsTocddb_hook(gb); return; // jp
}
