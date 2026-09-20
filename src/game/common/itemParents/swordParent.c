#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t sword_parent_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

enum sword_poke_result {
  SWORD_POKE_ABORT,
  SWORD_POKE_NORMAL,
  SWORD_POKE_TRIGGER,
};

static void sword_parent_check_and_ret_for_poke(GB *gb, uint16_t sp0_, enum sword_poke_result *result) {
  BASE(parentItemCode_sword);
  *result = SWORD_POKE_ABORT;
  CYC(b_+389, b_+390); alu_xor(gb, A);
  CYC(b_+390, b_+392); E = 0x02;
  CYC(b_+392, b_+393); mem_wr(gb, DE, A);
  CYC(b_+393, b_+396); A = W8(w1WeaponItem_var2a);
  CYC(b_+396, b_+398); alu_cp(gb, 0x04);
  if (F & FZ) { CYCT(b_+398, b_+400); goto set_subid; }
  CYC(b_+398, b_+400);
  CYC(b_+400, b_+401); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+401, b_+403); goto escape; }
  CYC(b_+401, b_+403);
  CALL_C(b_+403, checkLinkPushingAgainstWall_hook, SYM(checkLinkPushingAgainstWall), b_+406);
  if (!(F & FC)) {
    CYCT(b_+406, b_+407);
    ret_effect(gb);
    *result = SWORD_POKE_NORMAL;
    return;
  }
  CYC(b_+406, b_+407);

set_subid:
  CYC(b_+407, b_+409); E = 0x02;
  CYC(b_+409, b_+411); A = 0x01;
  CYC(b_+411, b_+412); mem_wr(gb, DE, A);

escape:
  CYC(b_+412, b_+413); SET_HL(pop_effect(gb));
  CYC(b_+413, b_+414); alu_xor(gb, A);
  CYC(b_+414, b_+417); W8(w1WeaponItem_collisionType) = A;
  *result = SWORD_POKE_TRIGGER;
}

static void sword_parent_create_beam(GB *gb, uint16_t sp0_) {
  BASE(parentItemCode_sword);
  CYC(b_+468, b_+471); SET_BC(0x2700);
  CYC(b_+471, b_+473); E = 0x01;
  CALL_C(b_+473, getFreeItemSlotWithObjectCap_hook, SYM(getFreeItemSlotWithObjectCap), b_+476);
  if (F & FC) { CYCT(b_+476, b_+477); ret_effect(gb); return; }
  CYC(b_+476, b_+477);
  CYC(b_+477, b_+478); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+478, b_+479); L = alu_inc8(gb, L);
  CYC(b_+479, b_+480); A = B;
  CYC(b_+480, b_+481); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+481, b_+482); A = C;
  CYC(b_+482, b_+483); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+483, b_+484); push_effect(gb, DE);
  CYC(b_+484, b_+487); SET_DE(w1Link_direction);
  CYC(b_+487, b_+489); L = 0x08;
  CYC(b_+489, b_+491); B = 0x08;
  CALL_C(b_+491, copyMemoryReverse_hook, SYM(copyMemoryReverse), b_+494);
  CYC(b_+494, b_+495); SET_DE(pop_effect(gb));
  CYC(b_+495, b_+496); F = (F & FZ) | FC;
  CYC(b_+496, b_+497); ret_effect(gb);
}

