#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t vire_jump_table(GB *gb) {
  burn_rom(gb, 0, 0, 1, false); alu_add(gb, A);
  burn_rom(gb, 0, 1, 2, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0, 2, 3, false); alu_add(gb, L);
  burn_rom(gb, 0, 3, 4, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0, 4, 6, false); burn_rom(gb, 0, 6, 7, false); H = alu_inc8(gb, H);
  } else burn_rom(gb, 0, 4, 6, true);
  burn_rom(gb, 0, 7, 8, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0, 8, 9, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0, 9, 10, false); L = A;
  burn_rom(gb, 0, 10, 11, false);
  return HL;
}

static void vire_add_a_to_hl(GB *gb, uint16_t ra) {
  push_effect(gb, ra);
  burn_rom(gb, 0, 0x10, 0x11, false); alu_add(gb, L);
  burn_rom(gb, 0, 0x11, 0x12, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0, 0x12, 0x13, false); burn_rom(gb, 0, 0x13, 0x14, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0, 0x14, 0x15, false);
  } else burn_rom(gb, 0, 0x12, 0x13, true);
  ret_effect(gb);
}

void interactionCodeb8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6abf, 0x6ac1); E = 0x42;
  CYC(0x6ac1, 0x6ac2); A = mem_rd(gb, DE);
  CYC(0x6ac2, 0x6ac3); push_effect(gb, 0x6ac3); switch (vire_jump_table(gb)) {
    case 0x6ac9: vire_subid0_hook(gb); return;
    case 0x6b0a: vire_subid1_hook(gb); return;
    case 0x6b88: vire_subid2_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void vire_subid0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6ac9, checkInteractionState_hook, 0x23fe, 0x6acc);
  if (F & FZ) { CYCT(0x6acc, 0x6ace); goto state0; }
  CYC(0x6acc, 0x6ace);
state1:
  CYC(0x6ace, 0x6ad0); E = 0x78;
  CYC(0x6ad0, 0x6ad1); A = mem_rd(gb, DE);
  CYC(0x6ad1, 0x6ad2); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6ad2, 0x6ad4); goto runScript; }
  CYC(0x6ad2, 0x6ad4); CALL_C(0x6ad4, vire_disableObjectsIfLinkIsReady_hook, 0x6c33, 0x6ad7);
  if (!(F & FC)) { CYC(0x6ad7, 0x6ad9); goto animate; }
  CYCT(0x6ad7, 0x6ad9); CYC(0x6ad9, 0x6ada); alu_xor(gb, A);
  CYC(0x6ada, 0x6add); mem_wr(gb, 0xd008, A);
runScript:
  CALL_C(0x6add, interactionRunScript_hook, 0x2552, 0x6ae0);
  if (F & FC) { CYC(0x6ae0, 0x6ae3); vire_deleteAndReturnControl_hook(gb); return; }
  CYC(0x6ae0, 0x6ae3);
animate:
  CYC(0x6ae3, 0x6ae6); interactionAnimate_hook(gb); return;
state0:
  CALL_C(0x6ae6, getThisRoomFlags_hook, 0x197d, 0x6ae9);
  CYC(0x6ae9, 0x6aeb); alu_bit(gb, 6, mem_rd(gb, HL));
  if (!(F & FZ)) { CYC(0x6aeb, 0x6aee); interactionDelete_hook(gb); return; }
  CYC(0x6aeb, 0x6aee); CYC(0x6aee, 0x6af0); A = 0x39;
  CALL_C(0x6af0, playSound_b00_hook, 0x0c98, 0x6af3);
  CYC(0x6af3, 0x6af6); SET_HL(0x7d4a);
  vire_setScript_hook(gb);
}

void vire_setScript_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6af6, interactionSetScript_hook, 0x2544, 0x6af9);
  CALL_C(0x6af9, interactionInitGraphics_hook, 0x15fb, 0x6afc);
  CALL_C(0x6afc, interactionIncState_hook, 0x23e0, 0x6aff);
  CYC(0x6aff, 0x6b01); L = 0x50;
  CYC(0x6b01, 0x6b03); mem_wr(gb, HL, 0x50);
  CYC(0x6b03, 0x6b04); alu_xor(gb, A);
  CYC(0x6b04, 0x6b07); mem_wr(gb, 0xcfd0, A);
  CYC(0x6b07, 0x6b0a); objectSetVisiblec2_hook(gb); return;
}

void vire_subid1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6b0a, 0x6b0c); E = 0x44;
  CYC(0x6b0c, 0x6b0d); A = mem_rd(gb, DE);
  CYC(0x6b0d, 0x6b0e); push_effect(gb, 0x6b0e); switch (vire_jump_table(gb)) {
    case 0x6b14: goto state0;
    case 0x6b31: goto state1;
    case 0x6b5b: goto state2;
    default: hook_continue(gb, HL, sp0_); return;
  }
