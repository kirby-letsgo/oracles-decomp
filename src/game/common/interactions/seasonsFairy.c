#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void objectOscillateZ_body_hook(GB *gb) {
  BASE(objectOscillateZ_body);
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb); return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, wFrameCounter);
  CYC(b_+9, b_+11); alu_and(gb, 0x38);
  CYC(b_+11, b_+13); A = alu_swap(gb, A);
  CYC(b_+13, b_+14); alu_rlca(gb);
  CYC(b_+14, b_+17); SET_HL(b_+27);
  CYC(b_+17, b_+18); push_effect(gb, b_+18);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  }
  CYC(b_+18, b_+20); A = mem_rd(gb, hActiveObjectType);
  CYC(b_+20, b_+22); alu_add(gb, 0x0f);
  CYC(b_+22, b_+23); E = A;
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+25); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  CYC(b_+26, b_+27); ret_effect(gb);
}

static uint16_t seasonsFairyJumpTable(GB *gb) {
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

static void seasonsFairyCreatePuff(GB *gb) {
  BASE(interactionCode50);
  CYC(b_+108, b_+111); objectCreatePuff_hook(gb);
}

static void seasonsFairyCreateSparkle0(GB *gb) {
  BASE(interactionCode50);
  CYC(b_+111, b_+114); SET_BC(0x8400);
  CYC(b_+114, b_+116);
  CYC(b_+131, b_+134); objectCreateInteraction_hook(gb);
}

static void seasonsFairyUpdateAnimation(GB *gb) {
  BASE(interactionCode50);
  CYC(b_+277, b_+280); interactionAnimate_hook(gb);
}

static void seasonsFairyCreateSparkle1(GB *gb) {
  BASE(interactionCode50);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+116, b_+119); SET_BC(0x8407);
  CALL_C(b_+119, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+122);
  CYC(b_+122, b_+124); E = 0x46;
  CYC(b_+124, b_+125); A = mem_rd(gb, DE);
  CYC(b_+125, b_+126); L = E;
  CYC(b_+126, b_+127); mem_wr(gb, HL, A);
  CYC(b_+127, b_+128); ret_effect(gb);
}

static void seasonsFairyCreateSparkle2(GB *gb) {
  BASE(interactionCode50);
  CYC(b_+128, b_+131); SET_BC(0x8401);
  CYC(b_+131, b_+134); objectCreateInteraction_hook(gb);
}

static void seasonsFairyFunc48eb(GB *gb) {
  BASE(interactionCode50);
  CYC(b_+336, b_+337); H = D;
  CYC(b_+337, b_+339); L = 0x4b;
  CYC(b_+339, b_+341); E = 0x78;
  CYC(b_+341, b_+342); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+342, b_+343); mem_wr(gb, DE, A);
  CYC(b_+343, b_+344); B = A;
  CYC(b_+344, b_+345); L = alu_inc8(gb, L);
  CYC(b_+345, b_+346); E = alu_inc8(gb, E);
  CYC(b_+346, b_+347); A = mem_rd(gb, HL);
  CYC(b_+347, b_+348); mem_wr(gb, DE, A);
  CYC(b_+348, b_+349); C = A;
  CYC(b_+349, b_+350); ret_effect(gb);
}

static void seasonsFairyFunc48d0(GB *gb) {
  BASE(interactionCode50);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+309, b_+311); L = 0x4b;
  CYC(b_+311, b_+313); E = 0x78;
  CYC(b_+313, b_+314); A = mem_rd(gb, DE);
  CYC(b_+314, b_+315); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+315, b_+316); L = alu_inc8(gb, L);
  CYC(b_+316, b_+317); E = alu_inc8(gb, E);
  CYC(b_+317, b_+318); A = mem_rd(gb, DE);
  CYC(b_+318, b_+319); mem_wr(gb, HL, A);
  CYC(b_+319, b_+322); A = mem_rd(gb, w1Link_yh);
  CYC(b_+322, b_+323); B = A;
  CYC(b_+323, b_+326); A = mem_rd(gb, w1Link_xh);
  CYC(b_+326, b_+327); C = A;
  CALL_C(b_+327, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+330);
  CYC(b_+330, b_+332); E = 0x49;
  CYC(b_+332, b_+333); mem_wr(gb, DE, A);
  CALL_C(b_+333, objectApplySpeed_hook, SYM(objectApplySpeed), b_+336);
  seasonsFairyFunc48eb(gb);
}

