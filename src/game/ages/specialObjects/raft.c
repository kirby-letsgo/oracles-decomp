#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

#define raftDismountTileOffsets_bank06 SYM(specialObjectCode_raft__dismountTileOffsets_b06)
#define raftWallPositionOffsets_bank06 SYM(specialObjectCode_raft__raftCalculateAdjacentWallsBitset__wallPositionOffsets_b06)
#define raftValidTiles_bank06 SYM(specialObjectCode_raft__raftCalculateAdjacentWallsBitset__validTiles_b06)

static uint16_t raft_jump_table(GB *gb) {
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

static void raft_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void raftCalculateAdjacentWallsBitset(GB *gb, uint16_t sp0_) {
  BASE(specialObjectCode_raft_b06);
  CYC(b_+355, b_+357); A = 0x01;
  CYC(b_+357, b_+359); H8(hFF8B) = A;
  CYC(b_+359, b_+362); SET_HL(raftWallPositionOffsets_bank06);
again:
  CYC(b_+362, b_+363); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+363, b_+364); B = A;
  CYC(b_+364, b_+365); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+365, b_+366); C = A;
  CYC(b_+366, b_+367); push_effect(gb, HL);
  CALL_C(b_+367, objectGetRelativeTile_hook, SYM(objectGetRelativeTile), b_+370);
  CYC(b_+370, b_+371); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+371, b_+373);
  } else {
    CYC(b_+371, b_+373);
    CYC(b_+373, b_+374); E = A;
    CYC(b_+374, b_+377); SET_HL(raftValidTiles_bank06);
    CALL_C(b_+377, findByteAtHl_hook, SYM(findByteAtHl), b_+380);
    CYC(b_+380, b_+381); alu_ccf(gb);
  }
  CYC(b_+381, b_+382); SET_HL(pop_effect(gb));
  CYC(b_+382, b_+384); A = H8(hFF8B);
  CYC(b_+384, b_+385); alu_rla(gb);
  CYC(b_+385, b_+387); H8(hFF8B) = A;
  if (!(F & FC)) {
    CYCT(b_+387, b_+389);
    goto again;
  }
  CYC(b_+387, b_+389);
  CYC(b_+389, b_+391); E = 0x33;
  CYC(b_+391, b_+392); mem_wr(gb, DE, A);
  CYC(b_+392, b_+393); ret_effect(gb);
}

static void raftTransferKnockbackToLink(GB *gb) {
  BASE(specialObjectCode_raft_b06);
  CYC(b_+417, b_+420); SET_HL(w1Link_invincibilityCounter);
  CYC(b_+420, b_+421); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+421, b_+422); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+422, b_+424);
    goto end;
  }
  CYC(b_+422, b_+424);
  CYC(b_+424, b_+425); E = L;
  CYC(b_+425, b_+426); A = mem_rd(gb, DE);
  CYC(b_+426, b_+427); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+427, b_+428); ret_effect(gb); return;
  }
  CYC(b_+427, b_+428);
  CYC(b_+428, b_+429); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+429, b_+430); E = alu_inc8(gb, E);
  CYC(b_+430, b_+431); A = mem_rd(gb, DE);
  CYC(b_+431, b_+432); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+432, b_+433); E = alu_inc8(gb, E);
  CYC(b_+433, b_+434); A = mem_rd(gb, DE);
  CYC(b_+434, b_+435); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+435, b_+436); E = alu_inc8(gb, E);
  CYC(b_+436, b_+437); A = mem_rd(gb, DE);
  CYC(b_+437, b_+438); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+438, b_+440); E = 0x25;
  CYC(b_+440, b_+441); A = mem_rd(gb, DE);
  CYC(b_+441, b_+442); L = E;
  CYC(b_+442, b_+443); mem_wr(gb, HL, A);
end:
  CYC(b_+443, b_+445); E = 0x2a;
  CYC(b_+445, b_+446); alu_xor(gb, A);
  CYC(b_+446, b_+447); mem_wr(gb, DE, A);
  CYC(b_+447, b_+448); E = alu_inc8(gb, E);
  CYC(b_+448, b_+449); mem_wr(gb, DE, A);
  CYC(b_+449, b_+450); ret_effect(gb);
}