state0:
  CYC(0x6b14, 0x6b17); A = mem_rd(gb, 0xcae7);
  CYC(0x6b17, 0x6b19); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYC(0x6b19, 0x6b1c); interactionDelete_hook(gb); return; }
  CALL_C(0x6b1c, getThisRoomFlags_hook, 0x197d, 0x6b1f);
  CYC(0x6b1f, 0x6b21); alu_bit(gb, 6, mem_rd(gb, HL));
  CYC(0x6b21, 0x6b24); SET_HL(0x7d57);
  if (F & FZ) { CYCT(0x6b24, 0x6b26); vire_setScript_hook(gb); return; }
  CYC(0x6b24, 0x6b26); CYC(0x6b26, 0x6b29); A = mem_rd(gb, 0xcc35);
  CYC(0x6b29, 0x6b2a); alu_or(gb, A); CYC(0x6b2a, 0x6b2c); A = 0x2d;
  if (!(F & FZ)) CALL_C(0x6b2c, playSound_b00_hook, 0x0c98, 0x6b2f); else CYC(0x6b2c, 0x6b2f);
  CYC(0x6b2f, 0x6b31); goto gotoState2;
state1:
  CYC(0x6b31, 0x6b33); E = 0x78; CYC(0x6b33, 0x6b34); A = mem_rd(gb, DE); CYC(0x6b34, 0x6b35); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6b35, 0x6b37); goto runScript; }
  CYC(0x6b35, 0x6b37); CYC(0x6b37, 0x6b3a); A = mem_rd(gb, 0xd00b); CYC(0x6b3a, 0x6b3c); alu_cp(gb, 0x9b);
  if (!(F & FC)) { CYC(0x6b3c, 0x6b3f); goto animate1; }
  CYC(0x6b3c, 0x6b3f); CALL_C(0x6b3f, vire_disableObjectsIfLinkIsReady_hook, 0x6c33, 0x6b42);
  if (!(F & FC)) { CYC(0x6b42, 0x6b45); goto animate1; }
runScript:
  CALL_C(0x6b45, interactionRunScript_hook, 0x2552, 0x6b48);
  if (!(F & FC)) { CYC(0x6b48, 0x6b4b); goto animate1; }
  CALL_C(0x6b4b, objectSetInvisible_hook, 0x1e7b, 0x6b4e);
  CALL_C(0x6b4e, vire_returnControl_hook, 0x6c52, 0x6b51);
gotoState2:
  CYC(0x6b51, 0x6b52); H = D; CYC(0x6b52, 0x6b54); L = 0x44; CYC(0x6b54, 0x6b56); mem_wr(gb, HL, 0x02); CYC(0x6b56, 0x6b58); L = 0x46; CYC(0x6b58, 0x6b5a); mem_wr(gb, HL, 0x08); RET(0x6b5a); return;
animate1:
  CYC(0x6b42, 0x6b46); interactionAnimate_hook(gb); return;
state2:
  CALL_C(0x6b5b, interactionDecCounter1_hook, 0x23cc, 0x6b5e);
  if (!(F & FZ)) { RET_TAKEN(0x6b5e); return; }
  CYC(0x6b5f, 0x6b62); SET_HL(0xd00b); CYC(0x6b62, 0x6b63); A = mem_rd(gb, HL); SET_HL(HL + 1); CYC(0x6b63, 0x6b65); alu_cp(gb, 0x10);
  if (!(F & FC)) { CYC(0x6b65, 0x6b67); goto spawn; }
  CYC(0x6b65, 0x6b67); L = alu_inc8(gb, L); CYC(0x6b68, 0x6b69); A = mem_rd(gb, HL); CYC(0x6b69, 0x6b6b); alu_cp(gb, 0xa0);
  if (!(F & FC)) { CYC(0x6b6b, 0x6b6d); vire_setRandomCounter1_hook(gb); return; }
spawn:
  CALL_C(0x6b6d, getFreePartSlot_hook, 0x3e8e, 0x6b70);
  if (!(F & FZ)) { CYC(0x6b70, 0x6b72); vire_setRandomCounter1_hook(gb); return; }
  CYC(0x6b72, 0x6b74); mem_wr(gb, HL, 0x2c); CYC(0x6b74, 0x6b75); L = alu_inc8(gb, L); CYC(0x6b75, 0x6b76); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); vire_setRandomCounter1_hook(gb);
}

