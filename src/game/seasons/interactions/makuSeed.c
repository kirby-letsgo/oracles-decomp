#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/makuSeed.s.
// INTERAC_MAKU_SEED

static uint16_t makuSeed_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

// interactionCode93@func_5338
static void makuSeed_func_5338(GB *gb) {
  BASE(interactionCode93);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+217, b_+220); SET_BC(0x8408);
  CALL_C(b_+220, s_objectCreateInteraction, SYM(objectCreateInteraction), b_+223);
  if (!(F & FZ)) { RET_TAKEN(b_+223); return; }
  CYC(b_+223, b_+224);
  CYC(b_+224, b_+226); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(b_+226, b_+228); A = 0x40;
  CYC(b_+228, b_+229); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+229, b_+230); mem_wr(gb, HL, D);
  RET(b_+230); return;
}

// objectApplyComponentSpeed@addSpeedComponent
static void makuSeed_addSpeedComponent(GB *gb) {
  BASE(objectApplyComponentSpeed);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+15, b_+16); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+16, b_+17); E = alu_inc8(gb, E);
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); alu_adc(gb, mem_rd(gb, HL));
  CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1);
  RET(b_+20); return;
}

// interactionCode93@func_52f3
static void makuSeed_func_52f3(GB *gb) {
  BASE(interactionCode93);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+148, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+151);
  if (!(F & FZ)) { RET_TAKEN(b_+151); return; }
  CYC(b_+151, b_+152);
  CYC(b_+152, b_+154); mem_wr(gb, HL, 0x84);
  CYC(b_+154, b_+155); L = alu_inc8(gb, L);
  CYC(b_+155, b_+157); mem_wr(gb, HL, 0x03);
  CYC(b_+157, b_+159); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+159, b_+161); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+161, b_+162); A = mem_rd(gb, DE);
  CYC(b_+162, b_+163); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+163, b_+164); E = alu_inc8(gb, E);
  CYC(b_+164, b_+165); E = alu_inc8(gb, E);
  CYC(b_+165, b_+166); L = alu_inc8(gb, L);
  CYC(b_+166, b_+167); A = mem_rd(gb, DE);
  CYC(b_+167, b_+168); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+168, b_+170); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+170, b_+172); L = INTERACTION_BASE + OBJ_YH;
  CALL_L(b_+172, makuSeed_addSpeedComponent, b_+175);
  CALL_C(b_+175, s_getRandomNumber, SYM(getRandomNumber), b_+178);
  CYC(b_+178, b_+180); alu_and(gb, 0x07);
  CYC(b_+180, b_+181); alu_add(gb, A);
  CYC(b_+181, b_+182); push_effect(gb, DE);
  CYC(b_+182, b_+185); SET_DE(b_+201 /* @table_5328 */);
  CALL_C(b_+185, s_addAToDe, SYM(addAToDe), b_+188);
  CYC(b_+188, b_+189); A = mem_rd(gb, DE);
  CYC(b_+189, b_+191); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+191, b_+192); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+192, b_+193); mem_wr(gb, HL, A);
  CYC(b_+193, b_+194); SET_DE(DE + 1);
  CYC(b_+194, b_+195); A = mem_rd(gb, DE);
  CYC(b_+195, b_+197); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+197, b_+198); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+198, b_+199); mem_wr(gb, HL, A);
  CYC(b_+199, b_+200); SET_DE(pop_effect(gb));
  RET(b_+200); return;
}

// interactionCode93@func_5298
static void makuSeed_func_5298(GB *gb) {
  BASE(interactionCode93);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+57, b_+59); A = 0xab;
  CALL_C(b_+59, s_loadPaletteHeader, SYM(loadPaletteHeader), b_+62);
  CALL_C(b_+62, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+65);
  CYC(b_+65, b_+68);
  TAIL(interactionIncState);
}

// INTERAC_MAKU_SEED
void s_interactionCode93_hook(GB *gb) {
  BASE(interactionCode93);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (makuSeed_jump_table(gb));
    if (jt_ == b_+8) goto subid0;
    if (jt_ == b_+68) goto subid1;
    HANDOFF(HL);
  } while (0);