void specialObjectCode_raft_b06_hook(GB *gb) {
  BASE(specialObjectCode_raft_b06);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = D;
  CYC(b_+1, b_+4); W8(wLinkRidingObject) = A;
  CYC(b_+4, b_+6); E = 0x04;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+8); push_effect(gb, b_+8);
  do { uint16_t jt_ = (raft_jump_table(gb));
    if (jt_ == b_+16) { goto state0; }
    else if (jt_ == b_+55) { goto state1; }
    else if (jt_ == b_+296) { goto state2; }
    else if (jt_ == b_+319) { goto state3; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CYC(b_+16, b_+19); SET_HL((SYM(nextToKeyDoor) + 49));
  CYC(b_+19, b_+21); E = 0x05;
  CALL_C(b_+21, interBankCall_hook, 0x008a, b_+24);
  CYC(b_+24, b_+25); alu_xor(gb, A);
  CALL_C(b_+25, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+28);
  CALL_C(b_+28, itemIncState_hook, SYM(itemIncState), b_+31);
  CYC(b_+31, b_+33); L = 0x24;
  CYC(b_+33, b_+35); A = 0x80;
  CYC(b_+35, b_+36); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+36, b_+37); L = alu_inc8(gb, L);
  CYC(b_+37, b_+39); A = 0x06;
  CYC(b_+39, b_+40); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+40, b_+41); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+41, b_+43); L = 0x06;
  CYC(b_+43, b_+45); mem_wr(gb, HL, 0x0c);
  CYC(b_+45, b_+46); A = D;
  CYC(b_+46, b_+49); W8(wLinkObjectIndex) = A;
  CALL_C(b_+49, setCameraFocusedObjectToLink_hook, SYM(setCameraFocusedObjectToLink), b_+52);
  CYC(b_+52, b_+55);
  goto saveRaftPosition;

state1:
  CYC(b_+55, b_+58); A = W8(wPaletteThread_mode);
  CYC(b_+58, b_+59); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+59, b_+60); ret_effect(gb); return; }
  CYC(b_+59, b_+60);
  CALL_C(b_+60, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+63);
  CYC(b_+63, b_+66); A = W8(wScrollMode);
  CYC(b_+66, b_+68); alu_and(gb, 0x0e);
  if (!(F & FZ)) { CYCT(b_+68, b_+69); ret_effect(gb); return; }
  CYC(b_+68, b_+69);
  CYC(b_+69, b_+72); A = W8(wDisabledObjects);
  CYC(b_+72, b_+74); alu_and(gb, 0x81);
  if (!(F & FZ)) { CYCT(b_+74, b_+75); ret_effect(gb); return; }
  CYC(b_+74, b_+75);
  CYC(b_+75, b_+78); A = W8(wLinkForceState);
  CYC(b_+78, b_+80); alu_cp(gb, 0x02);
  if (F & FZ) {
    CYCT(b_+80, b_+82);
    goto respawning;
  }
  CYC(b_+80, b_+82);
  CYC(b_+82, b_+85); A = W8(w1Link_state);
  CYC(b_+85, b_+87); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(b_+87, b_+89);
    goto updateDirection;
  }
  CYC(b_+87, b_+89);
respawning:
  CYC(b_+89, b_+92); SET_HL(wLinkLocalRespawnY);
  CYC(b_+92, b_+94); E = 0x0b;
  CYC(b_+94, b_+95); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+95, b_+96); mem_wr(gb, DE, A);
  CYC(b_+96, b_+98); E = 0x0d;
  CYC(b_+98, b_+99); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+99, b_+100); mem_wr(gb, DE, A);
  CYC(b_+100, b_+103); TAIL(objectSetInvisible);

updateDirection:
  CALL_C(b_+103, updateCompanionDirectionFromAngle_hook, SYM(updateCompanionDirectionFromAngle), b_+106);
  if (F & FC) {
    CYCT(b_+106, b_+108);
    CALL_C(b_+113, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+116);
  } else {
    CYC(b_+106, b_+108);
    CALL_C(b_+108, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+111);
    CYC(b_+111, b_+113);
  }
  CYC(b_+116, b_+119); push_effect(gb, b_+119); raftCalculateAdjacentWallsBitset(gb, sp0_);
  CYC(b_+119, b_+122); push_effect(gb, b_+122); raftTransferKnockbackToLink(gb);
  CYC(b_+122, b_+125); SET_HL(w1Link_knockbackCounter);
  CYC(b_+125, b_+126); A = mem_rd(gb, HL);
  CYC(b_+126, b_+127); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+127, b_+129);
    goto updateMovement;
  }
  CYC(b_+127, b_+129);
  CYC(b_+129, b_+130); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+130, b_+131); L = alu_dec8(gb, L);
  CYC(b_+131, b_+132); C = mem_rd(gb, HL);
  CYC(b_+132, b_+134); B = 0x28;
  CYC(b_+134, b_+137); SET_HL((SYM(animationData19d96) + 9));
  CYC(b_+137, b_+139); E = 0x05;
  CALL_C(b_+139, interBankCall_hook, 0x008a, b_+142);
  CYC(b_+142, b_+144); A = 0x88;
  CYC(b_+144, b_+147); W8(wcc92) = A;
  CYC(b_+147, b_+149);
  goto notDismounting;

