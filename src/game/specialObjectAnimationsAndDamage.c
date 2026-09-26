#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

#define specialObjectAnimationData_bank06 SYM(specialObjectAnimationTable)

void specialObjectSetAnimation_data_hook(GB *gb);
void label_06_032_hook(GB *gb);

void linkApplyDamage_b06_hook(GB *gb) {
  BASE(linkApplyDamage_b06);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x25;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+7, b_+9); goto damage_knockback; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); B = A;
  CYC(b_+10, b_+12); A = 0x3f;
  CALL_C(b_+12, cpActiveRing_hook, SYM(cpActiveRing), b_+15);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto apply_damage; }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); B = 0xf8;
apply_damage:
  CYC(b_+19, b_+21); L = 0x29;
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+23); alu_add(gb, B);
  CYC(b_+23, b_+24); mem_wr(gb, HL, A);
damage_knockback:
  CYC(b_+24, b_+26); L = 0x2a;
  CYC(b_+26, b_+27); A = mem_rd(gb, HL);
  CYC(b_+27, b_+28); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+28, b_+30); goto normalize_health; }
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+32); A = 0x10;
  CALL_C(b_+32, cpActiveRing_hook, SYM(cpActiveRing), b_+35);
  if (!(F & FZ)) { CYCT(b_+35, b_+37); goto normalize_health; }
  CYC(b_+35, b_+37);
  CYC(b_+37, b_+39); L = 0x2d;
  CYC(b_+39, b_+41); mem_wr(gb, HL, alu_srl(gb, mem_rd(gb, HL)));
normalize_health:
  CYC(b_+41, b_+44); SET_HL(wLinkHealth);
  CYC(b_+44, b_+46); E = 0x29;
  CYC(b_+46, b_+47); A = mem_rd(gb, DE);
  CYC(b_+47, b_+49); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(b_+49, b_+51); goto potion; }
  CYC(b_+49, b_+51);
  CYC(b_+51, b_+52); A = mem_rd(gb, DE);
normalize_loop:
  CYC(b_+52, b_+53); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+53, b_+55); alu_add(gb, 2);
  if (!(F & FC)) { CYCT(b_+55, b_+57); goto normalize_loop; }
  CYC(b_+55, b_+57);
  CYC(b_+57, b_+58); mem_wr(gb, DE, A);
potion:
  CYC(b_+58, b_+59); A = mem_rd(gb, HL);
  CYC(b_+59, b_+60); A = alu_dec8(gb, A);
  CYC(b_+60, b_+61); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+61, b_+63); goto finish; }
  CYC(b_+61, b_+63);
  CYC(b_+63, b_+65); A = 0x2f;
  CALL_C(b_+65, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+68);
  if (!(F & FC)) { CYCT(b_+68, b_+70); goto no_potion; }
  CYC(b_+68, b_+70);
  CYC(b_+70, b_+73); SET_HL(wLinkMaxHealth);
  CYC(b_+73, b_+74); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+74, b_+75); mem_wr(gb, HL, A);
  CYC(b_+75, b_+77); A = 1;
  CYC(b_+77, b_+78); mem_wr(gb, DE, A);
  CYC(b_+78, b_+80); A = 0x2f;
  CALL_C(b_+80, loseTreasure_hook, SYM(loseTreasure), b_+83);
  CYC(b_+83, b_+85); goto finish;
no_potion:
  CYC(b_+85, b_+86); alu_xor(gb, A);
  CYC(b_+86, b_+87); mem_wr(gb, DE, A);
  CYC(b_+87, b_+88); mem_wr(gb, HL, A);
  CYC(b_+88, b_+91); mem_wr(gb, wUsingShield, A);
  CYC(b_+91, b_+93); E = 0x04;
  CYC(b_+93, b_+94); A = mem_rd(gb, DE);
  CYC(b_+94, b_+96); alu_cp(gb, 0x0d);
  if (F & FZ) { CYCT(b_+96, b_+98); goto finish; }
  CYC(b_+96, b_+98);
  CYC(b_+98, b_+100); A = 0xff;
  CYC(b_+100, b_+103); mem_wr(gb, wLinkDeathTrigger, A);
  CALL_C(b_+103, clearAllParentItems_hook, SYM(clearAllParentItems), b_+106);
