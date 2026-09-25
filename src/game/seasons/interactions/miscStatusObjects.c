#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/miscStatusObjects.s.
// INTERAC_MISC_STATIC_OBJECTS

static uint16_t miscStatusObjects_jump_table(GB *gb) {
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

// interactionCode4c@func_72de
static void miscStatusObjects_func_72de(GB *gb) {
  BASE(interactionCode4c);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+94, s_checkInteractionState, SYM(checkInteractionState), b_+97);
  if (!(F & FZ)) { CYCT(b_+97, b_+99); goto L_72e9; }
  CYC(b_+97, b_+99);
  CYC(b_+99, b_+101); A = 0x01;
  CYC(b_+101, b_+102); mem_wr(gb, DE, A);
  CYC(b_+102, b_+105);
  TAIL(interactionInitGraphics);
L_72e9:
  CYC(b_+105, b_+106); SET_HL(pop_effect(gb));
  CYC(b_+106, b_+109);
  TAIL(interactionAnimate);
}

// INTERAC_MISC_STATIC_OBJECTS
void s_interactionCode4c_hook(GB *gb) {
  BASE(interactionCode4c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (miscStatusObjects_jump_table(gb));
    if (jt_ == b_+56) goto subid0;
    if (jt_ == b_+44) goto subid1;
    if (jt_ == b_+185) goto subid4;
    if (jt_ == b_+50) goto subid6;
    if (jt_ == b_+109) goto subid7;
    if (jt_ == b_+134) goto subid8;
    if (jt_ == b_+164) goto subid9;
    HANDOFF(HL);
  } while (0);
subid1:
  CALL_L(b_+44, miscStatusObjects_func_72de, b_+47);
  CYC(b_+47, b_+50);
  TAIL(objectSetVisible82);
subid6:
  CALL_L(b_+50, miscStatusObjects_func_72de, b_+53);
  CYC(b_+53, b_+56);
  TAIL(objectSetVisible83);
subid0:
  CALL_C(b_+56, s_checkInteractionState, SYM(checkInteractionState), b_+59);
  if (!(F & FZ)) { CYCT(b_+59, b_+61); goto L_72ca; }
  CYC(b_+59, b_+61);
  CYC(b_+61, b_+62); H = D;
  CYC(b_+62, b_+63); L = E;
  CYC(b_+63, b_+64); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+64, b_+66); L = INTERACTION_BASE + OBJ_ENABLED;
  CYC(b_+66, b_+68); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CALL_C(b_+68, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+71);
  CYC(b_+71, b_+74);
  TAIL(objectSetVisible80);
L_72ca:
  CALL_C(b_+74, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+77);
  CYC(b_+77, b_+79); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+79, b_+81); goto L_72db; }
  CYC(b_+79, b_+81);
  CYC(b_+81, b_+83); E = INTERACTION_BASE + OBJ_ANIM_COUNTER;
  CYC(b_+83, b_+84); A = mem_rd(gb, DE);
  CYC(b_+84, b_+86); alu_cp(gb, 0x10);
  if (!(F & FZ)) { CYCT(b_+86, b_+88); goto L_72db; }
  CYC(b_+86, b_+88);
  CYC(b_+88, b_+90); A = 0x02;
  CYC(b_+90, b_+91); mem_wr(gb, DE, A);
L_72db:
  CYC(b_+91, b_+94);
  TAIL(interactionAnimate);

subid7:
  CALL_C(b_+109, s_checkInteractionState, SYM(checkInteractionState), b_+112);
  if (!(F & FZ)) { CYCT(b_+112, b_+114); goto L_7303; }
  CYC(b_+112, b_+114);
  CYC(b_+114, b_+116); A = 0x01;
  CYC(b_+116, b_+117); mem_wr(gb, DE, A);
  CALL_C(b_+117, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+120);
  CYC(b_+120, b_+122); A = 0x9b;
  CALL_C(b_+122, s_loadPaletteHeader, SYM(loadPaletteHeader), b_+125);
  CALL_C(b_+125, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+128);
  CALL_C(b_+128, s_objectSetVisible82, SYM(objectSetVisible82), b_+131);
L_7303:
  CYC(b_+131, b_+134);
  TAIL(interactionAnimate);
subid8:
  CALL_C(b_+134, s_checkInteractionState, SYM(checkInteractionState), b_+137);
  if (!(F & FZ)) { CYCT(b_+137, b_+139); goto L_7317; }
  CYC(b_+137, b_+139);
  CYC(b_+139, b_+141); A = 0x01;
  CYC(b_+141, b_+142); mem_wr(gb, DE, A);
  CALL_C(b_+142, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+145);
  CALL_C(b_+145, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+148);
  CALL_C(b_+148, s_objectSetVisible80, SYM(objectSetVisible80), b_+151);
L_7317:
  CALL_C(b_+151, s_interactionAnimate, SYM(interactionAnimate), b_+154);
  CYC(b_+154, b_+157); A = mem_rd(gb, wFrameCounter);
  CYC(b_+157, b_+158); alu_rrca(gb);
  if (F & FC) { CYCT(b_+158, b_+161); TAIL(objectSetInvisible); }
  CYC(b_+158, b_+161);
  CYC(b_+161, b_+164);
  TAIL(objectSetVisible);
subid9:
  CALL_C(b_+164, s_checkInteractionState, SYM(checkInteractionState), b_+167);
  if (!(F & FZ)) { RET_TAKEN(b_+167); return; }
  CYC(b_+167, b_+168);
  CALL_C(b_+168, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+171);
  CYC(b_+171, b_+173); alu_and(gb, 0x40);
  if (F & FZ) { CYCT(b_+173, b_+176); TAIL(interactionDelete); }
  CYC(b_+173, b_+176);
  CALL_C(b_+176, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+179);
  CALL_C(b_+179, s_interactionIncState, SYM(interactionIncState), b_+182);
  CYC(b_+182, b_+185);
  TAIL(objectSetVisible83);
subid4:
  CALL_C(b_+185, s_checkInteractionState, SYM(checkInteractionState), b_+188);
  if (!(F & FZ)) { CYCT(b_+188, b_+190); goto L_735d; }
  CYC(b_+188, b_+190);
  CYC(b_+190, b_+192); A = 0x01;
  CYC(b_+192, b_+193); mem_wr(gb, DE, A);
  CALL_C(b_+193, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+196);
  CYC(b_+196, b_+199); SET_BC(0xfe00);
  CALL_C(b_+199, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+202);
  CYC(b_+202, b_+204); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+204, b_+206); mem_wr(gb, HL, 0x01);
  CYC(b_+206, b_+208); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+208, b_+210); mem_wr(gb, HL, 0x28);
  CYC(b_+210, b_+212); A = 0x51;
  CALL_C(b_+212, s_playSound, SYM(playSound_b00), b_+215);
  CALL_C(b_+215, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+218);
  CYC(b_+218, b_+221);
  TAIL(objectSetVisiblec0);
L_735d:
  CALL_C(b_+221, s_objectApplySpeed, SYM(objectApplySpeed), b_+224);
  CYC(b_+224, b_+226); C = 0x20;
  CALL_C(b_+226, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+229);
  if (!(F & FZ)) { RET_TAKEN(b_+229); return; }
  CYC(b_+229, b_+230);
  CYC(b_+230, b_+232); A = 0x77;
  CALL_C(b_+232, s_playSound, SYM(playSound_b00), b_+235);
  CYC(b_+235, b_+238);
  TAIL(interactionDelete);
}

