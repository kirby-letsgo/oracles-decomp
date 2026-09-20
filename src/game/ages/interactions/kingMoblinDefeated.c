#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t interactionCode72_jump_table(GB *gb) {
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

static void interactionCode72_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_KING_MOBLIN_DEFEATED
void interactionCode72_hook(GB *gb) {
  BASE(interactionCode72);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_STATE;
  {
    CYC(b_+5, b_+6); push_effect(gb, b_+6);
    uint16_t target = interactionCode72_jump_table(gb);
    if (target == b_+126) goto subid1;
    if (target == b_+144) goto subid2;
  }

  // Subid 0: King moblin / "parent" for other subids
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+14, b_+16); goto subid0State0; } // jr z
  CYC(b_+14, b_+16);

  // interactionCode72@subid0State1
  CALL_C(b_+16, interactionRunScript_hook, SYM(interactionRunScript), b_+19);
  if (!(F & FC)) { CYCT(b_+19, b_+22); interactionAnimate_hook(gb); return; } // jp nc
  CYC(b_+19, b_+22);
  CALL_C(b_+22, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+25);
  if (!(F & FZ)) { RET_TAKEN(b_+25); return; } // ret nz
  CYC(b_+25, b_+26);
  // Spawn instance of this object with subid 2
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x72); // INTERAC_KING_MOBLIN_DEFEATED
  CYC(b_+28, b_+29); L = alu_inc8(gb, L);
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x02);
  CYC(b_+31, b_+33); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x68);
  CYC(b_+35, b_+38); interactionDelete_hook(gb); return; // jp

subid0State0:
  CALL_C(b_+38, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+41);
  CYC(b_+41, b_+43); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(b_+43, b_+46); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+43, b_+46);
  CYC(b_+46, b_+48); A = 0x1a; // GLOBALFLAG_MOBLINS_KEEP_DESTROYED
  CALL_C(b_+48, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+51);
  if (F & FZ) { CYCT(b_+51, b_+54); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+51, b_+54);
  CALL_C(b_+54, setDeathRespawnPoint_hook, SYM(setDeathRespawnPoint), b_+57);
  CYC(b_+57, b_+59); A = 0x80;
  CYC(b_+59, b_+62); W8(wDisabledObjects) = A;
  CYC(b_+62, b_+65); W8(wMenuDisabled) = A;

  CYC(b_+65, b_+68); push_effect(gb, b_+68); goto spawnSubservientMoblin;
after1_spawnSubservientMoblin:
  CYC(b_+68, b_+70); mem_wr(gb, HL, 0x38);
  CYC(b_+70, b_+73); push_effect(gb, b_+73); goto spawnSubservientMoblin;
after2_spawnSubservientMoblin:
  CYC(b_+73, b_+75); mem_wr(gb, HL, 0x78);
  CYC(b_+75, b_+78); SET_HL(wTmpcfc0_armosStatue_killedArmosPositions);
  CYC(b_+78, b_+80); B = 0x04;
  CALL_C(b_+80, clearMemory_hook, SYM(clearMemory), b_+83);
  CYC(b_+83, b_+85); A = 0x02;
  CALL_C(b_+85, fadeinFromWhiteWithDelay_hook, SYM(fadeinFromWhiteWithDelay), b_+88);
  CYC(b_+88, b_+91); SET_HL((SYM(fallingRock_initDiagonalAngle) + 1)); // mainScripts.kingMoblinDefeated_kingScript

setScriptAndInitStuff:
  CALL_C(b_+91, interactionSetScript_hook, SYM(interactionSetScript), b_+94);
  CALL_C(b_+94, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+97);
  CALL_C(b_+97, interactionIncState_hook, SYM(interactionIncState), b_+100);
  CYC(b_+100, b_+102); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+102, b_+104); mem_wr(gb, HL, 0x3c); // SPEED_180
  CYC(b_+104, b_+106); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+106, b_+108); mem_wr(gb, HL, 0x10); // ANGLE_DOWN
  CYC(b_+108, b_+111); objectSetVisible82_hook(gb); return; // jp

// Spawn an instance of subid 1, the normal moblins
spawnSubservientMoblin:
  CALL_C(b_+111, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+114);
  if (!(F & FZ)) {
    RET_TAKEN(b_+114);
    if (gb->pc == b_+68 && gb->sp == sp0_) goto after1_spawnSubservientMoblin;
    if (gb->pc == b_+73 && gb->sp == sp0_) goto after2_spawnSubservientMoblin;
    return;
  } // ret nz
  CYC(b_+114, b_+115);
  CYC(b_+115, b_+117); mem_wr(gb, HL, 0x72); // INTERAC_KING_MOBLIN_DEFEATED
  CYC(b_+117, b_+118); L = alu_inc8(gb, L);
  CYC(b_+118, b_+119); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+119, b_+121); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+121, b_+123); mem_wr(gb, HL, 0x68);
  CYC(b_+123, b_+125); L = INTERACTION_BASE + OBJ_XH;
  RET(b_+125);
  if (gb->pc == b_+68 && gb->sp == sp0_) goto after1_spawnSubservientMoblin;
  if (gb->pc == b_+73 && gb->sp == sp0_) goto after2_spawnSubservientMoblin;
  return; // ret