finish:
  CYC(b_+106, b_+109); A = mem_rd(gb, wFrameCounter);
  CYC(b_+109, b_+110); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+110, b_+112); goto end; }
  CYC(b_+110, b_+112);
  CYC(b_+112, b_+114); E = 0x2e;
  CYC(b_+114, b_+115); A = mem_rd(gb, DE);
  CYC(b_+115, b_+116); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+116, b_+118); goto end; }
  CYC(b_+116, b_+118);
  CYC(b_+118, b_+119); A = alu_dec8(gb, A);
  CYC(b_+119, b_+120); mem_wr(gb, DE, A);
end:
  CYC(b_+120, b_+121); ret_effect(gb);
}

static void add_a_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
    ret_effect(gb);
    return;
  }
  burn_rom(gb, 0x00, 0x0012, 0x0013, false);
  burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
  burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  ret_effect(gb);
}

static void add_double_index_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false);
  ret_effect(gb);
}

void load_animation_frame_hook(GB *gb) {
  BASE(loadLinkAndCompanionAnimationFrame_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+43, b_+45); L = 0x32;
  CYC(b_+45, b_+46); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+46, b_+47); ret_effect(gb); return; }
  CYC(b_+46, b_+47);
  CYC(b_+47, b_+48); mem_wr(gb, HL, A);
  CALL_C(b_+48, getSpecialObjectGraphicsFrame_hook, SYM(getSpecialObjectGraphicsFrame), b_+51);
  if (F & FZ) { CYCT(b_+51, b_+52); ret_effect(gb); return; }
  CYC(b_+51, b_+52);
  CYC(b_+52, b_+54); E = 0x01;
  CYC(b_+54, b_+55); A = mem_rd(gb, DE);
  CYC(b_+55, b_+57); alu_cp(gb, 0x0a);
  CYC(b_+57, b_+60); SET_DE(0x8701);
  if (F & FC) { CYCT(b_+60, b_+62); goto queue; }
  CYC(b_+60, b_+62);
  CYC(b_+62, b_+64); D = 0x86;
queue:
  CYC(b_+64, b_+67);
  TAIL(queueDmaTransfer);
}

void loadLinkAndCompanionAnimationFrame_body_hook(GB *gb) {
  BASE(loadLinkAndCompanionAnimationFrame_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0xff;
  CYC(b_+2, b_+5); mem_wr(gb, wLinkPushingDirection, A);
  CYC(b_+5, b_+8); A = mem_rd(gb, w1Link_visible);
  CYC(b_+8, b_+9); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+9, b_+11); goto companion; }
  CYC(b_+9, b_+11);
  CALL_C(b_+11, func_4553_hook, SYM(func_4553), b_+14);
  CYC(b_+14, b_+17); A = mem_rd(gb, w1Link_id);
  CYC(b_+17, b_+20); SET_HL(b_+67);
  CYC(b_+20, b_+21); add_a_to_hl(gb, b_+21);
  CYC(b_+21, b_+22); A = B;
  CYC(b_+22, b_+23); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+23, b_+25); goto load; }
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+28); A = mem_rd(gb, w1Link_direction);
  CYC(b_+28, b_+29); alu_add(gb, B);
load:
  CYC(b_+29, b_+31); H = 0xd0;
  CYC(b_+31, b_+34); push_effect(gb, b_+34);
  load_animation_frame_hook(gb);
companion:
  CYC(b_+34, b_+37); SET_HL(w1Companion_visible);
  CYC(b_+37, b_+39); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+39, b_+40); ret_effect(gb); return; }
  CYC(b_+39, b_+40);
  CYC(b_+40, b_+42); L = 0x31;
  CYC(b_+42, b_+43); A = mem_rd(gb, HL);
  load_animation_frame_hook(gb);
}

