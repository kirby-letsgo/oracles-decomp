#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(partCode0f), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(partCode0f), (from), (to), true)

static uint16_t respawnableBush_jump_table(GB *gb) {
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

void partCode0f_hook(GB *gb);
void respawnableBush_setTileHere_hook(GB *gb);

void partCode0f_hook(GB *gb) {
  BASE(partCode0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);

  CYC(b_+2, b_+3); H = D;
  CYC(b_+3, b_+5); L = 0xc4; // Part.state
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 2
  CYC(b_+6, b_+8); L = 0xc6; // Part.counter1
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0xf0);
  CYC(b_+10, b_+12); L = 0xe4; // Part.collisionType
  CYC(b_+12, b_+14); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+14, b_+16); A = 0x02; // TILEINDEX_RESPAWNING_BUSH_CUT
  CYC(b_+16, b_+19); push_effect(gb, b_+19); respawnableBush_setTileHere_hook(gb);
  CALL_C(b_+19, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+22);
  CYC(b_+22, b_+23); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+23, b_+25); goto doneItemDropSpawn; } // jr nc
  CYC(b_+23, b_+25);
  CALL_C(b_+25, getFreePartSlot_hook, SYM(getFreePartSlot), b_+28);
  if (!(F & FZ)) { CYCT(b_+28, b_+30); goto doneItemDropSpawn; } // jr nz
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x01); // PART_ITEM_DROP
  CYC(b_+32, b_+33); L = alu_inc8(gb, L);
  CYC(b_+33, b_+34); E = L;
  CYC(b_+34, b_+35); A = mem_rd(gb, DE);
  CYC(b_+35, b_+36); mem_wr(gb, HL, A); // [itemDrop.subid] = [this.subid]
  CALL_C(b_+36, objectCopyPosition_hook, SYM(objectCopyPosition), b_+39);

doneItemDropSpawn:
  CYC(b_+39, b_+41); B = 0x00; // INTERAC_GRASSDEBRIS
  CALL_C(b_+41, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), b_+44);

normalStatus:
  CYC(b_+44, b_+46); E = 0xc4; // Part.state
  CYC(b_+46, b_+47); A = mem_rd(gb, DE);
  {
    CYC(b_+47, b_+48); push_effect(gb, b_+48);
    uint16_t target = respawnableBush_jump_table(gb);
    if (target == b_+58) goto state0;
    if (target == b_+62) goto state1;
    if (target == b_+63) goto state2;
    if (target == b_+80) goto state3;
    goto state4;
  }

state0:
  CYC(b_+58, b_+60); A = 0x01;
  CYC(b_+60, b_+61); mem_wr(gb, DE, A);
  RET(b_+61); return; // ret

state1:
  RET(b_+62); return; // ret

state2:
  CYC(b_+63, b_+66); A = mem_rd(gb, wFrameCounter); // wFrameCounter
  CYC(b_+66, b_+67); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+67); return; } // ret nc
  CYC(b_+67, b_+68);
  CALL_C(b_+68, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+71);
  if (!(F & FZ)) { RET_TAKEN(b_+71); return; } // ret nz
  CYC(b_+71, b_+72);
  CYC(b_+72, b_+74); mem_wr(gb, HL, 0x0c); // [counter1]
  CYC(b_+74, b_+75); L = E;
  CYC(b_+75, b_+76); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 3
  CYC(b_+76, b_+78); A = 0x03; // TILEINDEX_RESPAWNING_BUSH_REGEN
  CYC(b_+78, b_+80); respawnableBush_setTileHere_hook(gb); return; // jr

state3:
  CALL_C(b_+80, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+83);
  if (!(F & FZ)) { RET_TAKEN(b_+83); return; } // ret nz
  CYC(b_+83, b_+84);
  CYC(b_+84, b_+86); mem_wr(gb, HL, 0x08); // [counter1]
  CYC(b_+86, b_+87); L = E;
  CYC(b_+87, b_+88); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 4
  CYC(b_+88, b_+90); A = 0x04; // TILEINDEX_RESPAWNING_BUSH_READY
  respawnableBush_setTileHere_hook(gb); return; // falls through

state4:
  CALL_C(b_+99, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+102);
  if (!(F & FZ)) { RET_TAKEN(b_+102); return; } // ret nz
  CYC(b_+102, b_+103);
  CYC(b_+103, b_+104); L = E;
  CYC(b_+104, b_+106); mem_wr(gb, HL, 0x01); // [state] = 1
  CYC(b_+106, b_+108); L = 0xe4; // Part.collisionType
  CYC(b_+108, b_+110); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  RET(b_+110); return; // ret
}

void respawnableBush_setTileHere_hook(GB *gb) {
  BASE(partCode0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+90, b_+91); push_effect(gb, (uint16_t)((A << 8) | F));
  CALL_C(b_+91, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+94);
  CYC(b_+94, b_+95); C = A;
  {
    uint16_t af = pop_effect(gb);
    A = (uint8_t)(af >> 8); F = (uint8_t)(af & 0xff);
  }
  CYC(b_+95, b_+96);
  CYC(b_+96, b_+99); setTile_hook(gb); return; // jp
}