updateMovement:
  CYC(b_+149, b_+151); E = 0x10;
  CYC(b_+151, b_+153); A = 0x23;
  CYC(b_+153, b_+154); mem_wr(gb, DE, A);
  CYC(b_+154, b_+156); E = 0x09;
  CYC(b_+156, b_+159); A = W8(wLinkAngle);
  CYC(b_+159, b_+160); mem_wr(gb, DE, A);
  CYC(b_+160, b_+162); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+162, b_+164);
    goto notDismounting;
  }
  CYC(b_+162, b_+164);
  CYC(b_+164, b_+167); A = W8(wLinkImmobilized);
  CYC(b_+167, b_+168); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+168, b_+170);
    goto notDismounting;
  }
  CYC(b_+168, b_+170);
  CYC(b_+170, b_+173); SET_HL((SYM(animationData19d96) + 1));
  CYC(b_+173, b_+175); E = 0x05;
  CALL_C(b_+175, interBankCall_hook, 0x008a, b_+178);
  CYC(b_+178, b_+179); A = C;
  CYC(b_+179, b_+180); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+180, b_+182);
    goto positionUnchanged;
  }
  CYC(b_+180, b_+182);
  CYC(b_+182, b_+184); A = 0x08;
  CYC(b_+184, b_+187); W8(wcc92) = A;
notDismounting:
  CYC(b_+187, b_+188); H = D;
  CYC(b_+188, b_+190); L = 0x3e;
  CYC(b_+190, b_+192); A = 0xff;
  CYC(b_+192, b_+193); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+193, b_+195); mem_wr(gb, HL, 0x04);
  CYC(b_+195, b_+196); ret_effect(gb); return;

positionUnchanged:
  CYC(b_+196, b_+197); H = D;
  CYC(b_+197, b_+199); E = 0x09;
  CYC(b_+199, b_+200); A = mem_rd(gb, DE);
  CYC(b_+200, b_+202); L = 0x3e;
  CYC(b_+202, b_+203); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+203, b_+204); mem_wr(gb, HL, A); SET_HL(HL + 1);
  if (!(F & FZ)) { CYCT(b_+204, b_+205); ret_effect(gb); return; }
  CYC(b_+204, b_+205);
  CYC(b_+205, b_+206); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+206, b_+207); ret_effect(gb); return; }
  CYC(b_+206, b_+207);
  CYC(b_+207, b_+208); E = alu_dec8(gb, E);
  CYC(b_+208, b_+209); A = mem_rd(gb, DE);
  CYC(b_+209, b_+212); SET_HL(raftDismountTileOffsets_bank06);
  CYC(b_+212, b_+213); raft_add_double_index(gb, b_+213);
  CYC(b_+213, b_+214); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+214, b_+215); C = mem_rd(gb, HL);
  CYC(b_+215, b_+216); H = D;
  CYC(b_+216, b_+218); L = 0x0b;
  CYC(b_+218, b_+219); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+219, b_+220); B = A;
  CYC(b_+220, b_+222); L = 0x0d;
  CYC(b_+222, b_+223); A = mem_rd(gb, HL);
  CYC(b_+223, b_+224); alu_add(gb, C);
  CYC(b_+224, b_+225); C = A;
  CALL_C(b_+225, getTileAtPosition_hook, SYM(getTileAtPosition), b_+228);
  CYC(b_+228, b_+230); H = 0xce;
  CYC(b_+230, b_+231); A = mem_rd(gb, HL);
  CYC(b_+231, b_+232); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+232, b_+234);
    goto checkDismount;
  }
  CYC(b_+232, b_+234);
  CYC(b_+234, b_+236); alu_cp(gb, 0x18);
  if (!(F & FZ)) {
    CYCT(b_+236, b_+238);
    goto notDismounting;
  }
  CYC(b_+236, b_+238);