void parentItemCode_sword_hook(GB *gb) {
  BASE(parentItemCode_sword);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, clearParentItemIfCantUseSword_hook, SYM(clearParentItemIfCantUseSword), b_+3);
  CYC(b_+3, b_+5); E = 0x04;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); push_effect(gb, b_+7);
  do { uint16_t jt_ = (sword_parent_jump_table(gb));
    if (jt_ == b_+21) { goto state0; }
    else if (jt_ == b_+73) { goto state1; }
    else if (jt_ == b_+195) { goto state2; }
    else if (jt_ == b_+258) { goto state3; }
    else if (jt_ == b_+331) { goto state4; }
    else if (jt_ == b_+354) { goto state5; }
    else if (jt_ == b_+119) { goto state6; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CYC(b_+21, b_+24); SET_HL(wcc63);
  CYC(b_+24, b_+26); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) CYCT(b_+26, b_+28);
  else {
    CYC(b_+26, b_+28);
    CYC(b_+28, b_+30); mem_wr(gb, HL, 0x00);
    CALL_C(b_+30, updateLinkDirectionFromAngle_hook, SYM(updateLinkDirectionFromAngle), b_+33);
    CYC(b_+33, b_+36); A = W8(wLinkHealth);
    CYC(b_+36, b_+38); alu_cp(gb, 0x05);
    if (F & FC) CYCT(b_+38, b_+40);
    else {
      CYC(b_+38, b_+40);
      CYC(b_+40, b_+42); A = 0x32;
      CALL_C(b_+42, cpActiveRing_hook, SYM(cpActiveRing), b_+45);
      if (!(F & FZ)) CYCT(b_+45, b_+47);
      else {
        CYC(b_+45, b_+47);
        CYC(b_+47, b_+49); E = 0x3a;
        CYC(b_+49, b_+51); A = 0xf8;
        CYC(b_+51, b_+52); mem_wr(gb, DE, A);
      }
    }
  }
  CYC(b_+52, b_+55); SET_HL(w1WeaponItem_enabled);
  CYC(b_+55, b_+56); A = mem_rd(gb, HL);
  CYC(b_+56, b_+57); alu_or(gb, A);
  CYC(b_+57, b_+59); B = 0x40;
  if (!(F & FZ)) CALL_C_CC(b_+59, clearMemory_hook, SYM(clearMemory), b_+62);
  else CYC(b_+59, b_+62);
  CYC(b_+62, b_+63); H = D;
  CYC(b_+63, b_+65); L = 0x00;
  CYC(b_+65, b_+67); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CALL_C(b_+67, parentItemLoadAnimationAndIncState_hook, SYM(parentItemLoadAnimationAndIncState), b_+70);
  CYC(b_+70, b_+73); itemCreateChild_hook(gb); return;

state1:
  CYC(b_+73, b_+76); A = W8(wcc63);
  CYC(b_+76, b_+77); alu_rlca(gb);
  if (F & FC) { CYCT(b_+77, b_+80); goto spin_start; }
  CYC(b_+77, b_+80);
  CALL_C(b_+80, specialObjectAnimate_optimized_hook, SYM(specialObjectAnimate_optimized), b_+83);
  CYC(b_+83, b_+84); H = D;
  CYC(b_+84, b_+86); E = 0x21;
  CYC(b_+86, b_+87); A = mem_rd(gb, DE);
  CYC(b_+87, b_+88); alu_or(gb, A);
  if (F & FZ) CYCT(b_+88, b_+90);
  else {
    CYC(b_+88, b_+90);
    CYC(b_+90, b_+92); L = 0x3a;
    CYC(b_+92, b_+94); alu_bit(gb, 7, mem_rd(gb, HL));
    if (!(F & FZ)) CYCT(b_+94, b_+96);
    else {
      CYC(b_+94, b_+96);
      CYC(b_+96, b_+98); L = 0x00;
      CYC(b_+98, b_+100); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
    }
  }
  CYC(b_+100, b_+101); L = E;
  CYC(b_+101, b_+103); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+103, b_+105); goto state6; }
  CYC(b_+103, b_+105);
  CYC(b_+105, b_+107); alu_bit(gb, 5, A);
  if (F & FZ) { CYCT(b_+107, b_+108); ret_effect(gb); return; }
  CYC(b_+107, b_+108);
  CYC(b_+108, b_+110); mem_wr(gb, HL, mem_rd(gb, HL) & 0xdf);
  CYC(b_+110, b_+113); A = W8(wSwordLevel);
  CYC(b_+113, b_+115); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(b_+115, b_+118); goto check_create_beam; }
  CYC(b_+115, b_+118);
  CYC(b_+118, b_+119); ret_effect(gb); return;

