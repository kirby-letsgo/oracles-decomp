#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// Script pointer tables (bank $0c mainScripts.*), indexed by subid or game progress.
#define interactionCode3a_scriptTable_bank08 SYM(interactionCode3a__scriptTable)
#define interactionCode3a_subid03ScriptTable_bank08 SYM(interactionCode3a__subid03ScriptTable)
#define interactionCode3a_subid4And5ScriptTable_bank08 SYM(interactionCode3a__subid4And5ScriptTable)
#define interactionCode3a_subid6And7ScriptTable_bank08 SYM(interactionCode3a__subid6And7ScriptTable)
#define interactionCode3a_subid08ScriptTable_bank08 SYM(interactionCode3a__subid08ScriptTable)

// Cross-bank targets referenced by address only.
#define getGameProgress_1_bank09 SYM(getGameProgress_1)
#define getGameProgress_2_bank09 SYM(getGameProgress_2)
#define interactionOscillateXRandomly_bank08 SYM(interactionOscillateXRandomly)
#define setCounter1To120AndPlaySoundEffectAndIncSubstate_bank08 SYM(setCounter1To120AndPlaySoundEffectAndIncSubstate)
#define villagerSubid02Script_part2_bank0c SYM(villagerSubid02Script_part2)

static uint16_t villager_jumpTable(GB *gb) {
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

static void villager_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// @loadScript: pick the script for this subid from @scriptTable. Reached by static jumps
// and one static `call` (@initSubid0d) from interactionCode3a itself.
static void villager_loadScript(GB *gb) {
  BASE(interactionCode3a);
  CYC(b_+646, b_+648); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+648, b_+649); A = mem_rd(gb, DE);
  CYC(b_+649, b_+652); SET_HL(interactionCode3a_scriptTable_bank08);
  CYC(b_+652, b_+653); villager_addDoubleIndex(gb, b_+653);
  CYC(b_+653, b_+654); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+654, b_+655); H = mem_rd(gb, HL);
  CYC(b_+655, b_+656); L = A;
  CYC(b_+656, b_+659); interactionSetScript_hook(gb);
}

// @saveXAndLoadScript: remember xh in var3d, then fall into @initSubid01 (jp @loadScript).
// Reached by fallthrough from @initSubid02 and a static `call` from @runSubid02@substate1.
static void villager_saveXAndLoadScript(GB *gb) {
  BASE(interactionCode3a);
  CYC(b_+77, b_+79); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+79, b_+80); A = mem_rd(gb, DE);
  CYC(b_+80, b_+82); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+82, b_+83); mem_wr(gb, DE, A);
  // @initSubid01
  CYC(b_+83, b_+86); villager_loadScript(gb);
}

// @loadStoneAnimation: stone palette, radius and animation. H must already be D.
// Reached by `jr` (@initSubid0a), fallthrough (@initSubid0c) and `call` (@initSubid0b).
static void villager_loadStoneAnimation(GB *gb, uint16_t sp0_) {
  BASE(interactionCode3a);
  CYC(b_+228, b_+230); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+230, b_+232); mem_wr(gb, HL, 0x06);
  CYC(b_+232, b_+234); A = 0x06;
  CALL_C(b_+234, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+237);
  CYC(b_+237, b_+239); A = 0x0d;
  CYC(b_+239, b_+242); interactionSetAnimation_hook(gb);
}

// @state1: per-subid update. Also reached by fallthrough/jr from @initSubid0b/0d/0e.
static void villager_state1(GB *gb, uint16_t sp0_) {
  BASE(interactionCode3a);
  CYC(b_+271, b_+273); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+273, b_+274); A = mem_rd(gb, DE);
  CYC(b_+274, b_+275); push_effect(gb, b_+275);
  do { uint16_t jt_ = (villager_jumpTable(gb));
    if (jt_ == b_+305) { goto runSubid00; }
    else if (jt_ == b_+383) { goto runSubid01; }
    else if (jt_ == b_+389) { goto runSubid02; }
    else if (jt_ == b_+486) { goto runScriptAndFaceLink; }
    else if (jt_ == b_+492) { goto runSubid09; }
    else if (jt_ == b_+527) { goto plainRet; }
    else if (jt_ == b_+528) { goto runSubid0b; }
    else if (jt_ == b_+600) { goto runSubid0c; }
    else if (jt_ == b_+633) { goto runSubid0d; }
    else if (jt_ == b_+645) { goto runSubid0e; }
    else { HANDOFF(HL); }
  } while (0);

