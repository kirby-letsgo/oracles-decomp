#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

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
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(0x4995, 0x4997); goto normalStatus; } // jr z
  CYC(0x4995, 0x4997);

  CYC(0x4997, 0x4998); H = D;
  CYC(0x4998, 0x499a); L = 0xc4; // Part.state
  CYC(0x499a, 0x499b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 2
  CYC(0x499b, 0x499d); L = 0xc6; // Part.counter1
  CYC(0x499d, 0x499f); mem_wr(gb, HL, 0xf0);
  CYC(0x499f, 0x49a1); L = 0xe4; // Part.collisionType
  CYC(0x49a1, 0x49a3); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(0x49a3, 0x49a5); A = 0x02; // TILEINDEX_RESPAWNING_BUSH_CUT
  CYC(0x49a5, 0x49a8); push_effect(gb, 0x49a8); respawnableBush_setTileHere_hook(gb);
  CALL_C(0x49a8, getRandomNumber_noPreserveVars_hook, 0x0453, 0x49ab);
  CYC(0x49ab, 0x49ac); alu_rrca(gb);
  if (!(F & FC)) { CYCT(0x49ac, 0x49ae); goto doneItemDropSpawn; } // jr nc
  CYC(0x49ac, 0x49ae);
  CALL_C(0x49ae, getFreePartSlot_hook, 0x3e8e, 0x49b1);
  if (!(F & FZ)) { CYCT(0x49b1, 0x49b3); goto doneItemDropSpawn; } // jr nz
  CYC(0x49b1, 0x49b3);
  CYC(0x49b3, 0x49b5); mem_wr(gb, HL, 0x01); // PART_ITEM_DROP
  CYC(0x49b5, 0x49b6); L = alu_inc8(gb, L);
  CYC(0x49b6, 0x49b7); E = L;
  CYC(0x49b7, 0x49b8); A = mem_rd(gb, DE);
  CYC(0x49b8, 0x49b9); mem_wr(gb, HL, A); // [itemDrop.subid] = [this.subid]
  CALL_C(0x49b9, objectCopyPosition_hook, 0x2242, 0x49bc);

doneItemDropSpawn:
  CYC(0x49bc, 0x49be); B = 0x00; // INTERAC_GRASSDEBRIS
  CALL_C(0x49be, objectCreateInteractionWithSubid00_hook, 0x24c3, 0x49c1);

normalStatus:
  CYC(0x49c1, 0x49c3); E = 0xc4; // Part.state
  CYC(0x49c3, 0x49c4); A = mem_rd(gb, DE);
  {
    CYC(0x49c4, 0x49c5); push_effect(gb, 0x49c5);
    uint16_t target = respawnableBush_jump_table(gb);
    if (target == 0x49cf) goto state0;
    if (target == 0x49d3) goto state1;
    if (target == 0x49d4) goto state2;
    if (target == 0x49e5) goto state3;
    goto state4;
  }

state0:
  CYC(0x49cf, 0x49d1); A = 0x01;
  CYC(0x49d1, 0x49d2); mem_wr(gb, DE, A);
  RET(0x49d2); return; // ret

state1:
  RET(0x49d3); return; // ret

state2:
  CYC(0x49d4, 0x49d7); A = mem_rd(gb, 0xcc00); // wFrameCounter
  CYC(0x49d7, 0x49d8); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(0x49d8); return; } // ret nc
  CYC(0x49d8, 0x49d9);
  CALL_C(0x49d9, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x49dc);
  if (!(F & FZ)) { RET_TAKEN(0x49dc); return; } // ret nz
  CYC(0x49dc, 0x49dd);
  CYC(0x49dd, 0x49df); mem_wr(gb, HL, 0x0c); // [counter1]
  CYC(0x49df, 0x49e0); L = E;
  CYC(0x49e0, 0x49e1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 3
  CYC(0x49e1, 0x49e3); A = 0x03; // TILEINDEX_RESPAWNING_BUSH_REGEN
  CYC(0x49e3, 0x49e5); respawnableBush_setTileHere_hook(gb); return; // jr

state3:
  CALL_C(0x49e5, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x49e8);
  if (!(F & FZ)) { RET_TAKEN(0x49e8); return; } // ret nz
  CYC(0x49e8, 0x49e9);
  CYC(0x49e9, 0x49eb); mem_wr(gb, HL, 0x08); // [counter1]
  CYC(0x49eb, 0x49ec); L = E;
  CYC(0x49ec, 0x49ed); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 4
  CYC(0x49ed, 0x49ef); A = 0x04; // TILEINDEX_RESPAWNING_BUSH_READY
  respawnableBush_setTileHere_hook(gb); return; // falls through

state4:
  CALL_C(0x49f8, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x49fb);
  if (!(F & FZ)) { RET_TAKEN(0x49fb); return; } // ret nz
  CYC(0x49fb, 0x49fc);
  CYC(0x49fc, 0x49fd); L = E;
  CYC(0x49fd, 0x49ff); mem_wr(gb, HL, 0x01); // [state] = 1
  CYC(0x49ff, 0x4a01); L = 0xe4; // Part.collisionType
  CYC(0x4a01, 0x4a03); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  RET(0x4a03); return; // ret
}

void respawnableBush_setTileHere_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x49ef, 0x49f0); push_effect(gb, (uint16_t)((A << 8) | F));
  CALL_C(0x49f0, objectGetShortPosition_hook, 0x2096, 0x49f3);
  CYC(0x49f3, 0x49f4); C = A;
  {
    uint16_t af = pop_effect(gb);
    A = (uint8_t)(af >> 8); F = (uint8_t)(af & 0xff);
  }
  CYC(0x49f4, 0x49f5);
  CYC(0x49f5, 0x49f8); setTile_hook(gb); return; // jp
}
