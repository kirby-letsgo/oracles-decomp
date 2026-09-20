#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCodec6), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCodec6), (from), (to), true)

static uint16_t black_tower_door_jump_table(GB *gb) {
  burn_rom(gb, 0, 0, 1, false); alu_add(gb, A); burn_rom(gb, 0, 1, 2, false); SET_HL(pop_effect(gb)); burn_rom(gb, 0, 2, 3, false); alu_add(gb, L); burn_rom(gb, 0, 3, 4, false); L = A;
  if (F & FC) { burn_rom(gb, 0, 4, 6, false); burn_rom(gb, 0, 6, 7, false); H = alu_inc8(gb, H); } else burn_rom(gb, 0, 4, 6, true);
  burn_rom(gb, 0, 7, 8, false); A = mem_rd(gb, HL); SET_HL(HL + 1); burn_rom(gb, 0, 8, 9, false); H = mem_rd(gb, HL); burn_rom(gb, 0, 9, 10, false); L = A; burn_rom(gb, 0, 10, 11, false); return HL;
}

void interactionCodec6_hook(GB *gb) {
  BASE(interactionCodec6);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0,b_+2); E=INTERACTION_BASE+OBJ_STATE; CYC(b_+2,b_+3); A=mem_rd(gb,DE); CYC(b_+3,b_+4); push_effect(gb,b_+4);
  do { uint16_t jt_ = (black_tower_door_jump_table(gb));
    if (jt_ == b_+10) {
      CALL_C(b_+10,getThisRoomFlags_hook,SYM(getThisRoomFlags),b_+13); CYC(b_+13,b_+15); alu_and(gb,0x40);
      if (F&FZ) { CYCT(b_+15,b_+17); goto cutscene; }
      CYC(b_+15,b_+17); CYC(b_+17,b_+20); SET_HL(wRoomLayout+0x47); CYC(b_+20,b_+22); mem_wr(gb,HL,0x44); CYC(b_+22,b_+25); interactionDelete_hook(gb); return;
cutscene:
      CYC(b_+25,b_+27); A=0x36; CALL_C(b_+27,checkTreasureObtained_hook,SYM(checkTreasureObtained),b_+30);
      if (!(F&FC)) { CYCT(b_+30,b_+32); goto no_seed; }
      CYC(b_+30,b_+32); CALL_C(b_+32,clearAllItemsAndPutLinkOnGround_hook,SYM(clearAllItemsAndPutLinkOnGround),b_+35); CALL_C(b_+35,resetLinkInvincibility_hook,SYM(resetLinkInvincibility),b_+38);
      CYC(b_+38,b_+40); A=0x0b; CYC(b_+40,b_+43); W8(wLinkForceState)=A; CYC(b_+43,b_+45); A=0x70; CYC(b_+45,b_+48); W8(wLinkStateParameter)=A; CYC(b_+48,b_+50); E=0x46; CYC(b_+50,b_+51); mem_wr(gb,DE,A); CYC(b_+51,b_+54); SET_HL(w1Link_direction); CYC(b_+54,b_+56); mem_wr(gb,HL,1); CYC(b_+56,b_+57); L=alu_inc8(gb,L); CYC(b_+57,b_+59); mem_wr(gb,HL,8); CYC(b_+59,b_+61); A=0x81; CYC(b_+61,b_+64); W8(wDisabledObjects)=A; CYC(b_+64,b_+67); W8(wMenuDisabled)=A; CALL_C(b_+67,interactionIncState_hook,SYM(interactionIncState),b_+70); CYC(b_+70,b_+72); A=0xab; CALL_C(b_+72,loadPaletteHeader_hook,SYM(loadPaletteHeader),b_+75); CYC(b_+75,b_+78); restartSound_hook(gb); return;
no_seed:
      CYC(b_+78,b_+80); A=0x44; CYC(b_+80,b_+83); SET_HL(wRoomLayout+0x44); CYC(b_+83,b_+84); mem_wr(gb,HL,A); CYC(b_+84,b_+86); L=0x47; CYC(b_+86,b_+87); mem_wr(gb,HL,A); CYC(b_+87,b_+89); L=0x4a; CYC(b_+89,b_+90); mem_wr(gb,HL,A); CYC(b_+90,b_+93); W8(wDisableWarps)=A; CYC(b_+93,b_+96); interactionDelete_hook(gb); return;
    }
    else if (jt_ == b_+96) { CALL_C(b_+96,interactionDecCounter1_hook,SYM(interactionDecCounter1),b_+99); if (!(F&FZ)) { CYCT(b_+99,b_+100); ret_effect(gb); return; } CYC(b_+99,b_+100); CYC(b_+100,b_+102); mem_wr(gb,HL,0x1e); CYC(b_+102,b_+103); A=0; CYC(b_+103,b_+106); SET_HL(w1Link_direction); CYC(b_+106,b_+107); mem_wr(gb,HL,A); SET_HL(HL+1); CYC(b_+107,b_+108); mem_wr(gb,HL,A); CYC(b_+108,b_+111); interactionIncState_hook(gb); return; }
    else if (jt_ == b_+111) { CALL_C(b_+111,interactionDecCounter1_hook,SYM(interactionDecCounter1),b_+114); if (!(F&FZ)) { CYCT(b_+114,b_+115); ret_effect(gb); return; } CYC(b_+114,b_+115); CYC(b_+115,b_+117); B=0xd7; CALL_C(b_+117,objectCreateInteractionWithSubid00_hook,SYM(objectCreateInteractionWithSubid00),b_+120); CYC(b_+120,SYM(interactionCodec8)); interactionDelete_hook(gb); return; }
    else { hook_continue(gb,HL,sp0_); return; }
  } while (0);
}