checkDismount:
  CYC(b_+238, b_+241); SET_HL((SYM(animationData19e8f) + 20));
  CYC(b_+241, b_+243); E = 0x05;
  CALL_C(b_+243, interBankCall_hook, 0x008a, b_+246);
  CYC(b_+246, b_+248); A = H8(hFF8B);
  CYC(b_+248, b_+249); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+249, b_+251);
    goto notDismounting;
  }
  CYC(b_+249, b_+251);
  CYC(b_+251, b_+252); A = alu_inc8(gb, A);
  CYC(b_+252, b_+255); W8(wMenuDisabled) = A;
  CYC(b_+255, b_+257); A = 0x0b;
  CYC(b_+257, b_+260); W8(wLinkForceState) = A;
  CYC(b_+260, b_+262); A = 0x0e;
  CYC(b_+262, b_+265); W8(wLinkStateParameter) = A;
  CALL_C(b_+265, itemUpdateAngle_hook, SYM(itemUpdateAngle), b_+268);
  CYC(b_+268, b_+269); E = L;
  CYC(b_+269, b_+271); H = 0xd0;
  CYC(b_+271, b_+272); A = mem_rd(gb, DE);
  CYC(b_+272, b_+273); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+273, b_+274); E = alu_inc8(gb, E);
  CYC(b_+274, b_+275); A = mem_rd(gb, DE);
  CYC(b_+275, b_+276); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+276, b_+277); A = H;
  CYC(b_+277, b_+280); W8(wLinkObjectIndex) = A;
  CALL_C(b_+280, setCameraFocusedObjectToLink_hook, SYM(setCameraFocusedObjectToLink), b_+283);
  CALL_C(b_+283, itemIncState_hook, SYM(itemIncState), b_+286);
  CYC(b_+286, b_+288);
  goto saveRaftPosition;

state2:
  CYC(b_+296, b_+298); A = 0x80;
  CYC(b_+298, b_+301); W8(wcc92) = A;
  CALL_C(b_+301, itemDecCounter1_hook, SYM(itemDecCounter1), b_+304);
  if (!(F & FZ)) { CYCT(b_+304, b_+305); ret_effect(gb); return; }
  CYC(b_+304, b_+305);
  CYC(b_+305, b_+306); alu_xor(gb, A);
  CYC(b_+306, b_+309); W8(wMenuDisabled) = A;
  CYC(b_+309, b_+311); E = 0x00;
  CYC(b_+311, b_+312); A = alu_inc8(gb, A);
  CYC(b_+312, b_+313); mem_wr(gb, DE, A);
  CALL_C(b_+313, updateLinkLocalRespawnPosition_hook, SYM(updateLinkLocalRespawnPosition), b_+316);
  CALL_C(b_+316, itemIncState_hook, SYM(itemIncState), b_+319);
state3:
  CYC(b_+319, b_+322); SET_BC(0xe602);
  CALL_C(b_+322, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+325);
  if (!(F & FZ)) { CYCT(b_+325, b_+326); ret_effect(gb); return; }
  CYC(b_+325, b_+326);
  CYC(b_+326, b_+328); E = 0x08;
  CYC(b_+328, b_+329); A = mem_rd(gb, DE);
  CYC(b_+329, b_+331); L = 0x48;
  CYC(b_+331, b_+332); mem_wr(gb, HL, A);
  CYC(b_+332, b_+335); TAIL(itemDelete);

saveRaftPosition:
  CYC(b_+335, b_+338); SET_BC(wLastAnimalMountPointY);
  CYC(b_+338, b_+339); H = D;
  CYC(b_+339, b_+341); L = 0x0b;
  CYC(b_+341, b_+342); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+342, b_+343); mem_wr(gb, BC, A);
  CYC(b_+343, b_+344); C = alu_inc8(gb, C);
  CYC(b_+344, b_+345); L = alu_inc8(gb, L);
  CYC(b_+345, b_+346); A = mem_rd(gb, HL);
  CYC(b_+346, b_+347); mem_wr(gb, BC, A);
  CYC(b_+347, b_+350); SET_HL((SYM(linkUpdateDamageToApplyForRings) + 20));
  CYC(b_+350, b_+352); E = 0x05;
  CYC(b_+352, b_+355); interBankCall_hook(gb);
}