static void seasonsFairyFunc48f9(GB *gb) {
  BASE(interactionCode50);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+350, b_+352); E = 0x7a;
  CYC(b_+352, b_+353); A = mem_rd(gb, DE);
  CYC(b_+353, b_+354); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+354, b_+355); ret_effect(gb); return; }
  CYC(b_+354, b_+355);
  CALL_C(b_+355, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+358);
  if (!(F & FZ)) { CYCT(b_+358, b_+359); ret_effect(gb); return; }
  CYC(b_+358, b_+359);
  CYC(b_+359, b_+361); mem_wr(gb, HL, 0x04);
  CYC(b_+361, b_+362); L = E;
  CYC(b_+362, b_+363); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+363, b_+364); ret_effect(gb);
}

static void seasonsFairyFunc4907(GB *gb) {
  BASE(interactionCode50);
  CYC(b_+364, b_+367); A = mem_rd(gb, wFrameCounter);
  CYC(b_+367, b_+368); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+368, b_+369); ret_effect(gb); return; }
  CYC(b_+368, b_+369);
  CYC(b_+369, b_+371); E = 0x7b;
  CYC(b_+371, b_+372); A = mem_rd(gb, DE);
  CYC(b_+372, b_+373); A = alu_inc8(gb, A);
  CYC(b_+373, b_+375); alu_and(gb, 0x1f);
  CYC(b_+375, b_+376); mem_wr(gb, DE, A);
  CYC(b_+376, b_+377); ret_effect(gb);
}

static void seasonsFairyCheckLinkIsClose(GB *gb) {
  BASE(interactionCode50);
  CYC(b_+377, b_+378); H = D;
  CYC(b_+378, b_+380); L = 0x4b;
  CYC(b_+380, b_+383); A = mem_rd(gb, w1Link_yh);
  CYC(b_+383, b_+385); alu_add(gb, 0xf0);
  CYC(b_+385, b_+386); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+386, b_+388); alu_add(gb, 0x04);
  CYC(b_+388, b_+390); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(b_+390, b_+391); ret_effect(gb); return; }
  CYC(b_+390, b_+391);
  CYC(b_+391, b_+393); L = 0x4d;
  CYC(b_+393, b_+396); A = mem_rd(gb, w1Link_xh);
  CYC(b_+396, b_+397); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+397, b_+399); alu_add(gb, 0x02);
  CYC(b_+399, b_+401); alu_cp(gb, 0x05);
  CYC(b_+401, b_+402); ret_effect(gb);
}