void getSpecialObjectGraphicsFrame_hook(GB *gb) {
  BASE(getSpecialObjectGraphicsFrame);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); C = A;
  CYC(b_+1, b_+3); B = 0;
  CYC(b_+3, b_+4); D = H;
  CYC(b_+4, b_+6); L = 0x01;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+8); E = A;
  CYC(b_+8, b_+11); SET_HL(SYM(specialObjectGraphicsTable));
  CYC(b_+11, b_+12); add_double_index_to_hl(gb, b_+12);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+13, b_+14); H = mem_rd(gb, HL);
  CYC(b_+14, b_+15); L = A;
  CYC(b_+15, b_+16); alu_add_hl(gb, BC);
  CYC(b_+16, b_+17); alu_add_hl(gb, BC);
  CYC(b_+17, b_+18); alu_add_hl(gb, BC);
  CYC(b_+18, b_+19); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+19, b_+20); push_effect(gb, HL);
  CYC(b_+20, b_+21); alu_add(gb, A);
  CYC(b_+21, b_+22); C = A;
  CYC(b_+22, b_+23); A = E;
  CYC(b_+23, b_+26); SET_HL(SYM(specialObjectOamDataTable));
  CYC(b_+26, b_+27); add_double_index_to_hl(gb, b_+27);
  CYC(b_+27, b_+28); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+28, b_+29); H = mem_rd(gb, HL);
  CYC(b_+29, b_+30); L = A;
  CYC(b_+30, b_+31); alu_add_hl(gb, BC);
  CYC(b_+31, b_+33); E = 0x1e;
  CYC(b_+33, b_+34); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  CYC(b_+35, b_+36); E = alu_inc8(gb, E);
  CYC(b_+36, b_+37); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+37, b_+39); alu_and(gb, 0x3f);
  CYC(b_+39, b_+40); mem_wr(gb, DE, A);
  CYC(b_+40, b_+41); SET_HL(pop_effect(gb));
  CYC(b_+41, b_+42); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+42, b_+43); H = mem_rd(gb, HL);
  CYC(b_+43, b_+44); L = A;
  CYC(b_+44, b_+45); alu_or(gb, H);
  if (F & FZ) { CYCT(b_+45, b_+46); ret_effect(gb); return; }
  CYC(b_+45, b_+46);
  CYC(b_+46, b_+47); A = L;
  CYC(b_+47, b_+49); alu_and(gb, 0x01);
  CYC(b_+49, b_+51); alu_add(gb, 0x1a);
  CYC(b_+51, b_+52); C = A;
  CYC(b_+52, b_+53); A = L;
  CYC(b_+53, b_+55); alu_and(gb, 0x1e);
  CYC(b_+55, b_+56); A = alu_dec8(gb, A);
  CYC(b_+56, b_+57); B = A;
  CYC(b_+57, b_+59); L &= 0xef;
  CYC(b_+59, b_+60); alu_or(gb, D);
  CYC(b_+60, b_+61);
  ret_effect(gb);
}

void specialObjectSetAnimationWithLinkData_hook(GB *gb) {
  BASE(specialObjectSetAnimationWithLinkData);
  CYC(b_+0, b_+2); E = 0x30;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  CYC(b_+3, b_+4); alu_add(gb, A);
  CYC(b_+4, b_+5); C = A;
  CYC(b_+5, b_+7); B = 0;
  CYC(b_+7, b_+10); A = mem_rd(gb, w1Link_id);
  CYC(b_+10, b_+12);
  TAIL(label_06_032);
}

void specialObjectLoadAnimationFrameToBuffer_hook(GB *gb) {
  BASE(specialObjectLoadAnimationFrameToBuffer);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(w1Companion_visible);
  CYC(b_+3, b_+5); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); L = 0x32;
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CALL_C(b_+9, getSpecialObjectGraphicsFrame_hook, SYM(getSpecialObjectGraphicsFrame), b_+12);
  if (F & FZ) { CYCT(b_+12, b_+13); ret_effect(gb); return; }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+14); A = L;
  CYC(b_+14, b_+16); alu_and(gb, 0xf0);
  CYC(b_+16, b_+17); L = A;
  CYC(b_+17, b_+20); SET_DE(w1WeaponItem_counter1);
  CYC(b_+20, b_+23);
  TAIL(copy256BytesFromBank);
}

void specialObjectNextAnimationFrame_hook(GB *gb) {
  BASE(specialObjectNextAnimationFrame);
  CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+1, b_+2); H = mem_rd(gb, HL);
  CYC(b_+2, b_+3); L = A;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+6); alu_cp(gb, 0xff);
  if (!(F & FZ)) { CYCT(b_+6, b_+8); goto write; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); C = mem_rd(gb, HL);
  CYC(b_+9, b_+10); B = A;
  CYC(b_+10, b_+11); alu_add_hl(gb, BC);
  CYC(b_+11, b_+12); A = mem_rd(gb, HL); SET_HL(HL + 1);