state6:
  CYC(b_+119, b_+122); A = W8(w1WeaponItem_var2a);
  CYC(b_+122, b_+123); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+123, b_+126); goto enemy_contact; }
  CYC(b_+123, b_+126);
  CYC(b_+126, b_+129); A = W8(wLinkObjectIndex);
  CYC(b_+129, b_+130); alu_rrca(gb);
  if (F & FC) { CYCT(b_+130, b_+133); goto delete_self; }
  CYC(b_+130, b_+133);
  CALL_C(b_+133, parentItemCheckButtonPressed_hook, SYM(parentItemCheckButtonPressed), b_+136);
  if (F & FZ) { CYCT(b_+136, b_+139); goto delete_self; }
  CYC(b_+136, b_+139);
  CYC(b_+139, b_+141); A = 0x01;
  CYC(b_+141, b_+144); W8(wcc63) = A;
  CYC(b_+144, b_+145); A = alu_inc8(gb, A);
  CYC(b_+145, b_+148); W8(w1WeaponItem_state) = A;
  CYC(b_+148, b_+150); A = 0x89;
  CYC(b_+150, b_+153); W8(w1WeaponItem_collisionType) = A;
  CYC(b_+153, b_+155); L = 0x04;
  CYC(b_+155, b_+157); mem_wr(gb, HL, 0x02);
  CYC(b_+157, b_+158); L = alu_inc8(gb, L);
  CYC(b_+158, b_+159); alu_xor(gb, A);
  CYC(b_+159, b_+160); mem_wr(gb, HL, A);
  CYC(b_+160, b_+162); L = 0x3a;
  CYC(b_+162, b_+163); mem_wr(gb, HL, A);
  CYC(b_+163, b_+165); L = 0x3f;
  CYC(b_+165, b_+166); mem_wr(gb, HL, A);
  CYC(b_+166, b_+168); L = 0x06;
  CYC(b_+168, b_+170); mem_wr(gb, HL, 0x28);
  CYC(b_+170, b_+173); itemEnableLinkMovement_hook(gb); return;

enemy_contact:
  CYC(b_+173, b_+175); alu_bit(gb, 0, A);
  if (F & FZ) { CYCT(b_+175, b_+178); goto delete_self; }
  CYC(b_+175, b_+178);
  CYC(b_+178, b_+180); E = 0x3a;
  CYC(b_+180, b_+181); A = mem_rd(gb, DE);
  CYC(b_+181, b_+182); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+182, b_+185); goto delete_self; }
  CYC(b_+182, b_+185);
  CYC(b_+185, b_+188); SET_HL(w1Link_damageToApply);
  CYC(b_+188, b_+189); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+189, b_+190); mem_wr(gb, HL, A);
  CYC(b_+190, b_+191); alu_xor(gb, A);
  CYC(b_+191, b_+192); mem_wr(gb, DE, A);
  CYC(b_+192, b_+195); goto delete_self;

state2:
  CYC(b_+195, b_+198); A = W8(wLinkObjectIndex);
  CYC(b_+198, b_+199); alu_rrca(gb);
  if (F & FC) { CYCT(b_+199, b_+202); goto delete_self; }
  CYC(b_+199, b_+202);
  CALL_C(b_+202, parentItemCheckButtonPressed_hook, SYM(parentItemCheckButtonPressed), b_+205);
  if (F & FZ) { CYCT(b_+205, b_+208); goto delete_self; }
  CYC(b_+205, b_+208);
  CYC(b_+208, b_+211); push_effect(gb, b_+211);
  enum sword_poke_result poke_result;
  sword_parent_check_and_ret_for_poke(gb, sp0_, &poke_result);
  if (poke_result == SWORD_POKE_ABORT) return;
  if (poke_result == SWORD_POKE_TRIGGER) goto trigger_poke;
  CYC(b_+211, b_+213); A = 0x16;
  CALL_C(b_+213, cpActiveRing_hook, SYM(cpActiveRing), b_+216);
  CYC(b_+216, b_+218); C = 0x01;
  if (!(F & FZ)) CYCT(b_+218, b_+220);
  else { CYC(b_+218, b_+220); CYC(b_+220, b_+222); C = 0x04; }
  CYC(b_+222, b_+224); L = 0x06;
  CYC(b_+224, b_+225); A = mem_rd(gb, HL);
  CYC(b_+225, b_+226); alu_sub(gb, C);
  CYC(b_+226, b_+227); mem_wr(gb, HL, A);
  if (!(F & FC)) { CYCT(b_+227, b_+228); ret_effect(gb); return; }
  CYC(b_+227, b_+228);
  CYC(b_+228, b_+230); A = 0x31;
  CALL_C(b_+230, cpActiveRing_hook, SYM(cpActiveRing), b_+233);
  if (!(F & FZ)) { CYCT(b_+233, b_+235); goto charge_without_beam; }
  CYC(b_+233, b_+235);
  CYC(b_+235, b_+238); push_effect(gb, b_+238); sword_parent_create_beam(gb, sp0_);
  CYC(b_+238, b_+241); goto trigger_poke;

