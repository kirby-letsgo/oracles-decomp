#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t itemFromMaple_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void itemFromMaple_addAToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

static void itemFromMaple_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

/* PARTSTATUS_JUST_HIT is unreachable via the top-level cp/jr since @normalStatus is fallen
   through immediately below; both partCode14 and partCode15 alias the same code. */

static void itemFromMaple_setOamData(GB *gb) {
  BASE(partCode14);
  CYC(b_+429, b_+431); E = 0xc2; // Part.subid
  CYC(b_+431, b_+432); A = mem_rd(gb, DE);
  CYC(b_+432, b_+433); C = A;
  CYC(b_+433, b_+434); alu_add(gb, A);
  CYC(b_+434, b_+435); alu_add(gb, C);
  CYC(b_+435, b_+438); SET_HL(b_+454);
  CYC(b_+438, b_+439); itemFromMaple_addAToHl_from_rst(gb, b_+439);
  CYC(b_+439, b_+441); E = 0xdd; // Part.oamTileIndexBase
  CYC(b_+441, b_+442); A = mem_rd(gb, DE);
  CYC(b_+442, b_+443); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+443, b_+444); mem_wr(gb, DE, A);
  CYC(b_+444, b_+445); SET_HL(HL + 1);
  CYC(b_+445, b_+446); E = alu_dec8(gb, E);
  CYC(b_+446, b_+447); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+447, b_+448); mem_wr(gb, DE, A);
  CYC(b_+448, b_+449); E = alu_dec8(gb, E);
  CYC(b_+449, b_+450); mem_wr(gb, DE, A);
  CYC(b_+450, b_+451); A = mem_rd(gb, HL);
  CYC(b_+451, b_+454); partSetAnimation_hook(gb); return; // jp
}

static void itemFromMaple_setDroppedItemPosition(GB *gb) {
  BASE(partCode14);
  CYC(b_+496, b_+497); H = D;
  CYC(b_+497, b_+499); L = 0xcb; // Part.yh
  CYC(b_+499, b_+500); A = mem_rd(gb, HL);
  CYC(b_+500, b_+502); alu_cp(gb, 0xf0);
  if (F & FC) { CYCT(b_+502, b_+504); goto L_4f0c; } // jr c
  CYC(b_+502, b_+504);
  CYC(b_+504, b_+505); alu_xor(gb, A);
L_4f0c:
  CYC(b_+505, b_+507); alu_cp(gb, 0x20);
  if (!(F & FC)) { CYCT(b_+507, b_+509); goto L_4f14; } // jr nc
  CYC(b_+507, b_+509);
  CYC(b_+509, b_+511); mem_wr(gb, HL, 0x20);
  CYC(b_+511, b_+513); goto L_4f1a; // jr
L_4f14:
  CYC(b_+513, b_+515); alu_cp(gb, 0x78);
  if (F & FC) { CYCT(b_+515, b_+517); goto L_4f1a; } // jr c
  CYC(b_+515, b_+517);
  CYC(b_+517, b_+519); mem_wr(gb, HL, 0x78);
L_4f1a:
  CYC(b_+519, b_+521); L = 0xcd; // Part.xh
  CYC(b_+521, b_+522); A = mem_rd(gb, HL);
  CYC(b_+522, b_+524); alu_cp(gb, 0xf0);
  if (F & FC) { CYCT(b_+524, b_+526); goto L_4f22; } // jr c
  CYC(b_+524, b_+526);
  CYC(b_+526, b_+527); alu_xor(gb, A);
L_4f22:
  CYC(b_+527, b_+529); alu_cp(gb, 0x08);
  if (!(F & FC)) { CYCT(b_+529, b_+531); goto L_4f29; } // jr nc
  CYC(b_+529, b_+531);
  CYC(b_+531, b_+533); mem_wr(gb, HL, 0x08);
  RET(b_+533); return; // ret
L_4f29:
  CYC(b_+534, b_+536); alu_cp(gb, 0x98);
  if (F & FC) { RET_TAKEN(b_+536); return; } // ret c
  CYC(b_+536, b_+537);
  CYC(b_+537, b_+539); mem_wr(gb, HL, 0x98);
  RET(b_+539); return; // ret
}