write:
  CYC(b_+12, b_+14); E = 0x20;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+16); E = alu_inc8(gb, E);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+17, b_+18); C = A;
  CYC(b_+18, b_+19); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CYC(b_+20, b_+21); E = alu_inc8(gb, E);
  CYC(b_+21, b_+22); A = L;
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CYC(b_+23, b_+24); E = alu_inc8(gb, E);
  CYC(b_+24, b_+25); A = H;
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  CYC(b_+26, b_+28); E = 0x31;
  CYC(b_+28, b_+29); A = C;
  CYC(b_+29, b_+30); mem_wr(gb, DE, A);
  CYC(b_+30, b_+31);
  ret_effect(gb);
}

void specialObjectSetAnimation_data_hook(GB *gb) {
  BASE(label_06_032);
  CYC(b_+0, b_+3); SET_HL(specialObjectAnimationData_bank06);
  CYC(b_+3, b_+4); add_double_index_to_hl(gb, b_+4);
  CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+5, b_+6); H = mem_rd(gb, HL);
  CYC(b_+6, b_+7); L = A;
  CYC(b_+7, b_+8); alu_add_hl(gb, BC);
  TAIL(specialObjectNextAnimationFrame);
}

void label_06_032_hook(GB *gb) {
  specialObjectSetAnimation_data_hook(gb);
}

void specialObjectSetAnimation_body_hook(GB *gb) {
  BASE(specialObjectSetAnimation_body);
  CYC(b_+0, b_+2); E = 0x01;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  specialObjectSetAnimation_data_hook(gb);
}

void specialObjectAnimate_optimized_hook(GB *gb) {
  BASE(specialObjectAnimate_optimized);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x20;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); L = 0x22;
  CYC(b_+7, b_+9);
  TAIL(specialObjectNextAnimationFrame);
}

void fake_specialObjectLoadAnimationFrameToBuffer_hook(GB *gb) {
  BASE(fake_specialObjectLoadAnimationFrameToBuffer);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(w1Companion_visible);
  CYC(b_+3, b_+5); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); L = 0x32;
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CALL_ROM(b_+9, (SYM(getSpecialObjectGraphicsFrame) + 14));
  if (F & FZ) { CYCT(b_+12, b_+13); ret_effect(gb); return; }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+14); A = L;
  CYC(b_+14, b_+16); alu_and(gb, 0xf0);
  CYC(b_+16, b_+17); L = A;
  CYC(b_+17, b_+20); SET_DE(w1WeaponItem_counter1);
  CYC(b_+20, b_+23);
  HANDOFF((SYM(func_3ee4) + 51));
}

void linkUpdateDamageToApplyForRings_hook(GB *gb) {
  BASE(linkUpdateDamageToApplyForRings);
  CYC(b_+0, b_+2); E = 0x25;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+9); SET_HL(b_+70);
  CYC(b_+9, b_+12); A = mem_rd(gb, wActiveRing);
  CYC(b_+12, b_+13); E = A;
  for (;;) {
    CYC(b_+13, b_+14); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+14, b_+15); alu_or(gb, A);
    if (F & FZ) { CYCT(b_+15, b_+17); goto no_match; }
    CYC(b_+15, b_+17);
    CYC(b_+17, b_+18); alu_cp(gb, E);
    if (F & FZ) { CYCT(b_+18, b_+20); goto matching; }
    CYC(b_+18, b_+20);
    CYC(b_+20, b_+21); SET_HL(HL + 1);
    CYC(b_+21, b_+23);
  }
no_match:
  CYC(b_+23, b_+24); A = E;
  CYC(b_+24, b_+26); alu_cp(gb, 0x08);
  if (F & FZ) { CYCT(b_+26, b_+28); goto blue; }
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+30); alu_cp(gb, 0x09);
  if (F & FZ) { CYCT(b_+30, b_+32); goto green; }
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+34); alu_cp(gb, 0x0a);
  if (!(F & FZ)) { CYCT(b_+34, b_+35); ret_effect(gb); return; }
  CYC(b_+34, b_+35);
  CYC(b_+35, b_+36); A = B;
  CYC(b_+36, b_+37); alu_add(gb, A);
  CYC(b_+37, b_+39); goto write_damage;
blue:
  CYC(b_+39, b_+40); A = B;
  CYC(b_+40, b_+42); alu_sra(gb, A);
  CYC(b_+42, b_+44); goto write_damage;