charge_without_beam:
  CYC(b_+241, b_+243); L = 0x04;
  CYC(b_+243, b_+244); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+244, b_+246); L = 0x00;
  CYC(b_+246, b_+248); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+248, b_+250); A = 0x03;
  CYC(b_+250, b_+253); W8(w1WeaponItem_state) = A;
  CYC(b_+253, b_+255); A = 0x4f;
  CYC(b_+255, b_+258); playSound_b00_hook(gb); return;

state3:
  CYC(b_+258, b_+261); push_effect(gb, b_+261);
  sword_parent_check_and_ret_for_poke(gb, sp0_, &poke_result);
  if (poke_result == SWORD_POKE_ABORT) return;
  if (poke_result == SWORD_POKE_TRIGGER) goto trigger_poke;
  CALL_C(b_+261, parentItemCheckButtonPressed_hook, SYM(parentItemCheckButtonPressed), b_+264);
  if (!(F & FZ)) { CYCT(b_+264, b_+265); ret_effect(gb); return; }
  CYC(b_+264, b_+265);

spin_start:
  CYC(b_+265, b_+266); H = D;
  CYC(b_+266, b_+268); A = 0x02;
  CYC(b_+268, b_+271); W8(wcc63) = A;
  CYC(b_+271, b_+273); L = 0x04;
  CYC(b_+273, b_+275); mem_wr(gb, HL, 0x04);
  CYC(b_+275, b_+277); A = 0x2f;
  CALL_C(b_+277, cpActiveRing_hook, SYM(cpActiveRing), b_+280);
  CYC(b_+280, b_+282); A = 0x05;
  if (!(F & FZ)) CYCT(b_+282, b_+284);
  else { CYC(b_+282, b_+284); CYC(b_+284, b_+286); A = 0x09; }
  CYC(b_+286, b_+288); L = 0x06;
  CYC(b_+288, b_+289); mem_wr(gb, HL, A);
  CYC(b_+289, b_+291); L = 0x3f;
  CYC(b_+291, b_+293); mem_wr(gb, HL, 0x0f);
  CALL_C(b_+293, isLinkUnderwater_hook, SYM(isLinkUnderwater), b_+296);
  CYC(b_+296, b_+298); C = 0x28;
  if (F & FZ) CYCT(b_+298, b_+300);
  else { CYC(b_+298, b_+300); CYC(b_+300, b_+302); C = 0x30; }
  CYC(b_+302, b_+305); A = W8(w1Link_direction);
  CYC(b_+305, b_+306); alu_add(gb, C);
  CALL_C(b_+306, specialObjectSetAnimationWithLinkData_hook, SYM(specialObjectSetAnimationWithLinkData), b_+309);
  CYC(b_+309, b_+310); H = D;
  CYC(b_+310, b_+312); L = 0x21;
  CYC(b_+312, b_+314); mem_wr(gb, HL, mem_rd(gb, HL) & 0xbf);
  CYC(b_+314, b_+317); SET_HL(w1WeaponItem_state);
  CYC(b_+317, b_+319); mem_wr(gb, HL, 0x04);
  CYC(b_+319, b_+321); L = 0x3a;
  CYC(b_+321, b_+323); mem_wr(gb, HL, alu_sla(gb, mem_rd(gb, HL)));
  CALL_C(b_+323, itemDisableLinkMovement_hook, SYM(itemDisableLinkMovement), b_+326);
  CYC(b_+326, b_+328); A = 0x6b;
  CYC(b_+328, b_+331); playSound_b00_hook(gb); return;