void itemFromMaple_moveToMaple_hook(GB *gb) {
  BASE(partCode14);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+540, b_+542); L = 0x0b; // <w1Companion.yh
  CYC(b_+542, b_+543); B = mem_rd(gb, HL);
  CYC(b_+543, b_+545); L = 0x0d; // <w1Companion.xh
  CYC(b_+545, b_+546); C = mem_rd(gb, HL);
  CYC(b_+546, b_+547); push_effect(gb, BC);
  CALL_C(b_+547, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+550);
  CYC(b_+550, b_+552); E = 0xc9; // Part.angle
  CYC(b_+552, b_+553); mem_wr(gb, DE, A);
  CALL_C(b_+553, objectApplySpeed_hook, SYM(objectApplySpeed), b_+556);
  CYC(b_+556, b_+557); SET_BC(pop_effect(gb));
  CYC(b_+557, b_+558); H = D;
  CYC(b_+558, b_+560); L = 0xcb; // Part.yh
  CYC(b_+560, b_+561); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+561, b_+562); alu_cp(gb, B);
  if (!(F & FZ)) { RET_TAKEN(b_+562); return; } // ret nz
  CYC(b_+562, b_+563);
  CYC(b_+563, b_+564); L = alu_inc8(gb, L);
  CYC(b_+564, b_+565); A = mem_rd(gb, HL);
  CYC(b_+565, b_+566); alu_cp(gb, C);
  RET(b_+566); return; // ret
}

void partCode14_hook(GB *gb);

void partCode14_hook(GB *gb) {
  BASE(partCode14);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc2; // Part.subid
  if (F & FZ) { CYCT(b_+2, b_+4); goto normalStatus; } // jr z
  CYC(b_+2, b_+4);
  CYC(b_+4, b_+6); alu_cp(gb, 0x02); // PARTSTATUS_DEAD
  if (F & FZ) { CYCT(b_+6, b_+9); goto linkCollectedItem; } // jp z
  CYC(b_+6, b_+9);
  CYC(b_+9, b_+10); H = D;
  CYC(b_+10, b_+12); L = 0xc2; // Part.subid
  CYC(b_+12, b_+14); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+14, b_+16); L = 0xc4; // Part.state
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x03);
  CYC(b_+18, b_+19); L = alu_inc8(gb, L);
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x00);

normalStatus:
  CYC(b_+21, b_+23); E = 0xc4; // Part.state
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  {
    CYC(b_+24, b_+25); push_effect(gb, b_+25);
    uint16_t target = itemFromMaple_jump_table(gb);
    if (target == b_+35) goto state0;
    if (target == b_+112) goto state1;
    if (target == b_+136) goto state3;
    if (target == b_+185) goto state4;
    objectReplaceWithAnimationIfOnHazard_hook(gb); return;
  }

state0:
  CYC(b_+35, b_+36); H = D;
  CYC(b_+36, b_+37); L = E; // Part.state
  CYC(b_+37, b_+38); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+38, b_+40); L = 0xe6; // Part.collisionRadiusY
  CYC(b_+40, b_+42); A = 0x06;
  CYC(b_+42, b_+43); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+43, b_+44); mem_wr(gb, HL, A);
  CALL_C(b_+44, getRandomNumber_hook, SYM(getRandomNumber), b_+47);
  CYC(b_+47, b_+48); B = A;
  CYC(b_+48, b_+50); alu_and(gb, 0x70);
  CYC(b_+50, b_+52); A = alu_swap(gb, A);
  CYC(b_+52, b_+55); SET_HL(b_+88); // @speedValues
  CYC(b_+55, b_+56); itemFromMaple_addAToHl_from_rst(gb, b_+56);
  CYC(b_+56, b_+58); E = 0xd0; // Part.speed
  CYC(b_+58, b_+59); A = mem_rd(gb, HL);
  CYC(b_+59, b_+60); mem_wr(gb, DE, A);
  CYC(b_+60, b_+61); A = B;
  CYC(b_+61, b_+63); alu_and(gb, 0x0e);
  CYC(b_+63, b_+66); SET_HL(b_+96); // @speedZValues
  CYC(b_+66, b_+67); itemFromMaple_addAToHl_from_rst(gb, b_+67);
  CYC(b_+67, b_+69); E = 0xd4; // Part.speedZ
  CYC(b_+69, b_+70); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+70, b_+71); mem_wr(gb, DE, A);
  CYC(b_+71, b_+72); E = alu_inc8(gb, E);
  CYC(b_+72, b_+73); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+73, b_+74); mem_wr(gb, DE, A);
  CALL_C(b_+74, getRandomNumber_hook, SYM(getRandomNumber), b_+77);
  CYC(b_+77, b_+79); E = 0xc9; // Part.angle
  CYC(b_+79, b_+81); alu_and(gb, 0x1f);
  CYC(b_+81, b_+82); mem_wr(gb, DE, A);
  CYC(b_+82, b_+85); push_effect(gb, b_+85); itemFromMaple_setOamData(gb);
  CYC(b_+85, b_+88); objectSetVisiblec3_hook(gb); return; // jp