runSubid00:
  // Cutscene where guy is struck by lightning in intro
  CYC(b_+305, b_+307); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+307, b_+308); A = mem_rd(gb, DE);
  CYC(b_+308, b_+309); push_effect(gb, b_+309);
  do { uint16_t jt_ = (villager_jumpTable(gb));
    if (jt_ == b_+315) { goto subid00_substate0; }
    else if (jt_ == b_+337) { goto subid00_substate1; }
    else if (jt_ == b_+371) { goto subid00_substate2; }
    else { HANDOFF(HL); }
  } while (0);

subid00_substate0:
  CYC(b_+315, b_+318); A = mem_rd(gb, wTmpcfc0 + 0x11);
  CYC(b_+318, b_+320); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(b_+320, b_+323); interactionAnimate_hook(gb); return;
  }
  CYC(b_+320, b_+323);
  CALL_C(b_+323, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+326);
  CYC(b_+326, b_+328); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+328, b_+330); mem_wr(gb, HL, 0x3c);
  CYC(b_+330, b_+332); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+332, b_+333); A = mem_rd(gb, HL);
  CYC(b_+333, b_+335); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+335, b_+336); mem_wr(gb, HL, A);
  CYC(b_+336, b_+337); ret_effect(gb);
  return;

subid00_substate1:
  // callab interactionOscillateXRandomly
  CYC(b_+337, b_+340); SET_HL(interactionOscillateXRandomly_bank08);
  CYC(b_+340, b_+342); E = 0x08;
  CALL_C(b_+342, interBankCall_hook, 0x008a, b_+345);
  CYC(b_+345, b_+348); A = mem_rd(gb, wTmpcfc0 + 0x11);
  CYC(b_+348, b_+350); alu_cp(gb, 0x03);
  if (!(F & FZ)) {
    CYCT(b_+350, b_+351); ret_effect(gb); return;
  }
  CYC(b_+350, b_+351);
  CALL_C(b_+351, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+354);
  CYC(b_+354, b_+356); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+356, b_+358); mem_wr(gb, HL, 0x10);
  CALL_C(b_+358, getFreePartSlot_hook, SYM(getFreePartSlot), b_+361);
  if (!(F & FZ)) {
    CYCT(b_+361, b_+362); ret_effect(gb); return;
  }
  CYC(b_+361, b_+362);
  CYC(b_+362, b_+364); mem_wr(gb, HL, 0x27); // PART_LIGHTNING
  CYC(b_+364, b_+365); L = alu_inc8(gb, L);
  CYC(b_+365, b_+366); mem_wr(gb, HL, E);
  CYC(b_+366, b_+367); L = alu_inc8(gb, L);
  CYC(b_+367, b_+368); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+368, b_+371); objectCopyPosition_hook(gb);
  return;

subid00_substate2:
  CALL_C(b_+371, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+374);
  if (!(F & FZ)) {
    CYCT(b_+374, b_+375); ret_effect(gb); return;
  }
  CYC(b_+374, b_+375);
  CYC(b_+375, b_+377); A = 0x04;
  CYC(b_+377, b_+380); mem_wr(gb, wTmpcfc0 + 0x11, A);
  CYC(b_+380, b_+383); interactionDelete_hook(gb);
  return;

runSubid01:
  // Past villager?
  CALL_C(b_+383, interactionRunScript_hook, SYM(interactionRunScript), b_+386);
  CYC(b_+386, b_+389); interactionAnimateAsNpc_hook(gb);
  return;

runSubid02:
  // Construction worker blocking path to upper part of black tower.
  CYC(b_+389, b_+391); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+391, b_+392); A = mem_rd(gb, DE);
  CYC(b_+392, b_+393); push_effect(gb, b_+393);
  do { uint16_t jt_ = (villager_jumpTable(gb));
    if (jt_ == b_+397) { goto subid02_substate0; }
    else if (jt_ == b_+460) { goto subid02_substate1; }
    else { HANDOFF(HL); }
  } while (0);