green:
  CYC(b_+44, b_+45); A = B;
  CYC(b_+45, b_+46); alu_cpl(gb);
  CYC(b_+46, b_+47); A = alu_inc8(gb, A);
  CYC(b_+47, b_+48); alu_add(gb, A);
  CYC(b_+48, b_+49); alu_add(gb, A);
  CYC(b_+49, b_+50); alu_add(gb, B);
  CYC(b_+50, b_+52); alu_sra(gb, A);
  CYC(b_+52, b_+54); alu_sra(gb, A);
  CYC(b_+54, b_+55); alu_cpl(gb);
  CYC(b_+55, b_+56); A = alu_inc8(gb, A);
  CYC(b_+56, b_+58); goto write_damage;
matching:
  CYC(b_+58, b_+59); A = mem_rd(gb, HL);
  CYC(b_+59, b_+60); alu_add(gb, B);
write_damage:
  CYC(b_+60, b_+62); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+62, b_+64); goto store; }
  CYC(b_+62, b_+64);
  CYC(b_+64, b_+66); A = 0xff;
store:
  CYC(b_+66, b_+68); E = 0x25;
  CYC(b_+68, b_+69); mem_wr(gb, DE, A);
  CYC(b_+69, b_+70);
  ret_effect(gb);
}

void getTransformedLinkID_hook(GB *gb) {
  BASE(getTransformedLinkID);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wDisableRingTransformations);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) CYCT(b_+5, b_+7);
  else {
    CYC(b_+5, b_+7);
    CYC(b_+7, b_+8); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    CYC(b_+8, b_+10);
    goto zero;
  }
  CYC(b_+10, b_+13); A = mem_rd(gb, wTilesetFlags);
  CYC(b_+13, b_+15); alu_and(gb, 0x60);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto zero; }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+20); A = mem_rd(gb, wMenuDisabled);
  CYC(b_+20, b_+21); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+21, b_+23); goto zero; }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+26); A = mem_rd(gb, wAButtonSensitiveObjectListEnd);
  CYC(b_+26, b_+27); B = A;
  CYC(b_+27, b_+30); A = mem_rd(gb, wLinkGrabState);
  CYC(b_+30, b_+31); alu_or(gb, B);
  if (!(F & FZ)) { CYCT(b_+31, b_+33); goto zero; }
  CYC(b_+31, b_+33);
  CYC(b_+33, b_+36); A = mem_rd(gb, wActiveRing);
  CYC(b_+36, b_+37); E = A;
  CYC(b_+37, b_+40); SET_HL(b_+48);
  CALL_C(b_+40, lookupKey_hook, SYM(lookupKey), b_+43);
  CYC(b_+43, b_+44); B = A;
  CYC(b_+44, b_+45);
  ret_effect(gb);
  return;
zero:
  CYC(b_+45, b_+47); B = 0;
  CYC(b_+47, b_+48);
  ret_effect(gb);
}

// func_4553@getLinkWalkingAnimation's underwater preamble exists only in Ages, where the label
// sits 23 bytes before @notUnderwater; in Seasons the two labels coincide and this burns nothing.
// *found is set when the preamble jumped to @animationFound, cleared when it fell into @notUnderwater.
static void link_walking_animation_underwater(GB *gb, bool *found) {
  BASE(func_4553__getLinkWalkingAnimation);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  *found = false;
  if (b_ == SYM(func_4553__notUnderwater)) return;
  CYC(b_+0, b_+2); C = 0x0a;
  CYC(b_+2, b_+5); A = mem_rd(gb, wTilesetFlags);
  alu_and(gb, 0x40);
  CYC(b_+5, b_+7);
  if (F & FZ) { CYCT(b_+7, b_+9); return; }
  CYC(b_+7, b_+9);
  *found = true;
  CALL_C(b_+9, checkLinkPushingAgainstWall_hook, SYM(checkLinkPushingAgainstWall), b_+12);
  if (!(F & FC)) { CYCT(b_+12, b_+15); return; }
  CYC(b_+12, b_+15);
  CYC(b_+15, b_+18); A = mem_rd(gb, w1Link_direction);
  CYC(b_+18, b_+21); mem_wr(gb, wLinkPushingDirection, A);
  CYCT(b_+21, b_+23);
}