state1:
  CALL_C(b_+112, objectApplySpeed_hook, SYM(objectApplySpeed), b_+115);
  CYC(b_+115, b_+118); push_effect(gb, b_+118); itemFromMaple_setDroppedItemPosition(gb);
  CYC(b_+118, b_+120); C = 0x20;
  CALL_C(b_+120, objectUpdateSpeedZAndBounce_hook, SYM(objectUpdateSpeedZAndBounce), b_+123);
  if (!(F & FC)) { CYCT(b_+123, b_+125); goto L_4d98; } // jr nc
  CYC(b_+123, b_+125);
  CYC(b_+125, b_+126); H = D;
  CYC(b_+126, b_+128); L = 0xe4; // Part.collisionType
  CYC(b_+128, b_+130); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+130, b_+132); L = 0xc4; // Part.state
  CYC(b_+132, b_+133); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
L_4d98:
  CYC(b_+133, b_+136); objectReplaceWithAnimationIfOnHazard_hook(gb); return; // jp

state3:
  CYC(b_+136, b_+137); E = alu_inc8(gb, E); // Part.substate
  CYC(b_+137, b_+138); A = mem_rd(gb, DE);
  CYC(b_+138, b_+139); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+139, b_+141); goto L_4db3; } // jr nz
  CYC(b_+139, b_+141);
  CYC(b_+141, b_+142); H = D;
  CYC(b_+142, b_+143); L = E; // Part.substate
  CYC(b_+143, b_+144); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+144, b_+146); L = 0xcf; // Part.zh
  CYC(b_+146, b_+148); mem_wr(gb, HL, 0x00);
  CYC(b_+148, b_+150); A = 0x01; // Object.id
  CALL_C(b_+150, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+153);
  CYC(b_+153, b_+154); A = mem_rd(gb, HL);
  CYC(b_+154, b_+156); E = 0xf0; // Part.var30
  CYC(b_+156, b_+157); mem_wr(gb, DE, A);
  CALL_C(b_+157, objectSetVisible80_hook, SYM(objectSetVisible80), b_+160);
L_4db3:
  CALL_C(b_+160, objectCheckCollidedWithLink_hook, SYM(objectCheckCollidedWithLink), b_+163);
  if (F & FC) { CYCT(b_+163, b_+166); goto linkCollectedItem; } // jp c
  CYC(b_+163, b_+166);
  CYC(b_+166, b_+168); A = 0x00; // Object.enabled
  CALL_C(b_+168, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+171);
  CYC(b_+171, b_+172); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+172, b_+173); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+173, b_+175); goto L_4dc9; } // jr z
  CYC(b_+173, b_+175);
  CYC(b_+175, b_+177); E = 0xf0; // Part.var30
  CYC(b_+177, b_+178); A = mem_rd(gb, DE);
  CYC(b_+178, b_+179); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+179, b_+182); objectTakePosition_hook(gb); return; } // jp z
  CYC(b_+179, b_+182);
L_4dc9:
  CYC(b_+182, b_+185); partDelete_hook(gb); return; // jp