void vire_setRandomCounter1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6b76, getRandomNumber_noPreserveVars_hook, 0x0453, 0x6b79);
  CYC(0x6b79, 0x6b7b); alu_and(gb, 0x03); CYC(0x6b7b, 0x6b7e); SET_HL(0x6b84);
  vire_add_a_to_hl(gb, 0x6b7f);
  CYC(0x6b7f, 0x6b81); E = 0x46; CYC(0x6b81, 0x6b82); A = mem_rd(gb, HL); CYC(0x6b82, 0x6b83); mem_wr(gb, DE, A); RET(0x6b83); return;
}

void vire_subid2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6b88, 0x6b8a); E = 0x44; CYC(0x6b8a, 0x6b8b); A = mem_rd(gb, DE);
  CYC(0x6b8b, 0x6b8c); push_effect(gb, 0x6b8c); switch (vire_jump_table(gb)) {
    case 0x6b94: goto state0;
    case 0x6bb6: goto state1;
    case 0x6c0c: goto state2;
    case 0x6c25: goto state3;
    default: hook_continue(gb, HL, sp0_); return;
  }
state0:
  CALL_C(0x6b94, getThisRoomFlags_hook, 0x197d, 0x6b97); CYC(0x6b97, 0x6b99); alu_bit(gb, 6, mem_rd(gb, HL));
  if (!(F & FZ)) { CYC(0x6b99, 0x6b9c); interactionDelete_hook(gb); return; }
  CYC(0x6b9c, 0x6b9f); SET_BC(0xad03); CALL_C(0x6b9f, objectCreateInteraction_hook, 0x24c5, 0x6ba2);
  if (!(F & FZ)) { RET_TAKEN(0x6ba2); return; }
  CYC(0x6ba3, 0x6ba5); E = 0x56; CYC(0x6ba5, 0x6ba7); A = 0x40; CYC(0x6ba7, 0x6ba8); mem_wr(gb, DE, A); CYC(0x6ba8, 0x6ba9); E = alu_inc8(gb, E); CYC(0x6ba9, 0x6baa); A = H; CYC(0x6baa, 0x6bab); mem_wr(gb, DE, A); CYC(0x6bab, 0x6bae); SET_HL(0x7d6a); CALL_C(0x6bae, vire_setScript_hook, 0x6af6, 0x6bb1);
  CYC(0x6bb1, 0x6bb3); L = 0x46; CYC(0x6bb3, 0x6bb5); mem_wr(gb, HL, 0x08); RET(0x6bb5); return;
state1:
  CYC(0x6bb6, 0x6bb9); SET_HL(0xd00b); CYC(0x6bb9, 0x6bba); A = mem_rd(gb, HL); SET_HL(HL + 1); CYC(0x6bba, 0x6bbc); alu_cp(gb, 0x40);
  if (!(F & FC)) { CYC(0x6bbc, 0x6bbe); goto game; }
  CYC(0x6bbe, 0x6bbf); L = alu_inc8(gb, L); CYC(0x6bbf, 0x6bc0); A = mem_rd(gb, HL); CYC(0x6bc0, 0x6bc2); alu_cp(gb, 0x58);
  if (!(F & FC)) { CYC(0x6bc2, 0x6bc4); goto game; }
  CALL_C(0x6bc4, vire_disableObjectsIfLinkIsReady_hook, 0x6c33, 0x6bc7); if (!(F & FC)) { CYC(0x6bc7, 0x6bc9); goto game; }
  CYC(0x6bc9, 0x6bcb); A = 1; CYC(0x6bcb, 0x6bce); mem_wr(gb, 0xcc8a, A); CYC(0x6bce, 0x6bd1); mem_wr(gb, 0xcfd0, A); CYC(0x6bd1, 0x6bd3); A = 3; CYC(0x6bd3, 0x6bd6); mem_wr(gb, 0xd008, A); CYC(0x6bd6, 0x6bd9); interactionIncState_hook(gb); return;
game:
  CYC(0x6bd9, 0x6bda); H = D; CYC(0x6bda, 0x6bdc); L = 0x47; CYC(0x6bdc, 0x6bde); A = mem_rd(gb, HL); CYC(0x6bdd, 0x6bde); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6bde, 0x6be0); goto counter; }
  CYC(0x6be0, 0x6be1); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); if (!(F & FZ)) { CYCT(0x6be1, 0x6be3); goto counter; }
  CYC(0x6be1, 0x6be3); CYC(0x6be3, 0x6be5); E = 0x48; CYC(0x6be5, 0x6be6); alu_xor(gb, A); CYC(0x6be6, 0x6be7); mem_wr(gb, DE, A); CALL_C(0x6be7, interactionSetAnimation_hook, 0x262e, 0x6bea);