subid1:
  CYC(b_+126, b_+127); A = mem_rd(gb, DE);
  CYC(b_+127, b_+128); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+128, b_+130); goto subid1State0; } // jr z
  CYC(b_+128, b_+130);

runScriptAndAnimate:
  CALL_C(b_+130, interactionRunScript_hook, SYM(interactionRunScript), b_+133);
  if (!(F & FC)) { CYCT(b_+133, b_+136); interactionAnimate_hook(gb); return; } // jp nc
  CYC(b_+133, b_+136);
  CYC(b_+136, b_+139); interactionDelete_hook(gb); return; // jp

subid1State0:
  CYC(b_+139, b_+142); SET_HL(SYM(fallingRock_initDiagonalAngle__diagonalAngles)); // mainScripts.kingMoblinDefeated_helperMoblinScript
  CYC(b_+142, b_+144); goto setScriptAndInitStuff; // jr

// Subid 2: Gorons who approach after he leaves; var03 is the index
subid2:
  CYC(b_+144, b_+145); A = mem_rd(gb, DE);
  CYC(b_+145, b_+146); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+146, b_+148); goto runScriptAndAnimate; } // jr nz
  CYC(b_+146, b_+148);
  CALL_C(b_+148, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+151);
  CALL_C(b_+151, interactionIncState_hook, SYM(interactionIncState), b_+154);
  CYC(b_+154, b_+156); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+156, b_+158); mem_wr(gb, HL, 0x14); // SPEED_80
  // Load script
  CYC(b_+158, b_+160); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+160, b_+161); A = mem_rd(gb, DE);
  CYC(b_+161, b_+164); SET_HL(b_+225); // @scriptTable
  CYC(b_+164, b_+165); interactionCode72_add_double_index(gb, b_+165);
  CYC(b_+165, b_+166); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+166, b_+167); H = mem_rd(gb, HL);
  CYC(b_+167, b_+168); L = A;
  CALL_C(b_+168, interactionSetScript_hook, SYM(interactionSetScript), b_+171);
  CALL_C(b_+171, objectSetVisible82_hook, SYM(objectSetVisible82), b_+174);
  // Load data from table
  CYC(b_+174, b_+176); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+176, b_+177); A = mem_rd(gb, DE);
  CYC(b_+177, b_+178); alu_add(gb, A); // add a
  CYC(b_+178, b_+181); SET_HL(b_+233); // @goronData
  CYC(b_+181, b_+182); interactionCode72_add_double_index(gb, b_+182);
  CYC(b_+182, b_+184); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+184, b_+185); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+185, b_+186); mem_wr(gb, DE, A);
  CYC(b_+186, b_+188); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+188, b_+189); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+189, b_+190); mem_wr(gb, DE, A);
  CYC(b_+190, b_+192); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+192, b_+193); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+193, b_+194); mem_wr(gb, DE, A);
  CYC(b_+194, b_+195); A = mem_rd(gb, HL);
  CALL_C(b_+195, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+198);
  // If [var03] == 0, spawn the other gorons
  CYC(b_+198, b_+200); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+200, b_+201); A = mem_rd(gb, DE);
  CYC(b_+201, b_+202); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+202); return; } // ret nz
  CYC(b_+202, b_+203);
  CYC(b_+203, b_+205); B = 0x01;

  CYC(b_+205, b_+208); push_effect(gb, b_+208); goto spawnGoronInstance;
after1_spawnGoronInstance:
  CYC(b_+208, b_+209); B = alu_inc8(gb, B);
  CYC(b_+209, b_+212); push_effect(gb, b_+212); goto spawnGoronInstance;
after2_spawnGoronInstance:
  CYC(b_+212, b_+213); B = alu_inc8(gb, B);

spawnGoronInstance:
  CALL_C(b_+213, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+216);
  if (!(F & FZ)) {
    RET_TAKEN(b_+216);
    if (gb->pc == b_+208 && gb->sp == sp0_) goto after1_spawnGoronInstance;
    if (gb->pc == b_+212 && gb->sp == sp0_) goto after2_spawnGoronInstance;
    return;
  } // ret nz
  CYC(b_+216, b_+217);
  CYC(b_+217, b_+219); mem_wr(gb, HL, 0x72); // INTERAC_KING_MOBLIN_DEFEATED
  CYC(b_+219, b_+220); L = alu_inc8(gb, L);
  CYC(b_+220, b_+222); mem_wr(gb, HL, 0x02);
  CYC(b_+222, b_+223); L = alu_inc8(gb, L);
  CYC(b_+223, b_+224); mem_wr(gb, HL, B);
  RET(b_+224);
  if (gb->pc == b_+208 && gb->sp == sp0_) goto after1_spawnGoronInstance;
  if (gb->pc == b_+212 && gb->sp == sp0_) goto after2_spawnGoronInstance;
  return; // ret
}