state4:
  CYC(b_+185, b_+186); E = alu_inc8(gb, E); // Part.substate
  CYC(b_+186, b_+187); A = mem_rd(gb, DE);
  {
    CYC(b_+187, b_+188); push_effect(gb, b_+188);
    uint16_t target = itemFromMaple_jump_table(gb);
    if (target == b_+196) goto substate0;
    if (target == b_+211) goto substate1;
    if (target == b_+235) goto substate2;
    goto substate3;
  }

substate0:
  CYC(b_+196, b_+197); H = D;
  CYC(b_+197, b_+198); L = E; // Part.substate
  CYC(b_+198, b_+199); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+199, b_+202); A = mem_rd(gb, w1Companion_damage); // w1Companion.damage
  CYC(b_+202, b_+203); A = alu_dec8(gb, A);
  CYC(b_+203, b_+205); L = 0xd0; // Part.speed
  CYC(b_+205, b_+207); mem_wr(gb, HL, 0x14); // SPEED_80
  if (F & FZ) { CYCT(b_+207, b_+209); goto substate1; } // jr z
  CYC(b_+207, b_+209);
  CYC(b_+209, b_+211); mem_wr(gb, HL, 0x28); // SPEED_100

substate1:
  CYC(b_+211, b_+214); SET_HL(w1Companion_damage); // w1Companion.damage
  CYC(b_+214, b_+215); A = mem_rd(gb, HL);
  CYC(b_+215, b_+216); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+216, b_+218); goto L_4e09; } // jr z
  CYC(b_+216, b_+218);
  CYC(b_+218, b_+221); push_effect(gb, b_+221); itemFromMaple_moveToMaple_hook(gb);
  if (!(F & FZ)) { RET_TAKEN(b_+221); return; } // ret nz
  CYC(b_+221, b_+222);
  CYC(b_+222, b_+224); L = 0xc5; // Part.substate
  CYC(b_+224, b_+225); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+225, b_+227); L = 0xe4; // Part.collisionType
  CYC(b_+227, b_+229); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+229, b_+232); SET_BC(0xffc0);
  CYC(b_+232, b_+235); objectSetSpeedZ_hook(gb); return; // jp

substate2:
  CYC(b_+235, b_+237); C = 0x00;
  CALL_C(b_+237, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+240);
  CYC(b_+240, b_+242); E = 0xcf; // Part.zh
  CYC(b_+242, b_+243); A = mem_rd(gb, DE);
  CYC(b_+243, b_+245); alu_cp(gb, 0xf7);
  if (!(F & FC)) { RET_TAKEN(b_+245); return; } // ret nc
  CYC(b_+245, b_+246);
L_4e09:
  CYC(b_+246, b_+248); A = 0x01;
  CYC(b_+248, b_+251); mem_wr(gb, w1Companion_damageToApply, A); // w1Companion.damageToApply
  CYC(b_+251, b_+252); H = D;
  CYC(b_+252, b_+254); L = 0xc5; // Part.substate
  CYC(b_+254, b_+256); mem_wr(gb, HL, 0x03);
  CYC(b_+256, b_+258); L = 0xc3; // Part.var03
  CYC(b_+258, b_+260); mem_wr(gb, HL, 0x00);
  RET(b_+260); return; // ret

substate3:
  CYC(b_+261, b_+263); E = 0xc3; // Part.var03
  CYC(b_+263, b_+264); A = mem_rd(gb, DE);
  CYC(b_+264, b_+265); alu_rlca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+265); return; } // ret nc
  CYC(b_+265, b_+266);
  CYC(b_+266, b_+269); partDelete_hook(gb); return; // jp