subid02_substate0:
  CALL_C(b_+397, npcFaceLinkAndAnimate_hook, SYM(npcFaceLinkAndAnimate), b_+400);
  CALL_C(b_+400, interactionRunScript_hook, SYM(interactionRunScript), b_+403);
  CYC(b_+403, b_+406); SET_BC(0x0503);
  CALL_C(b_+406, objectSetCollideRadii_hook, SYM(objectSetCollideRadii), b_+409);
  // Temporarily overwrite the worker's X position to check for "collision" at the
  // position he's left open. His position will be reverted before returning.
  CYC(b_+409, b_+411); B = 0x11;
  CYC(b_+411, b_+413); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+413, b_+414); A = mem_rd(gb, DE);
  CYC(b_+414, b_+415); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+415, b_+417);
  } else {
    CYC(b_+415, b_+417);
    CYC(b_+417, b_+419); B = 0xef;
  }
  CYC(b_+419, b_+421); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+421, b_+422); A = mem_rd(gb, DE);
  CYC(b_+422, b_+423); alu_add(gb, B);
  CYC(b_+423, b_+424); mem_wr(gb, DE, A);
  CYC(b_+424, b_+425); push_effect(gb, BC);
  CALL_C(b_+425, objectCheckCollidedWithLink_ignoreZ_hook, SYM(objectCheckCollidedWithLink_ignoreZ), b_+428);
  CYC(b_+428, b_+429); SET_BC(pop_effect(gb));
  if (!(F & FC)) {
    CYCT(b_+429, b_+431); goto subid02_restorePosition;
  }
  CYC(b_+429, b_+431);
  // Link tried to approach; move over to block his path
  CALL_C(b_+431, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+434);
  CYC(b_+434, b_+437); SET_HL(villagerSubid02Script_part2_bank0c);
  CALL_C(b_+437, interactionSetScript_hook, SYM(interactionSetScript), b_+440);
subid02_restorePosition:
  CYC(b_+440, b_+443); SET_HL(w1Link_yh);
  CYC(b_+443, b_+445); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+445, b_+446); A = mem_rd(gb, HL);
  CYC(b_+446, b_+447); mem_wr(gb, DE, A);
  CYC(b_+447, b_+450); SET_BC(0x0606);
  CALL_C(b_+450, objectSetCollideRadii_hook, SYM(objectSetCollideRadii), b_+453);
  CYC(b_+453, b_+455); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+455, b_+456); A = mem_rd(gb, DE);
  CYC(b_+456, b_+458); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+458, b_+459); mem_wr(gb, DE, A);
  CYC(b_+459, b_+460); ret_effect(gb);
  return;

subid02_substate1:
  CALL_C(b_+460, interactionAnimateAsNpc_hook, SYM(interactionAnimateAsNpc), b_+463);
  CALL_C(b_+463, interactionRunScript_hook, SYM(interactionRunScript), b_+466);
  if (!(F & FC)) {
    CYCT(b_+466, b_+469); interactionAnimateBasedOnSpeed_hook(gb); return;
  }
  CYC(b_+466, b_+469);
  CYC(b_+469, b_+472); push_effect(gb, b_+472); villager_saveXAndLoadScript(gb);
  CYC(b_+472, b_+473); H = D;
  CYC(b_+473, b_+475); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+475, b_+476); A = mem_rd(gb, HL);
  CYC(b_+476, b_+478); alu_xor(gb, 0x01);
  CYC(b_+478, b_+479); mem_wr(gb, HL, A);
  CYC(b_+479, b_+481); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+481, b_+483); mem_wr(gb, HL, 0x00);
  CYC(b_+483, b_+486); villager_loadScript(gb);
  return;

runScriptAndFaceLink:
  CALL_C(b_+486, interactionRunScript_hook, SYM(interactionRunScript), b_+489);
  CYC(b_+489, b_+492); npcFaceLinkAndAnimate_hook(gb);
  return;