void interactionCode50_hook(GB *gb) {
  BASE(interactionCode50);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x44;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (seasonsFairyJumpTable(gb));
    if (jt_ == b_+16) { goto state0; }
    else if (jt_ == b_+134) { goto state1; }
    else if (jt_ == b_+202) { goto state2; }
    else if (jt_ == b_+239) { goto state3; }
    else if (jt_ == b_+280) { goto state4; }
    else if (jt_ == b_+293) { goto state5; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CYC(b_+16, b_+18); E = 0x45;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+20); push_effect(gb, b_+20);
  do { uint16_t jt_ = (seasonsFairyJumpTable(gb));
    if (jt_ == b_+26) { goto substate0; }
    else if (jt_ == b_+49) { goto substate1; }
    else if (jt_ == b_+63) { goto substate2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CALL_C(b_+26, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+29);
  CYC(b_+29, b_+31); L = 0x45;
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x01);
  CYC(b_+33, b_+35); L = 0x46;
  CYC(b_+35, b_+37); mem_wr(gb, HL, 0x01);
  CYC(b_+37, b_+39); L = 0x4f;
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x00);
  CYC(b_+41, b_+43); A = 0x0f;
  CYC(b_+43, b_+46); mem_wr(gb, wActiveMusic, A);
  CYC(b_+46, b_+49); TAIL(playSound_b00);

substate1:
  CALL_C(b_+49, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+52);
  if (!(F & FZ)) { CYCT(b_+52, b_+53); ret_effect(gb); return; }
  CYC(b_+52, b_+53);
  CYC(b_+53, b_+55); L = 0x45;
  CYC(b_+55, b_+57); mem_wr(gb, HL, 0x02);
  CYC(b_+57, b_+59); L = 0x46;
  CYC(b_+59, b_+61); mem_wr(gb, HL, 0x10);
  CYC(b_+61, b_+63); goto create_puff;

substate2:
  CALL_C(b_+63, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+66);
  if (!(F & FZ)) { CYCT(b_+66, b_+67); ret_effect(gb); return; }
  CYC(b_+66, b_+67);
  CALL_C(b_+67, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+70);
  CALL_C(b_+70, objectSetVisible80_hook, SYM(objectSetVisible80), b_+73);
  CYC(b_+73, b_+74); H = D;
  CYC(b_+74, b_+76); L = 0x44;
  CYC(b_+76, b_+78); mem_wr(gb, HL, 0x01);
  CYC(b_+78, b_+80); L = 0x45;
  CYC(b_+80, b_+82); mem_wr(gb, HL, 0x00);
  CYC(b_+82, b_+84); L = 0x43;
  CYC(b_+84, b_+85); A = mem_rd(gb, HL);
  CYC(b_+85, b_+86); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+86, b_+88); goto sparkle1; }
  CYC(b_+86, b_+88);
  CYC(b_+88, b_+90); L = 0x46;
  CYC(b_+90, b_+92); mem_wr(gb, HL, 0x78);
  CYC(b_+92, b_+95); push_effect(gb, b_+95); seasonsFairyCreateSparkle0(gb);
  CYC(b_+95, b_+98); goto update_animation;

sparkle1:
  CYC(b_+98, b_+100); L = 0x46;
  CYC(b_+100, b_+102); mem_wr(gb, HL, 0x3c);
  CYC(b_+102, b_+105); push_effect(gb, b_+105); seasonsFairyCreateSparkle1(gb);
  CYC(b_+105, b_+108); goto update_animation;

create_puff:
  CYC(b_+108, b_+111); TAIL(objectCreatePuff);

state1:
  CALL_C(b_+134, objectOscillateZ_body_hook, SYM(objectOscillateZ_body), b_+137);
  CALL_C(b_+137, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+140);
  if (F & FZ) { CYCT(b_+140, b_+142); goto state1_expired; }
  CYC(b_+140, b_+142);
  CYC(b_+142, b_+145); push_effect(gb, b_+145); seasonsFairyUpdateAnimation(gb);
  CYC(b_+145, b_+148); A = mem_rd(gb, wFrameCounter);
  CYC(b_+148, b_+149); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+149, b_+152); objectSetInvisible_hook(gb); return; }
  CYC(b_+149, b_+152);
  CYC(b_+152, b_+155); TAIL(objectSetVisible);

state1_expired:
  CYC(b_+155, b_+157); L = 0x43;
  CYC(b_+157, b_+158); A = mem_rd(gb, HL);
  CYC(b_+158, b_+159); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+159, b_+161); goto state1_normal; }
  CYC(b_+159, b_+161);
  CYC(b_+161, b_+163); L = 0x44;
  CYC(b_+163, b_+165); mem_wr(gb, HL, 0x05);
  CYC(b_+165, b_+168); SET_HL(wRoomLayoutEnd);
  CYC(b_+168, b_+170); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x02));
  CALL_C(b_+170, objectSetVisible_hook, SYM(objectSetVisible), b_+173);
  CYC(b_+173, b_+175); goto update_animation;

state1_normal:
  CYC(b_+175, b_+177); L = 0x44;
  CYC(b_+177, b_+178); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+178, b_+180); L = 0x4f;
  CYC(b_+180, b_+182); mem_wr(gb, HL, 0x00);
  CYC(b_+182, b_+184); L = 0x7a;
  CYC(b_+184, b_+186); mem_wr(gb, HL, 0x30);
  CYC(b_+186, b_+188); L = 0x49;
  CYC(b_+188, b_+190); mem_wr(gb, HL, 0x00);
  CYC(b_+190, b_+192); L = 0x50;
  CYC(b_+192, b_+194); mem_wr(gb, HL, 0x14);
  CALL_C(b_+194, objectSetVisible_hook, SYM(objectSetVisible), b_+197);
  CYC(b_+197, b_+199); A = 0x4f;
  CALL_C(b_+199, playSound_b00_hook, SYM(playSound_b00), b_+202);

