#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t black_tower_door_jump_table(GB *gb) {
  burn_rom(gb, 0, 0, 1, false); alu_add(gb, A); burn_rom(gb, 0, 1, 2, false); SET_HL(pop_effect(gb)); burn_rom(gb, 0, 2, 3, false); alu_add(gb, L); burn_rom(gb, 0, 3, 4, false); L = A;
  if (F & FC) { burn_rom(gb, 0, 4, 6, false); burn_rom(gb, 0, 6, 7, false); H = alu_inc8(gb, H); } else burn_rom(gb, 0, 4, 6, true);
  burn_rom(gb, 0, 7, 8, false); A = mem_rd(gb, HL); SET_HL(HL + 1); burn_rom(gb, 0, 8, 9, false); H = mem_rd(gb, HL); burn_rom(gb, 0, 9, 10, false); L = A; burn_rom(gb, 0, 10, 11, false); return HL;
}

void interactionCodec6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x74c7,0x74c9); E=INTERACTION_BASE+OBJ_STATE; CYC(0x74c9,0x74ca); A=mem_rd(gb,DE); CYC(0x74ca,0x74cb); push_effect(gb,0x74cb);
  switch (black_tower_door_jump_table(gb)) {
    case 0x74d1:
      CALL_C(0x74d1,getThisRoomFlags_hook,0x197d,0x74d4); CYC(0x74d4,0x74d6); alu_and(gb,0x40);
      if (F&FZ) { CYCT(0x74d6,0x74d8); goto cutscene; }
      CYC(0x74d6,0x74d8); CYC(0x74d8,0x74db); SET_HL(wRoomLayout+0x47); CYC(0x74db,0x74dd); mem_wr(gb,HL,0x44); CYC(0x74dd,0x74e0); interactionDelete_hook(gb); return;
cutscene:
      CYC(0x74e0,0x74e2); A=0x36; CALL_C(0x74e2,checkTreasureObtained_hook,0x1748,0x74e5);
      if (!(F&FC)) { CYCT(0x74e5,0x74e7); goto no_seed; }
      CYC(0x74e5,0x74e7); CALL_C(0x74e7,clearAllItemsAndPutLinkOnGround_hook,0x19ad,0x74ea); CALL_C(0x74ea,resetLinkInvincibility_hook,0x2ba9,0x74ed);
      CYC(0x74ed,0x74ef); A=0x0b; CYC(0x74ef,0x74f2); W8(wLinkForceState)=A; CYC(0x74f2,0x74f4); A=0x70; CYC(0x74f4,0x74f7); W8(wLinkStateParameter)=A; CYC(0x74f7,0x74f9); E=0x46; CYC(0x74f9,0x74fa); mem_wr(gb,DE,A); CYC(0x74fa,0x74fd); SET_HL(w1Link_direction); CYC(0x74fd,0x74ff); mem_wr(gb,HL,1); CYC(0x74ff,0x7500); L=alu_inc8(gb,L); CYC(0x7500,0x7502); mem_wr(gb,HL,8); CYC(0x7502,0x7504); A=0x81; CYC(0x7504,0x7507); W8(wDisabledObjects)=A; CYC(0x7507,0x750a); W8(wMenuDisabled)=A; CALL_C(0x750a,interactionIncState_hook,0x23e0,0x750d); CYC(0x750d,0x750f); A=0xab; CALL_C(0x750f,loadPaletteHeader_hook,0x050b,0x7512); CYC(0x7512,0x7515); restartSound_hook(gb); return;
no_seed:
      CYC(0x7515,0x7517); A=0x44; CYC(0x7517,0x751a); SET_HL(wRoomLayout+0x44); CYC(0x751a,0x751b); mem_wr(gb,HL,A); CYC(0x751b,0x751d); L=0x47; CYC(0x751d,0x751e); mem_wr(gb,HL,A); CYC(0x751e,0x7520); L=0x4a; CYC(0x7520,0x7521); mem_wr(gb,HL,A); CYC(0x7521,0x7524); W8(wDisableWarps)=A; CYC(0x7524,0x7527); interactionDelete_hook(gb); return;
    case 0x7527: CALL_C(0x7527,interactionDecCounter1_hook,0x23cc,0x752a); if (!(F&FZ)) { CYCT(0x752a,0x752b); ret_effect(gb); return; } CYC(0x752a,0x752b); CYC(0x752b,0x752d); mem_wr(gb,HL,0x1e); CYC(0x752d,0x752e); A=0; CYC(0x752e,0x7531); SET_HL(w1Link_direction); CYC(0x7531,0x7532); mem_wr(gb,HL,A); SET_HL(HL+1); CYC(0x7532,0x7533); mem_wr(gb,HL,A); CYC(0x7533,0x7536); interactionIncState_hook(gb); return;
    case 0x7536: CALL_C(0x7536,interactionDecCounter1_hook,0x23cc,0x7539); if (!(F&FZ)) { CYCT(0x7539,0x753a); ret_effect(gb); return; } CYC(0x7539,0x753a); CYC(0x753a,0x753c); B=0xd7; CALL_C(0x753c,objectCreateInteractionWithSubid00_hook,0x24c3,0x753f); CYC(0x753f,0x7542); interactionDelete_hook(gb); return;
    default: hook_continue(gb,HL,sp0_); return;
  }
}