runSubid09:
  CYC(b_+492, b_+494); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+494, b_+495); A = mem_rd(gb, DE);
  CYC(b_+495, b_+496); push_effect(gb, b_+496);
  do { uint16_t jt_ = (villager_jumpTable(gb));
    if (jt_ == b_+502) { goto subid09_substate0; }
    else if (jt_ == b_+514) { goto subid09_substate1; }
    else if (jt_ == b_+527) { goto plainRet; }
    else { HANDOFF(HL); }
  } while (0);

subid09_substate0:
  CALL_C(b_+502, interactionRunScript_hook, SYM(interactionRunScript), b_+505);
  CYC(b_+505, b_+508); A = mem_rd(gb, wTmpcfc0 + 0x11);
  CYC(b_+508, b_+510); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(b_+510, b_+511); ret_effect(gb); return;
  }
  CYC(b_+510, b_+511);
  CYC(b_+511, b_+514); interactionIncSubstate_hook(gb);
  return;

subid09_substate1:
  CYC(b_+514, b_+517); A = mem_rd(gb, wTmpcfc0 + 0x11);
  CYC(b_+517, b_+519); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(b_+519, b_+520); ret_effect(gb); return;
  }
  CYC(b_+519, b_+520);
  CALL_C(b_+520, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+523);
  CYC(b_+523, b_+525); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+525, b_+527); mem_wr(gb, HL, 0x06);

plainRet:
  // @ret
  CYC(b_+527, b_+528); ret_effect(gb);
  return;

runSubid0b:
  // Villager being restored from stone, resumes playing catch
  CYC(b_+528, b_+530); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+530, b_+531); A = mem_rd(gb, DE);
  CYC(b_+531, b_+532); push_effect(gb, b_+532);
  do { uint16_t jt_ = (villager_jumpTable(gb));
    if (jt_ == b_+538) { goto subid0b_substate0; }
    else if (jt_ == b_+560) { goto subid0b_substate1; }
    else if (jt_ == b_+590) { goto subid0b_substate2; }
    else { HANDOFF(HL); }
  } while (0);

subid0b_substate0:
  CALL_C(b_+538, interactionDecCounter1IfPaletteNotFading_hook, SYM(interactionDecCounter1IfPaletteNotFading), b_+541);
  if (!(F & FZ)) {
    CYCT(b_+541, b_+542); ret_effect(gb); return;
  }
  CYC(b_+541, b_+542);
  CYC(b_+542, b_+544); A = 0x01;
  CYC(b_+544, b_+547); mem_wr(gb, wTmpcfc0 + 0x11, A);
  CYC(b_+547, b_+549); A = 0xc1; // SND_RESTORE
  CALL_C(b_+549, playSound_b00_hook, SYM(playSound_b00), b_+552);
  // jpab setCounter1To120AndPlaySoundEffectAndIncSubstate
  CYC(b_+552, b_+555); SET_HL(setCounter1To120AndPlaySoundEffectAndIncSubstate_bank08);
  CYC(b_+555, b_+557); E = 0x08;
  CYC(b_+557, b_+560); interBankCall_hook(gb);
  return;

subid0b_substate1:
  CALL_C(b_+560, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+563);
  if (!(F & FZ)) {
    CYCT(b_+563, b_+565); goto subid0b_flickerPalette;
  }
  CYC(b_+563, b_+565);
  CALL_C(b_+565, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+568);
  CYC(b_+568, b_+570); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+570, b_+572); mem_wr(gb, HL, 0x01);
  CYC(b_+572, b_+575); villager_loadScript(gb);
  return;
subid0b_flickerPalette:
  // Flicker palette every 8 frames
  CYC(b_+575, b_+578); A = mem_rd(gb, wFrameCounter);
  CYC(b_+578, b_+580); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(b_+580, b_+581); ret_effect(gb); return;
  }
  CYC(b_+580, b_+581);
  CYC(b_+581, b_+583); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+583, b_+584); A = mem_rd(gb, DE);
  CYC(b_+584, b_+585); A = alu_dec8(gb, A);
  CYC(b_+585, b_+587); alu_xor(gb, 0x05);
  CYC(b_+587, b_+588); A = alu_inc8(gb, A);
  CYC(b_+588, b_+589); mem_wr(gb, DE, A);
  CYC(b_+589, b_+590); ret_effect(gb);
  return;