state2:
  CALL_C(b_+202, objectApplySpeed_hook, SYM(objectApplySpeed), b_+205);
  CYC(b_+205, b_+206); H = D;
  CYC(b_+206, b_+208); L = 0x4b;
  CYC(b_+208, b_+209); A = mem_rd(gb, HL);
  CYC(b_+209, b_+211); alu_cp(gb, 0x10);
  if (!(F & FC)) { CYCT(b_+211, b_+213); goto update_animation; }
  CYC(b_+211, b_+213);
  CYC(b_+213, b_+215); L = 0x44;
  CYC(b_+215, b_+216); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+216, b_+218); L = 0x46;
  CYC(b_+218, b_+220); mem_wr(gb, HL, 0x04);
  CYC(b_+220, b_+222); L = 0x7b;
  CYC(b_+222, b_+224); mem_wr(gb, HL, 0x00);
  CYC(b_+224, b_+227); A = mem_rd(gb, w1Link_yh);
  CYC(b_+227, b_+229); L = 0x4b;
  CYC(b_+229, b_+230); mem_wr(gb, HL, A);
  CYC(b_+230, b_+233); A = mem_rd(gb, w1Link_xh);
  CYC(b_+233, b_+235); L = 0x4d;
  CYC(b_+235, b_+236); mem_wr(gb, HL, A);
  CYC(b_+236, b_+239); push_effect(gb, b_+239); seasonsFairyFunc48eb(gb);

state3:
  CYC(b_+239, b_+242); push_effect(gb, b_+242); seasonsFairyCheckLinkIsClose(gb);
  if (F & FC) { CYCT(b_+242, b_+244); goto link_close; }
  CYC(b_+242, b_+244);
  CYC(b_+244, b_+247); push_effect(gb, b_+247); seasonsFairyFunc48d0(gb);
  CYC(b_+247, b_+250); push_effect(gb, b_+250); seasonsFairyFunc48f9(gb);
  CYC(b_+250, b_+251); A = mem_rd(gb, DE);
  CYC(b_+251, b_+253); E = 0x7b;
  CALL_C(b_+253, objectSetPositionInCircleArc_hook, SYM(objectSetPositionInCircleArc), b_+256);
  CYC(b_+256, b_+259); push_effect(gb, b_+259); seasonsFairyFunc4907(gb);
  CYC(b_+259, b_+262); A = mem_rd(gb, wFrameCounter);
  CYC(b_+262, b_+264); alu_and(gb, 0x07);
  if (F & FZ) { CYCT(b_+264, b_+267); push_effect(gb, b_+267); seasonsFairyCreateSparkle2(gb); }
  else CYC(b_+264, b_+267);
  CYC(b_+267, b_+269); goto update_animation;

link_close:
  CYC(b_+269, b_+271); L = 0x44;
  CYC(b_+271, b_+272); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+272, b_+275); SET_HL(wRoomLayoutEnd);
  CYC(b_+275, b_+277); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x02));

update_animation:
  seasonsFairyUpdateAnimation(gb); return;

state4:
  CALL_C(b_+280, objectOscillateZ_body_hook, SYM(objectOscillateZ_body), b_+283);
  CYC(b_+283, b_+286); A = mem_rd(gb, wRoomLayoutEnd);
  CYC(b_+286, b_+288); alu_cp(gb, 0x07);
  if (F & FZ) { CYCT(b_+288, b_+291); interactionDelete_hook(gb); return; }
  CYC(b_+288, b_+291);
  CYC(b_+291, b_+293); goto update_animation;

state5:
  CALL_C(b_+293, objectOscillateZ_body_hook, SYM(objectOscillateZ_body), b_+296);
  CYC(b_+296, b_+299); A = mem_rd(gb, wRoomLayoutEnd);
  CYC(b_+299, b_+301); alu_cp(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+301, b_+303); goto update_animation; }
  CYC(b_+301, b_+303);
  CYC(b_+303, b_+306); push_effect(gb, b_+306); seasonsFairyCreatePuff(gb);
  CYC(b_+306, b_+309); TAIL(interactionDelete);
}