static void get_link_walking_animation(GB *gb) {
  bool found;
  link_walking_animation_underwater(gb, &found);
  BASE(func_4553__notUnderwater);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  if (found) goto animationFound;
  CYC(b_+0, b_+2); C = 0x00;
  CYC(b_+2, b_+5); A = mem_rd(gb, wLinkGrabState);
  alu_bit(gb, 6, A);
  CYC(b_+5, b_+7);
  if (!(F & FZ)) { CYCT(b_+7, b_+8); return; }
  CYC(b_+7, b_+8);
  alu_or(gb, A);
  CYC(b_+8, b_+9);
  if (F & FZ) CYCT(b_+9, b_+11);
  else {
    CYC(b_+9, b_+11);
    C = 0x02;
    CYC(b_+11, b_+13);
  }
  CYC(b_+13, b_+16); A = mem_rd(gb, wLinkObjectIndex);
  alu_rrca(gb);
  CYC(b_+16, b_+17);
  if (!(F & FC)) CYCT(b_+17, b_+19);
  else {
    CYC(b_+17, b_+19);
    CYC(b_+19, b_+22); A = mem_rd(gb, w1Companion_id);
    alu_cp(gb, 0x0a);
    CYC(b_+22, b_+24);
    if (!(F & FZ)) CYCT(b_+24, b_+26);
    else {
      CYC(b_+24, b_+26);
      C = alu_inc8(gb, C);
      CYC(b_+26, b_+27);
    }
  }
  A = C;
  alu_or(gb, A);
  CYC(b_+27, b_+29);
  if (!(F & FZ)) { CYCT(b_+29, b_+31); goto animationFound; }
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+34); A = mem_rd(gb, wMagnetGloveState);
  alu_or(gb, A);
  CYC(b_+34, b_+35);
  if (!(F & FZ)) {
    CYC(b_+35, b_+37);
    C = 0x09;
    CYC(b_+37, b_+39);
    CYCT(b_+39, b_+41);
    goto animationFound;
  }
  CYCT(b_+35, b_+37);
  CYC(b_+41, b_+44); A = mem_rd(gb, wUsingShield);
  alu_or(gb, A);
  CYC(b_+44, b_+45);
  if (!(F & FZ)) {
    CYC(b_+45, b_+47);
    C = 0x07;
    alu_cp(gb, 0x02);
    CYC(b_+47, b_+51);
    if (F & FC) { CYCT(b_+51, b_+53); goto animationFound; }
    CYC(b_+51, b_+53);
    C = alu_inc8(gb, C);
    CYC(b_+53, b_+54);
    CYCT(b_+54, b_+56);
    goto animationFound;
  }
  CYCT(b_+45, b_+47);
  CYC(b_+56, b_+59); A = mem_rd(gb, wLinkTurningDisabled);
  alu_or(gb, A);
  CYC(b_+59, b_+60);
  if (!(F & FZ)) { CYCT(b_+60, b_+62); goto standingAnimation; }
  CYC(b_+60, b_+62);
  CYC(b_+62, b_+65); A = mem_rd(gb, wForceLinkPushAnimation);
  A = alu_dec8(gb, A);
  CYC(b_+65, b_+66);
  if (F & FZ) { CYCT(b_+66, b_+68); goto pushingAnimation; }
  CYC(b_+66, b_+68);
  CYC(b_+68, b_+71); A = mem_rd(gb, wForceLinkPushAnimation);
  alu_rlca(gb);
  CYC(b_+71, b_+72);
  if (F & FC) { CYCT(b_+72, b_+74); goto standingAnimation; }
  CYC(b_+72, b_+74);
  CYC(b_+74, b_+77); A = mem_rd(gb, wLinkClimbingVine);
  L = A;
  CYC(b_+77, b_+78);
  CYC(b_+78, b_+81); A = mem_rd(gb, wTextIsActive);
  alu_or(gb, L);
  CYC(b_+81, b_+82);
  if (!(F & FZ)) { CYCT(b_+82, b_+84); goto standingAnimation; }
  CYC(b_+82, b_+84);
  CALL_C(b_+84, checkLinkPushingAgainstWall_hook, SYM(checkLinkPushingAgainstWall), b_+87);
  if (!(F & FC)) { CYCT(b_+87, b_+89); goto standingAnimation; }
  CYC(b_+87, b_+89);