subid0b_substate2:
  CYC(b_+590, b_+592); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+592, b_+593); A = mem_rd(gb, DE);
  CYC(b_+593, b_+594); alu_or(gb, A);
  if (F & FZ) {
    CALL_C_CC(b_+594, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+597);
  } else {
    CYC(b_+594, b_+597);
  }
  CYC(b_+597, b_+600); interactionRunScript_hook(gb);
  return;

runSubid0c:
  // Villager playing catch with son
  CALL_C(b_+600, interactionPushLinkAwayAndUpdateDrawPriority_hook, SYM(interactionPushLinkAwayAndUpdateDrawPriority), b_+603);
  CYC(b_+603, b_+605); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+605, b_+606); A = mem_rd(gb, DE);
  CYC(b_+606, b_+607); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+607, b_+608); ret_effect(gb); return;
  }
  CYC(b_+607, b_+608);
  CALL_C(b_+608, interactionRunScript_hook, SYM(interactionRunScript), b_+611);
  // If you press the A button, show text
  CYC(b_+611, b_+613); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+613, b_+614); A = mem_rd(gb, DE);
  CYC(b_+614, b_+615); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+615, b_+616); ret_effect(gb); return;
  }
  CYC(b_+615, b_+616);
  CYC(b_+616, b_+617); alu_xor(gb, A);
  CYC(b_+617, b_+618); mem_wr(gb, DE, A);
  CYC(b_+618, b_+621); SET_BC(0x1442); // TX_1442
  CYC(b_+621, b_+624); SET_HL(wGroup4RoomFlags + 0xfc);
  CYC(b_+624, b_+626); alu_bit(gb, 7, mem_rd(gb, HL)); // Has Veran been beaten?
  if (F & FZ) {
    CYCT(b_+626, b_+628);
  } else {
    CYC(b_+626, b_+628);
    CYC(b_+628, b_+630); C = 0x43; // <TX_1443
  }
  CYC(b_+630, b_+633); showText_hook(gb);
  return;

runSubid0d:
  // Cutscene when you first enter the past
  CALL_C(b_+633, interactionRunScript_hook, SYM(interactionRunScript), b_+636);
  if (F & FC) {
    CYCT(b_+636, b_+639); interactionDelete_hook(gb); return;
  }
  CYC(b_+636, b_+639);
  CALL_C(b_+639, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+642);
  CYC(b_+642, b_+645); interactionPushLinkAwayAndUpdateDrawPriority_hook(gb);
  return;

runSubid0e:
  // Stone villager? Not much to do.
  CYC(b_+645, b_+646); ret_effect(gb);
}

// @initSubid: per-subid initialization. Reached only by a static `call` from
// interactionCode3a@state0 (NOT HOOKED).
static void villager_initSubid(GB *gb, uint16_t sp0_) {
  BASE(interactionCode3a);
  CYC(b_+28, b_+30); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+32); push_effect(gb, b_+32);
  do { uint16_t jt_ = (villager_jumpTable(gb));
    if (jt_ == b_+62) { goto initSubid00; }
    else if (jt_ == b_+83) { goto initSubid01; }
    else if (jt_ == b_+67) { goto initSubid02; }
    else if (jt_ == b_+86) { goto initSubid03; }
    else if (jt_ == b_+105) { goto initSubid04And05; }
    else if (jt_ == b_+139) { goto initSubid06And07; }
    else if (jt_ == b_+168) { goto initSubid08; }
    else if (jt_ == b_+202) { goto initAnimationAndLoadScript; }
    else if (jt_ == b_+199) { goto initSubid0a; }
    else if (jt_ == b_+242) { goto initSubid0b; }
    else if (jt_ == b_+210) { goto initSubid0c; }
    else if (jt_ == b_+253) { goto initSubid0d; }
    else if (jt_ == b_+258) { goto initSubid0e; }
    else { HANDOFF(HL); }
  } while (0);

initSubid00:
  CYC(b_+62, b_+64); A = 0x03;
  CYC(b_+64, b_+67); interactionSetAnimation_hook(gb);
  return;