state4:
  CALL_C(b_+331, specialObjectAnimate_optimized_hook, SYM(specialObjectAnimate_optimized), b_+334);
  CYC(b_+334, b_+335); H = D;
  CYC(b_+335, b_+337); L = 0x21;
  CYC(b_+337, b_+339); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+339, b_+340); ret_effect(gb); return; }
  CYC(b_+339, b_+340);
  CYC(b_+340, b_+342); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+342, b_+344); L = 0x06;
  CYC(b_+344, b_+345); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+345, b_+346); ret_effect(gb); return; }
  CYC(b_+345, b_+346);
  CYC(b_+346, b_+348); A = 0x05;
  CYC(b_+348, b_+351); W8(w1WeaponItem_state) = A;
  CYC(b_+351, b_+354); goto delete_self;

state5:
  CALL_C(b_+354, specialObjectAnimate_optimized_hook, SYM(specialObjectAnimate_optimized), b_+357);
  CYC(b_+357, b_+358); H = D;
  CYC(b_+358, b_+360); L = 0x21;
  CYC(b_+360, b_+362); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+362, b_+363); ret_effect(gb); return; }
  CYC(b_+362, b_+363);
  CYC(b_+363, b_+365); L = 0x02;
  CYC(b_+365, b_+366); A = mem_rd(gb, HL);
  CYC(b_+366, b_+367); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+367, b_+369); goto delete_self; }
  CYC(b_+367, b_+369);
  CYC(b_+369, b_+371); A = 0x06;
  CYC(b_+371, b_+374); W8(w1WeaponItem_state) = A;
  CYC(b_+374, b_+376); L = 0x04;
  CYC(b_+376, b_+377); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+377, b_+378); alu_xor(gb, A);
  CYC(b_+378, b_+381); W8(w1WeaponItem_var2a) = A;
  CYC(b_+381, b_+382); ret_effect(gb); return;

delete_self:
  CYC(b_+382, b_+383); alu_xor(gb, A);
  CYC(b_+383, b_+386); W8(wcc63) = A;
  CYC(b_+386, b_+389); clearParentItem_hook(gb); return;

trigger_poke:
  CYC(b_+417, b_+418); H = D;
  CYC(b_+418, b_+420); L = 0x3f;
  CYC(b_+420, b_+422); mem_wr(gb, HL, 0x08);
  CYC(b_+422, b_+424); L = 0x04;
  CYC(b_+424, b_+426); mem_wr(gb, HL, 0x05);
  CALL_C(b_+426, itemDisableLinkMovement_hook, SYM(itemDisableLinkMovement), b_+429);
  CALL_C(b_+429, isLinkUnderwater_hook, SYM(isLinkUnderwater), b_+432);
  CYC(b_+432, b_+434); A = 0x1f;
  if (F & FZ) CYCT(b_+434, b_+436);
  else { CYC(b_+434, b_+436); CYC(b_+436, b_+438); A = 0x2c; }
  CYC(b_+438, b_+441); specialObjectSetAnimationWithLinkData_hook(gb); return;

check_create_beam:
  CYC(b_+441, b_+443); C = 0x08;
  CYC(b_+443, b_+445); A = 0x17;
  CALL_C(b_+445, cpActiveRing_hook, SYM(cpActiveRing), b_+448);
  if (F & FZ) CYCT(b_+448, b_+450);
  else {
    CYC(b_+448, b_+450);
    CYC(b_+450, b_+452); C = 0x0c;
    CYC(b_+452, b_+454); A = 0x18;
    CALL_C(b_+454, cpActiveRing_hook, SYM(cpActiveRing), b_+457);
    if (F & FZ) CYCT(b_+457, b_+459);
    else { CYC(b_+457, b_+459); CYC(b_+459, b_+461); C = 0x00; }
  }
  CYC(b_+461, b_+464); SET_HL(wLinkHealth);
  CYC(b_+464, b_+465); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+465, b_+466); alu_add(gb, C);
  CYC(b_+466, b_+467); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+467, b_+468); ret_effect(gb); return; }
  CYC(b_+467, b_+468);
  sword_parent_create_beam(gb, sp0_);
}