linkCollectedItem:
  CYC(b_+269, b_+272); A = mem_rd(gb, wDisabledObjects); // wDisabledObjects
  CYC(b_+272, b_+274); alu_bit(gb, 0, A);
  if (!(F & FZ)) { RET_TAKEN(b_+274); return; } // ret nz
  CYC(b_+274, b_+275);
  CYC(b_+275, b_+277); E = 0xc2; // Part.subid
  CYC(b_+277, b_+278); A = mem_rd(gb, DE);
  CYC(b_+278, b_+280); alu_and(gb, 0x7f);
  CYC(b_+280, b_+283); SET_HL(b_+567); // @obtainedValue
  CYC(b_+283, b_+284); itemFromMaple_addAToHl_from_rst(gb, b_+284);
  CYC(b_+284, b_+287); A = mem_rd(gb, w1Companion_var2a); // w1Companion.var2a
  CYC(b_+287, b_+288); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+288, b_+291); mem_wr(gb, w1Companion_var2a, A); // w1Companion.var2a
  CYC(b_+291, b_+292); A = mem_rd(gb, DE);
  CYC(b_+292, b_+294); alu_and(gb, 0x7f);
  if (F & FZ) { CYCT(b_+294, b_+296); goto func_4e6e; } // jr z
  CYC(b_+294, b_+296);
  CYC(b_+296, b_+297); alu_add(gb, A);
  CYC(b_+297, b_+300); SET_HL(b_+373); // @itemDropTreasureTable
  CYC(b_+300, b_+301); itemFromMaple_addDoubleIndexToHl_from_rst(gb, b_+301);
  CYC(b_+301, b_+302); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+302, b_+303); B = A;
  CYC(b_+303, b_+305); A = 0x26; // GOLD_JOY_RING
  CALL_C(b_+305, cpActiveRing_hook, SYM(cpActiveRing), b_+308);
  CYC(b_+308, b_+309); A = mem_rd(gb, HL); SET_HL(HL + 1);
  if (F & FZ) { CYCT(b_+309, b_+311); goto L_4e53; } // jr z
  CYC(b_+309, b_+311);
  CYC(b_+311, b_+313); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+313, b_+315); goto L_4e54; } // jr z
  CYC(b_+313, b_+315);
  CALL_C(b_+315, cpActiveRing_hook, SYM(cpActiveRing), b_+318);
  if (!(F & FZ)) { CYCT(b_+318, b_+320); goto L_4e54; } // jr nz
  CYC(b_+318, b_+320);
L_4e53:
  CYC(b_+320, b_+321); SET_HL(HL + 1);
L_4e54:
  CYC(b_+321, b_+322); C = mem_rd(gb, HL);
  CYC(b_+322, b_+323); A = B;
  CYC(b_+323, b_+325); alu_cp(gb, 0x2d); // TREASURE_RING
  if (!(F & FZ)) { CYCT(b_+325, b_+327); goto L_4e5d; } // jr nz
  CYC(b_+325, b_+327);
  CALL_C(b_+327, getRandomRingOfGivenTier_hook, SYM(getRandomRingOfGivenTier), b_+330);
L_4e5d:
  CYC(b_+330, b_+332); alu_cp(gb, 0x2f); // TREASURE_POTION
  if (!(F & FZ)) { CYCT(b_+332, b_+334); goto L_4e68; } // jr nz
  CYC(b_+332, b_+334);
  CYC(b_+334, b_+336); A = 0x5e; // SND_GETSEED
  CALL_C(b_+336, playSound_b00_hook, SYM(playSound_b00), b_+339);
  CYC(b_+339, b_+341); A = 0x2f; // TREASURE_POTION
L_4e68:
  CALL_C(b_+341, giveTreasure_hook, SYM(giveTreasure), b_+344);
  CYC(b_+344, b_+347); partDelete_hook(gb); return; // jp

func_4e6e:
  CYC(b_+347, b_+350); SET_BC(0x2b02); // TREASURE_HEART_PIECE, $02
  CALL_C(b_+350, createTreasure_hook, SYM(createTreasure), b_+353);
  if (!(F & FZ)) { RET_TAKEN(b_+353); return; } // ret nz
  CYC(b_+353, b_+354);
  CYC(b_+354, b_+356); L = 0x4b; // Interaction.yh
  CYC(b_+356, b_+359); A = mem_rd(gb, w1Link_yh); // w1Link.yh
  CYC(b_+359, b_+360); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+360, b_+361); L = alu_inc8(gb, L);
  CYC(b_+361, b_+364); A = mem_rd(gb, w1Link_xh); // w1Link.xh
  CYC(b_+364, b_+365); mem_wr(gb, HL, A);
  CYC(b_+365, b_+368); SET_HL(wMapleState); // wMapleState
  CYC(b_+368, b_+370); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+370, b_+373); partDelete_hook(gb); return; // jp
}