initSubid02:
  CYC(b_+67, b_+69); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CALL_C(b_+69, objectAddToAButtonSensitiveObjectList_hook, SYM(objectAddToAButtonSensitiveObjectList), b_+72);
  CYC(b_+72, b_+74); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+74, b_+76); A = 0x28; // SPEED_100
  CYC(b_+76, b_+77); mem_wr(gb, DE, A);
  villager_saveXAndLoadScript(gb);
  return;

initSubid01:
  CYC(b_+83, b_+86); villager_loadScript(gb);
  return;

initSubid03:
  // callab agesInteractionsBank09.getGameProgress_1
  CYC(b_+86, b_+89); SET_HL(getGameProgress_1_bank09);
  CYC(b_+89, b_+91); E = 0x09;
  CALL_C(b_+91, interBankCall_hook, 0x008a, b_+94);
  CYC(b_+94, b_+95); A = B;
  CYC(b_+95, b_+98); SET_HL(interactionCode3a_subid03ScriptTable_bank08);
  CYC(b_+98, b_+99); villager_addDoubleIndex(gb, b_+99);
  CYC(b_+99, b_+100); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+100, b_+101); H = mem_rd(gb, HL);
  CYC(b_+101, b_+102); L = A;
  CYC(b_+102, b_+105); interactionSetScript_hook(gb);
  return;

initSubid04And05:
  CYC(b_+105, b_+107); A = 0x02;
  CYC(b_+107, b_+109); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+109, b_+110); mem_wr(gb, DE, A);
  // callab agesInteractionsBank09.getGameProgress_1
  CYC(b_+110, b_+113); SET_HL(getGameProgress_1_bank09);
  CYC(b_+113, b_+115); E = 0x09;
  CALL_C(b_+115, interBankCall_hook, 0x008a, b_+118);
  CYC(b_+118, b_+120); C = 0x04;
  CYC(b_+120, b_+122); A = 0x03;
  CALL_C(b_+122, checkNpcShouldExistAtGameStage_hook, SYM(checkNpcShouldExistAtGameStage), b_+125);
  if (!(F & FZ)) {
    CYCT(b_+125, b_+128); interactionDelete_hook(gb); return;
  }
  CYC(b_+125, b_+128);
  CYC(b_+128, b_+129); A = B;
  CYC(b_+129, b_+132); SET_HL(interactionCode3a_subid4And5ScriptTable_bank08);
  CYC(b_+132, b_+133); villager_addDoubleIndex(gb, b_+133);
  CYC(b_+133, b_+134); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+134, b_+135); H = mem_rd(gb, HL);
  CYC(b_+135, b_+136); L = A;
  CYC(b_+136, b_+139); interactionSetScript_hook(gb);
  return;

initSubid06And07:
  // callab agesInteractionsBank09.getGameProgress_2
  CYC(b_+139, b_+142); SET_HL(getGameProgress_2_bank09);
  CYC(b_+142, b_+144); E = 0x09;
  CALL_C(b_+144, interBankCall_hook, 0x008a, b_+147);
  CYC(b_+147, b_+149); C = 0x06;
  CYC(b_+149, b_+151); A = 0x04;
  CALL_C(b_+151, checkNpcShouldExistAtGameStage_hook, SYM(checkNpcShouldExistAtGameStage), b_+154);
  if (!(F & FZ)) {
    CYCT(b_+154, b_+157); interactionDelete_hook(gb); return;
  }
  CYC(b_+154, b_+157);
  CYC(b_+157, b_+158); A = B;
  CYC(b_+158, b_+161); SET_HL(interactionCode3a_subid6And7ScriptTable_bank08);
  CYC(b_+161, b_+162); villager_addDoubleIndex(gb, b_+162);
  CYC(b_+162, b_+163); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+163, b_+164); H = mem_rd(gb, HL);
  CYC(b_+164, b_+165); L = A;
  CYC(b_+165, b_+168); interactionSetScript_hook(gb);
  return;