pushingAnimation:
  CYC(b_+89, b_+92); A = mem_rd(gb, w1Link_direction);
  CYC(b_+92, b_+95); mem_wr(gb, wLinkPushingDirection, A);
  C = 0x04;
  CYC(b_+95, b_+97);
  CYCT(b_+97, b_+99);
  goto animationFound;
standingAnimation:
  CYC(b_+99, b_+102); A = mem_rd(gb, wInventoryA);
  alu_cp(gb, 0x01);
  CYC(b_+102, b_+104);
  if (F & FZ) CYCT(b_+104, b_+106);
  else {
    CYC(b_+104, b_+106);
    CYC(b_+106, b_+109); A = mem_rd(gb, wInventoryB);
    alu_cp(gb, 0x01);
    CYC(b_+109, b_+111);
    if (!(F & FZ)) { CYCT(b_+111, b_+113); goto animationFound; }
    CYC(b_+111, b_+113);
  }
  C = 0x05;
  CYC(b_+113, b_+115);
  CYC(b_+115, b_+118); A = mem_rd(gb, wShieldLevel);
  alu_cp(gb, 0x01);
  CYC(b_+118, b_+120);
  if (F & FZ) CYCT(b_+120, b_+122);
  else {
    CYC(b_+120, b_+122);
    C = 0x06;
    CYC(b_+122, b_+124);
  }
animationFound:
  CYC(b_+124, b_+127); A = mem_rd(gb, wLinkClimbingVine);
  alu_or(gb, A);
  CYC(b_+127, b_+128);
  if (F & FZ) CYCT(b_+128, b_+130);
  else {
    CYC(b_+128, b_+130);
    alu_xor(gb, A);
    CYC(b_+130, b_+131);
    CYC(b_+131, b_+134); mem_wr(gb, w1Link_direction, A);
  }
  A = C;
  alu_add(gb, A);
  alu_add(gb, A);
  CYC(b_+134, b_+137);
  CYC(b_+137, b_+140); mem_wr(gb, w1Link_var34, A);
  CYC(b_+140, b_+141);
}

void func_4553_hook(GB *gb) {
  BASE(func_4553);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, w1Link_id);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto scan; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, w1Link_var31);
  CYC(b_+9, b_+10); B = A;
  CYC(b_+10, b_+11);
  ret_effect(gb);
  return;
scan:
  CYC(b_+11, b_+14); SET_HL(w1ParentItem2);
  CYC(b_+14, b_+17); SET_BC(0);
  do {
    CYC(b_+17, b_+19); L = 0x3f;
    CYC(b_+19, b_+20); A = mem_rd(gb, HL);
    CYC(b_+20, b_+21); alu_cp(gb, C);
    if (F & FC) { CYCT(b_+21, b_+23); }
    else {
      CYC(b_+21, b_+23);
      CYC(b_+23, b_+24); C = A;
      CYC(b_+24, b_+26); L = 0x31;
      CYC(b_+26, b_+27); B = mem_rd(gb, HL);
    }
    CYC(b_+27, b_+28); H = alu_inc8(gb, H);
    CYC(b_+28, b_+29); A = H;
    CYC(b_+29, b_+31); alu_cp(gb, 0xd6);
    if (F & FC) { CYCT(b_+31, b_+33); continue; }
    CYC(b_+31, b_+33);
    break;
  } while (true);
  CYC(b_+33, b_+36); A = mem_rd(gb, w1Link_var3f);
  CYC(b_+36, b_+37); alu_cp(gb, C);
  if (F & FC) { CYCT(b_+37, b_+38); ret_effect(gb); return; }
  CYC(b_+37, b_+38);
  CYC(b_+38, b_+41); A = mem_rd(gb, w1Link_var31);
  CYC(b_+41, b_+42); B = A;
  CYC(b_+42, b_+45); A = mem_rd(gb, w1Link_animMode);
  CYC(b_+45, b_+47); alu_cp(gb, 0x10);
  if (!(F & FZ)) { CYCT(b_+47, b_+48); ret_effect(gb); return; }
  CYC(b_+47, b_+48);
  CYC(b_+48, b_+51);
  get_link_walking_animation(gb);
  CYC(b_+51, b_+52); alu_add(gb, B);
  CYC(b_+52, b_+53); B = A;
  CYC(b_+53, b_+54);
  ret_effect(gb);
}