subid0:
  CALL_C(b_+8, s_checkInteractionState, SYM(checkInteractionState), b_+11);
  if (!(F & FZ)) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  CALL_L(b_+12, makuSeed_func_5298, b_+15);
  CALL_C(b_+15, s_objectSetVisible80, SYM(objectSetVisible80), b_+18);
  CYC(b_+18, b_+21); A = mem_rd(gb, wc6e5);
  CYC(b_+21, b_+23); B = 0x04;
  CYC(b_+23, b_+25); alu_cp(gb, 0x06);
  if (F & FZ) { CYCT(b_+25, b_+27); goto L_5286; }
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+29); alu_cp(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+29, b_+32); TAIL(interactionDelete); }
  CYC(b_+29, b_+32);
  CYC(b_+32, b_+34); A = 0x01;
  CALL_C(b_+34, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+37);
  CYC(b_+37, b_+39); B = 0x08;
L_5286:
  CALL_C(b_+39, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+42);
  if (!(F & FZ)) { RET_TAKEN(b_+42); return; }
  CYC(b_+42, b_+43);
  CYC(b_+43, b_+45); mem_wr(gb, HL, 0x84);
  CYC(b_+45, b_+46); L = alu_inc8(gb, L);
  CYC(b_+46, b_+48); mem_wr(gb, HL, 0x04);
  CALL_C(b_+48, s_objectCopyPosition, SYM(objectCopyPosition), b_+51);
  CYC(b_+51, b_+53); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+53, b_+54); A = mem_rd(gb, HL);
  CYC(b_+54, b_+55); alu_add(gb, B);
  CYC(b_+55, b_+56); mem_wr(gb, HL, A);
  RET(b_+56); return;

subid1:
  CYC(b_+68, b_+70); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+70, b_+71); A = mem_rd(gb, DE);
  CYC(b_+71, b_+72); push_effect(gb, b_+72);
  do { uint16_t jt_ = (makuSeed_jump_table(gb));
    if (jt_ == b_+76) goto state0;
    if (jt_ == b_+107) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_L(b_+76, makuSeed_func_5298, b_+79);
  CYC(b_+79, b_+81); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+81, b_+83); mem_wr(gb, HL, 0x65);
  CYC(b_+83, b_+85); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+85, b_+87); mem_wr(gb, HL, 0x50);
  CYC(b_+87, b_+89); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+89, b_+91); mem_wr(gb, HL, 0x8b);
  CYC(b_+91, b_+93); A = 0x02;
  CALL_C(b_+93, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+96);
  CYC(b_+96, b_+99); A = mem_rd(gb, wFrameCounter);
  CYC(b_+99, b_+100); alu_cpl(gb);
  CYC(b_+100, b_+101); A = alu_inc8(gb, A);
  CYC(b_+101, b_+103); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+103, b_+104); mem_wr(gb, DE, A);
  CALL_L(b_+104, makuSeed_func_5338, b_+107);
state1:
  CYC(b_+107, b_+108); H = D;
  CYC(b_+108, b_+110); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+110, b_+111); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+111, b_+113); alu_cp(gb, 0xed);
  if (!(F & FC)) { CYCT(b_+113, b_+116); TAIL(interactionDelete); }
  CYC(b_+113, b_+116);
  CYC(b_+116, b_+119); SET_BC(0x0080);
  CYC(b_+119, b_+120); A = C;
  CYC(b_+120, b_+121); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+121, b_+122); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+122, b_+123); A = B;
  CYC(b_+123, b_+124); alu_adc(gb, mem_rd(gb, HL));
  CYC(b_+124, b_+125); mem_wr(gb, HL, A);
  CYC(b_+125, b_+128); A = mem_rd(gb, wFrameCounter);
  CYC(b_+128, b_+130); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+130, b_+131); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+131, b_+132); push_effect(gb, AF);
  CYC(b_+132, b_+134); alu_and(gb, 0x0f);
  if (F & FZ) CALL_L_CC(b_+134, makuSeed_func_52f3, b_+137);
  else CYC(b_+134, b_+137);
  CYC(b_+137, b_+138); SET_AF(pop_effect(gb));
  CYC(b_+138, b_+140); alu_and(gb, 0x3f);
  CYC(b_+140, b_+142); A = 0x83;
  if (F & FZ) CALL_C_CC(b_+142, s_playSound, SYM(playSound_b00), b_+145);
  else CYC(b_+142, b_+145);
  CYC(b_+145, b_+148);
  TAIL(objectSetPriorityRelativeToLink_withTerrainEffects);
}