initSubid08:
  CYC(b_+168, b_+170); A = 0x03;
  CYC(b_+170, b_+172); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+172, b_+173); mem_wr(gb, DE, A);
  // Delete if you haven't beaten d7 yet? (callab agesInteractionsBank09.getGameProgress_2)
  CYC(b_+173, b_+176); SET_HL(getGameProgress_2_bank09);
  CYC(b_+176, b_+178); E = 0x09;
  CALL_C(b_+178, interBankCall_hook, 0x008a, b_+181);
  CYC(b_+181, b_+182); A = B;
  CYC(b_+182, b_+184); alu_cp(gb, 0x04);
  if (F & FC) {
    CYCT(b_+184, b_+187); interactionDelete_hook(gb); return;
  }
  CYC(b_+184, b_+187);
  CYC(b_+187, b_+189); alu_sub(gb, 0x04);
  CYC(b_+189, b_+192); SET_HL(interactionCode3a_subid08ScriptTable_bank08);
  CYC(b_+192, b_+193); villager_addDoubleIndex(gb, b_+193);
  CYC(b_+193, b_+194); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+194, b_+195); H = mem_rd(gb, HL);
  CYC(b_+195, b_+196); L = A;
  CYC(b_+196, b_+199); interactionSetScript_hook(gb);
  return;

initSubid0a:
  CYC(b_+199, b_+200); H = D;
  CYC(b_+200, b_+202); villager_loadStoneAnimation(gb, sp0_);
  return;

initAnimationAndLoadScript:
  CYC(b_+202, b_+204); A = 0x01;
  CALL_C(b_+204, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+207);
  CYC(b_+207, b_+210); villager_loadScript(gb);
  return;

initSubid0c:
  // Check whether the villager should be stone right now: have we beaten Veran?
  CYC(b_+210, b_+213); SET_HL(wGroup4RoomFlags + 0xfc);
  CYC(b_+213, b_+215); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+215, b_+217); goto initAnimationAndLoadScript;
  }
  CYC(b_+215, b_+217);
  CYC(b_+217, b_+220); A = mem_rd(gb, wEssencesObtained);
  CYC(b_+220, b_+222); alu_bit(gb, 6, A);
  if (F & FZ) {
    CYCT(b_+222, b_+224); goto initAnimationAndLoadScript;
  }
  CYC(b_+222, b_+224);
  CYC(b_+224, b_+225); H = D;
  CYC(b_+225, b_+227); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+227, b_+228); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  villager_loadStoneAnimation(gb, sp0_);
  return;

initSubid0b:
  CYC(b_+242, b_+243); H = D;
  CYC(b_+243, b_+246); push_effect(gb, b_+246); villager_loadStoneAnimation(gb, sp0_);
  CYC(b_+246, b_+248); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+248, b_+250); A = 0x3c;
  CYC(b_+250, b_+251); mem_wr(gb, DE, A);
  CYC(b_+251, b_+253); villager_state1(gb, sp0_);
  return;

initSubid0d:
  CYC(b_+253, b_+256); push_effect(gb, b_+256); villager_loadScript(gb);
  CYC(b_+256, b_+258); villager_state1(gb, sp0_);
  return;

initSubid0e:
  CALL_C(b_+258, loadStoneNpcPalette_hook, SYM(loadStoneNpcPalette), b_+261);
  CYC(b_+261, b_+262); H = D;
  CYC(b_+262, b_+264); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+264, b_+266); mem_wr(gb, HL, 0x06);
  CYC(b_+266, b_+268); A = 0x0d;
  CALL_C(b_+268, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+271);
  villager_state1(gb, sp0_);
}

// INTERAC_MALE_VILLAGER: assorted male NPCs, subid-dispatched.
//   var03: nonzero if he's turned to stone
//   var39: for some subids, animations only update when var39 is zero
//   var3d: saved X position
void interactionCode3a_hook(GB *gb) {
  BASE(interactionCode3a);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (villager_jumpTable(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+271) { villager_state1(gb, sp0_); return; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  CALL_C(b_+14, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+17);
  CYC(b_+17, b_+20); push_effect(gb, b_+20); villager_initSubid(gb, sp0_);
  CYC(b_+20, b_+22); E = INTERACTION_BASE + OBJ_ENABLED;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+24, b_+27); objectMarkSolidPosition_hook(gb); return;
  }
  CYC(b_+24, b_+27);
  CYC(b_+27, b_+28); ret_effect(gb);
}