counter:
  CALL_C(0x6bea, interactionDecCounter1_hook, 0x23cc, 0x6bed); if (!(F & FZ)) { CYC(0x6bed, 0x6bef); goto animate2; }
  CALL_C(0x6bef, getFreePartSlot_hook, 0x3e8e, 0x6bf2); if (!(F & FZ)) { CYC(0x6bf2, 0x6bf4); goto setCounter; }
  CYC(0x6bf4, 0x6bf6); mem_wr(gb, HL, 0x2c); CALL_C(0x6bf6, objectCopyPosition_hook, 0x2242, 0x6bf9); CYC(0x6bf9, 0x6bfb); E = 0x48; CYC(0x6bfb, 0x6bfd); A = 1; CYC(0x6bfd, 0x6bfe); mem_wr(gb, DE, A); CALL_C(0x6bfe, interactionSetAnimation_hook, 0x262e, 0x6c01); CYC(0x6c01, 0x6c03); E = 0x47; CYC(0x6c03, 0x6c05); A = 0x18; CYC(0x6c05, 0x6c06); mem_wr(gb, DE, A);
setCounter:
  CALL_C(0x6c06, vire_setRandomCounter1_hook, 0x6b76, 0x6c09);
animate2:
  CYC(0x6c09, 0x6c0c); interactionAnimate_hook(gb); return;
state2:
  CALL_C(0x6c0c, interactionIncState_hook, 0x23e0, 0x6c0f); CYC(0x6c0f, 0x6c11); L = 0x46; CYC(0x6c11, 0x6c12); alu_xor(gb, A); CYC(0x6c12, 0x6c13); mem_wr(gb, HL, A); SET_HL(HL + 1); CYC(0x6c13, 0x6c14); mem_wr(gb, HL, A); CYC(0x6c14, 0x6c16); E = 0x48; CYC(0x6c16, 0x6c17); A = mem_rd(gb, DE); CYC(0x6c17, 0x6c18); A = alu_dec8(gb, A); if (F & FZ) CALL_C_CC(0x6c18, interactionSetAnimation_hook, 0x262e, 0x6c1b); else CYC(0x6c18, 0x6c1b); CYC(0x6c1b, 0x6c1d); A = 0x80; CYC(0x6c1d, 0x6c20); mem_wr(gb, 0xcc8a, A); CYC(0x6c20, 0x6c22); A = 0xf0; CALL_C(0x6c22, playSound_b00_hook, 0x0c98, 0x6c25); goto state3;
state3:
  CALL_C(0x6c25, interactionRunScript_hook, 0x2552, 0x6c28); if (!(F & FC)) { CYC(0x6c28, 0x6c2a); goto animate2; } CYC(0x6c2a, 0x6c2c); A = 5; CALL_C(0x6c2c, objectGetRelatedObject1Var_hook, 0x2160, 0x6c2f); CYC(0x6c2f, 0x6c30); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); CYC(0x6c30, 0x6c33); interactionDelete_hook(gb); return;
}

void vire_disableObjectsIfLinkIsReady_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6c33, 0x6c36); A = mem_rd(gb, 0xcc5c); CYC(0x6c36, 0x6c37); alu_or(gb, A); if (!(F & FZ)) { RET_TAKEN(0x6c37); return; }
  CALL_C(0x6c38, checkLinkVulnerable_hook, 0x1d28, 0x6c3b); if (!(F & FC)) { RET_TAKEN(0x6c3b); return; }
  CYC(0x6c3c, 0x6c3e); A = 0x80; CYC(0x6c3e, 0x6c41); mem_wr(gb, 0xcc8a, A); CYC(0x6c41, 0x6c44); mem_wr(gb, 0xcc02, A); CYC(0x6c44, 0x6c46); E = 0x78; CYC(0x6c46, 0x6c47); mem_wr(gb, DE, A); CALL_C(0x6c47, clearAllParentItems_hook, 0x2c10, 0x6c4a); CALL_C(0x6c4a, dropLinkHeldItem_hook, 0x2c43, 0x6c4d); CYC(0x6c4d, 0x6c4e); alu_scf(gb); RET(0x6c4e); return;
}

void vire_deleteAndReturnControl_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6c4f, interactionDelete_hook, 0x3b05, 0x6c52);
  vire_returnControl_hook(gb);
}

void vire_returnControl_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6c52, 0x6c53); alu_xor(gb, A); CYC(0x6c53, 0x6c56); mem_wr(gb, 0xcc8a, A); CYC(0x6c56, 0x6c59); mem_wr(gb, 0xcc02, A); RET(0x6c59); return;
}
