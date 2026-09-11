#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x06, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x06, (from), (to), true)

void linkApplyDamage_b06_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x46bb, 0x46bc); H = D;
  CYC(0x46bc, 0x46be); L = 0x25;
  CYC(0x46be, 0x46bf); A = mem_rd(gb, HL);
  CYC(0x46bf, 0x46c1); mem_wr(gb, HL, 0);
  CYC(0x46c1, 0x46c2); alu_or(gb, A);
  if (F & FZ) { CYCT(0x46c2, 0x46c4); goto damage_knockback; }
  CYC(0x46c2, 0x46c4);
  CYC(0x46c4, 0x46c5); B = A;
  CYC(0x46c5, 0x46c7); A = 0x3f;
  CALL_C(0x46c7, cpActiveRing_hook, 0x23b0, 0x46ca);
  if (!(F & FZ)) { CYCT(0x46ca, 0x46cc); goto apply_damage; }
  CYC(0x46ca, 0x46cc);
  CYC(0x46cc, 0x46ce); B = 0xf8;
apply_damage:
  CYC(0x46ce, 0x46d0); L = 0x29;
  CYC(0x46d0, 0x46d1); A = mem_rd(gb, HL);
  CYC(0x46d1, 0x46d2); alu_add(gb, B);
  CYC(0x46d2, 0x46d3); mem_wr(gb, HL, A);
damage_knockback:
  CYC(0x46d3, 0x46d5); L = 0x2a;
  CYC(0x46d5, 0x46d6); A = mem_rd(gb, HL);
  CYC(0x46d6, 0x46d7); alu_or(gb, A);
  if (F & FZ) { CYCT(0x46d7, 0x46d9); goto normalize_health; }
  CYC(0x46d7, 0x46d9);
  CYC(0x46d9, 0x46db); A = 0x10;
  CALL_C(0x46db, cpActiveRing_hook, 0x23b0, 0x46de);
  if (!(F & FZ)) { CYCT(0x46de, 0x46e0); goto normalize_health; }
  CYC(0x46de, 0x46e0);
  CYC(0x46e0, 0x46e2); L = 0x2d;
  CYC(0x46e2, 0x46e4); mem_wr(gb, HL, alu_srl(gb, mem_rd(gb, HL)));
normalize_health:
  CYC(0x46e4, 0x46e7); SET_HL(0xc6aa);
  CYC(0x46e7, 0x46e9); E = 0x29;
  CYC(0x46e9, 0x46ea); A = mem_rd(gb, DE);
  CYC(0x46ea, 0x46ec); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(0x46ec, 0x46ee); goto potion; }
  CYC(0x46ec, 0x46ee);
  CYC(0x46ee, 0x46ef); A = mem_rd(gb, DE);
normalize_loop:
  CYC(0x46ef, 0x46f0); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x46f0, 0x46f2); alu_add(gb, 2);
  if (!(F & FC)) { CYCT(0x46f2, 0x46f4); goto normalize_loop; }
  CYC(0x46f2, 0x46f4);
  CYC(0x46f4, 0x46f5); mem_wr(gb, DE, A);
potion:
  CYC(0x46f5, 0x46f6); A = mem_rd(gb, HL);
  CYC(0x46f6, 0x46f7); A = alu_dec8(gb, A);
  CYC(0x46f7, 0x46f8); alu_rlca(gb);
  if (!(F & FC)) { CYCT(0x46f8, 0x46fa); goto finish; }
  CYC(0x46f8, 0x46fa);
  CYC(0x46fa, 0x46fc); A = 0x2f;
  CALL_C(0x46fc, checkTreasureObtained_hook, 0x1748, 0x46ff);
  if (!(F & FC)) { CYCT(0x46ff, 0x4701); goto no_potion; }
  CYC(0x46ff, 0x4701);
  CYC(0x4701, 0x4704); SET_HL(0xc6ab);
  CYC(0x4704, 0x4705); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x4705, 0x4706); mem_wr(gb, HL, A);
  CYC(0x4706, 0x4708); A = 1;
  CYC(0x4708, 0x4709); mem_wr(gb, DE, A);
  CYC(0x4709, 0x470b); A = 0x2f;
  CALL_C(0x470b, loseTreasure_hook, 0x1733, 0x470e);
  CYC(0x470e, 0x4710); goto finish;
no_potion:
  CYC(0x4710, 0x4711); alu_xor(gb, A);
  CYC(0x4711, 0x4712); mem_wr(gb, DE, A);
  CYC(0x4712, 0x4713); mem_wr(gb, HL, A);
  CYC(0x4713, 0x4716); mem_wr(gb, 0xcc6f, A);
  CYC(0x4716, 0x4718); E = 0x04;
  CYC(0x4718, 0x4719); A = mem_rd(gb, DE);
  CYC(0x4719, 0x471b); alu_cp(gb, 0x0d);
  if (F & FZ) { CYCT(0x471b, 0x471d); goto finish; }
  CYC(0x471b, 0x471d);
  CYC(0x471d, 0x471f); A = 0xff;
  CYC(0x471f, 0x4722); mem_wr(gb, 0xcdd5, A);
  CALL_C(0x4722, clearAllParentItems_hook, 0x2c10, 0x4725);
finish:
  CYC(0x4725, 0x4728); A = mem_rd(gb, 0xcc00);
  CYC(0x4728, 0x4729); alu_rrca(gb);
  if (!(F & FC)) { CYCT(0x4729, 0x472b); goto end; }
  CYC(0x4729, 0x472b);
  CYC(0x472b, 0x472d); E = 0x2e;
  CYC(0x472d, 0x472e); A = mem_rd(gb, DE);
  CYC(0x472e, 0x472f); alu_or(gb, A);
  if (F & FZ) { CYCT(0x472f, 0x4731); goto end; }
  CYC(0x472f, 0x4731);
  CYC(0x4731, 0x4732); A = alu_dec8(gb, A);
  CYC(0x4732, 0x4733); mem_wr(gb, DE, A);
end:
  CYC(0x4733, 0x4734); ret_effect(gb);
}

static void add_a_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
    pop_effect(gb);
    return;
  }
  burn_rom(gb, 0x00, 0x0012, 0x0013, false);
  burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
  burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  pop_effect(gb);
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
  pop_effect(gb);
}

void load_animation_frame_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x44f4, 0x44f6); L = 0x32;
  CYC(0x44f6, 0x44f7); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x44f7, 0x44f8); ret_effect(gb); return; }
  CYC(0x44f7, 0x44f8);
  CYC(0x44f8, 0x44f9); mem_wr(gb, HL, A);
  CALL_C(0x44f9, getSpecialObjectGraphicsFrame_hook, 0x4516, 0x44fc);
  if (F & FZ) { CYCT(0x44fc, 0x44fd); ret_effect(gb); return; }
  CYC(0x44fc, 0x44fd);
  CYC(0x44fd, 0x44ff); E = 0x01;
  CYC(0x44ff, 0x4500); A = mem_rd(gb, DE);
  CYC(0x4500, 0x4502); alu_cp(gb, 0x0a);
  CYC(0x4502, 0x4505); SET_DE(0x8701);
  if (F & FC) { CYCT(0x4505, 0x4507); goto queue; }
  CYC(0x4505, 0x4507);
  CYC(0x4507, 0x4509); D = 0x86;
queue:
  CYC(0x4509, 0x450c);
  queueDmaTransfer_hook(gb);
}

void loadLinkAndCompanionAnimationFrame_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x44c9, 0x44cb); A = 0xff;
  CYC(0x44cb, 0x44ce); mem_wr(gb, 0xcc65, A);
  CYC(0x44ce, 0x44d1); A = mem_rd(gb, 0xd01a);
  CYC(0x44d1, 0x44d2); alu_rlca(gb);
  if (!(F & FC)) { CYCT(0x44d2, 0x44d4); goto companion; }
  CYC(0x44d2, 0x44d4);
  CALL_C(0x44d4, func_4553_hook, 0x4553, 0x44d7);
  CYC(0x44d7, 0x44da); A = mem_rd(gb, 0xd001);
  CYC(0x44da, 0x44dd); SET_HL(0x450c);
  CYC(0x44dd, 0x44de); add_a_to_hl(gb, 0x44de);
  CYC(0x44de, 0x44df); A = B;
  CYC(0x44df, 0x44e0); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(0x44e0, 0x44e2); goto load; }
  CYC(0x44e0, 0x44e2);
  CYC(0x44e2, 0x44e5); A = mem_rd(gb, 0xd008);
  CYC(0x44e5, 0x44e6); alu_add(gb, B);
load:
  CYC(0x44e6, 0x44e8); H = 0xd0;
  CYC(0x44e8, 0x44eb); push_effect(gb, 0x44eb);
  load_animation_frame_hook(gb);
companion:
  CYC(0x44eb, 0x44ee); SET_HL(0xd11a);
  CYC(0x44ee, 0x44f0); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x44f0, 0x44f1); ret_effect(gb); return; }
  CYC(0x44f0, 0x44f1);
  CYC(0x44f1, 0x44f3); L = 0x31;
  CYC(0x44f3, 0x44f4); A = mem_rd(gb, HL);
  load_animation_frame_hook(gb);
}

void getSpecialObjectGraphicsFrame_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4516, 0x4517); C = A;
  CYC(0x4517, 0x4519); B = 0;
  CYC(0x4519, 0x451a); D = H;
  CYC(0x451a, 0x451c); L = 0x01;
  CYC(0x451c, 0x451d); A = mem_rd(gb, HL);
  CYC(0x451d, 0x451e); E = A;
  CYC(0x451e, 0x4521); SET_HL(0x4451);
  CYC(0x4521, 0x4522); add_double_index_to_hl(gb, 0x4522);
  CYC(0x4522, 0x4523); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4523, 0x4524); H = mem_rd(gb, HL);
  CYC(0x4524, 0x4525); L = A;
  CYC(0x4525, 0x4526); alu_add_hl(gb, BC);
  CYC(0x4526, 0x4527); alu_add_hl(gb, BC);
  CYC(0x4527, 0x4528); alu_add_hl(gb, BC);
  CYC(0x4528, 0x4529); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4529, 0x452a); push_effect(gb, HL);
  CYC(0x452a, 0x452b); alu_add(gb, A);
  CYC(0x452b, 0x452c); C = A;
  CYC(0x452c, 0x452d); A = E;
  CYC(0x452d, 0x4530); SET_HL(0x44a1);
  CYC(0x4530, 0x4531); add_double_index_to_hl(gb, 0x4531);
  CYC(0x4531, 0x4532); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4532, 0x4533); H = mem_rd(gb, HL);
  CYC(0x4533, 0x4534); L = A;
  CYC(0x4534, 0x4535); alu_add_hl(gb, BC);
  CYC(0x4535, 0x4537); E = 0x1e;
  CYC(0x4537, 0x4538); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4538, 0x4539); mem_wr(gb, DE, A);
  CYC(0x4539, 0x453a); E = alu_inc8(gb, E);
  CYC(0x453a, 0x453b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x453b, 0x453d); alu_and(gb, 0x3f);
  CYC(0x453d, 0x453e); mem_wr(gb, DE, A);
  CYC(0x453e, 0x453f); SET_HL(pop_effect(gb));
  CYC(0x453f, 0x4540); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4540, 0x4541); H = mem_rd(gb, HL);
  CYC(0x4541, 0x4542); L = A;
  CYC(0x4542, 0x4543); alu_or(gb, H);
  if (F & FZ) { CYCT(0x4543, 0x4544); ret_effect(gb); return; }
  CYC(0x4543, 0x4544);
  CYC(0x4544, 0x4545); A = L;
  CYC(0x4545, 0x4547); alu_and(gb, 0x01);
  CYC(0x4547, 0x4549); alu_add(gb, 0x1a);
  CYC(0x4549, 0x454a); C = A;
  CYC(0x454a, 0x454b); A = L;
  CYC(0x454b, 0x454d); alu_and(gb, 0x1e);
  CYC(0x454d, 0x454e); A = alu_dec8(gb, A);
  CYC(0x454e, 0x454f); B = A;
  CYC(0x454f, 0x4551); L &= 0xef;
  CYC(0x4551, 0x4552); alu_or(gb, D);
  CYC(0x4552, 0x4553);
  ret_effect(gb);
}

void specialObjectSetAnimationWithLinkData_hook(GB *gb) {
  CYC(0x4412, 0x4414); E = 0x30;
  CYC(0x4414, 0x4415); mem_wr(gb, DE, A);
  CYC(0x4415, 0x4416); alu_add(gb, A);
  CYC(0x4416, 0x4417); C = A;
  CYC(0x4417, 0x4419); B = 0;
  CYC(0x4419, 0x441c); A = mem_rd(gb, 0xd001);
  CYC(0x441c, 0x441e);
  label_06_032(gb);
}

void specialObjectLoadAnimationFrameToBuffer_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x79dc, 0x79df); SET_HL(0xd11a);
  CYC(0x79df, 0x79e1); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x79e1, 0x79e2); ret_effect(gb); return; }
  CYC(0x79e1, 0x79e2);
  CYC(0x79e2, 0x79e4); L = 0x32;
  CYC(0x79e4, 0x79e5); A = mem_rd(gb, HL);
  CALL_C(0x79e5, getSpecialObjectGraphicsFrame_hook, 0x4516, 0x79e8);
  if (F & FZ) { CYCT(0x79e8, 0x79e9); ret_effect(gb); return; }
  CYC(0x79e8, 0x79e9);
  CYC(0x79e9, 0x79ea); A = L;
  CYC(0x79ea, 0x79ec); alu_and(gb, 0xf0);
  CYC(0x79ec, 0x79ed); L = A;
  CYC(0x79ed, 0x79f0); SET_DE(0xd606);
  CYC(0x79f0, 0x79f3);
  copy256BytesFromBank_hook(gb);
}

void specialObjectNextAnimationFrame_hook(GB *gb) {
  CYC(0x4432, 0x4433); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4433, 0x4434); H = mem_rd(gb, HL);
  CYC(0x4434, 0x4435); L = A;
  CYC(0x4435, 0x4436); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4436, 0x4438); alu_cp(gb, 0xff);
  if (!(F & FZ)) { CYCT(0x4438, 0x443a); goto write; }
  CYC(0x4438, 0x443a);
  CYC(0x443a, 0x443b); C = mem_rd(gb, HL);
  CYC(0x443b, 0x443c); B = A;
  CYC(0x443c, 0x443d); alu_add_hl(gb, BC);
  CYC(0x443d, 0x443e); A = mem_rd(gb, HL); SET_HL(HL + 1);
write:
  CYC(0x443e, 0x4440); E = 0x20;
  CYC(0x4440, 0x4441); mem_wr(gb, DE, A);
  CYC(0x4441, 0x4442); E = alu_inc8(gb, E);
  CYC(0x4442, 0x4443); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4443, 0x4444); C = A;
  CYC(0x4444, 0x4445); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4445, 0x4446); mem_wr(gb, DE, A);
  CYC(0x4446, 0x4447); E = alu_inc8(gb, E);
  CYC(0x4447, 0x4448); A = L;
  CYC(0x4448, 0x4449); mem_wr(gb, DE, A);
  CYC(0x4449, 0x444a); E = alu_inc8(gb, E);
  CYC(0x444a, 0x444b); A = H;
  CYC(0x444b, 0x444c); mem_wr(gb, DE, A);
  CYC(0x444c, 0x444e); E = 0x31;
  CYC(0x444e, 0x444f); A = C;
  CYC(0x444f, 0x4450); mem_wr(gb, DE, A);
  CYC(0x4450, 0x4451);
  ret_effect(gb);
}

void specialObjectSetAnimation_data_hook(GB *gb) {
  CYC(0x442a, 0x442d); SET_HL(0x4479);
  CYC(0x442d, 0x442e); add_double_index_to_hl(gb, 0x442e);
  CYC(0x442e, 0x442f); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x442f, 0x4430); H = mem_rd(gb, HL);
  CYC(0x4430, 0x4431); L = A;
  CYC(0x4431, 0x4432); alu_add_hl(gb, BC);
  specialObjectNextAnimationFrame_hook(gb);
}

void specialObjectSetAnimation_body_hook(GB *gb) {
  CYC(0x4427, 0x4429); E = 0x01;
  CYC(0x4429, 0x442a); A = mem_rd(gb, DE);
  specialObjectSetAnimation_data_hook(gb);
}

void specialObjectAnimate_optimized_hook(GB *gb) {
  CYC(0x441e, 0x441f); H = D;
  CYC(0x441f, 0x4421); L = 0x20;
  CYC(0x4421, 0x4422); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x4422, 0x4423); ret_effect(gb); return; }
  CYC(0x4422, 0x4423);
  CYC(0x4423, 0x4425); L = 0x22;
  CYC(0x4425, 0x4427);
  specialObjectNextAnimationFrame_hook(gb);
}

void fake_specialObjectLoadAnimationFrameToBuffer_hook(GB *gb) {
  CYC(0x7a07, 0x7a0a); SET_HL(0xd11a);
  CYC(0x7a0a, 0x7a0c); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x7a0c, 0x7a0d); ret_effect(gb); return; }
  CYC(0x7a0c, 0x7a0d);
  CYC(0x7a0d, 0x7a0f); L = 0x32;
  CYC(0x7a0f, 0x7a10); A = mem_rd(gb, HL);
  CALL_ROM(0x7a10, 0x4524);
  if (F & FZ) { CYCT(0x7a13, 0x7a14); ret_effect(gb); return; }
  CYC(0x7a13, 0x7a14);
  CYC(0x7a14, 0x7a15); A = L;
  CYC(0x7a15, 0x7a17); alu_and(gb, 0xf0);
  CYC(0x7a17, 0x7a18); L = A;
  CYC(0x7a18, 0x7a1b); SET_DE(0xd606);
  CYC(0x7a1b, 0x7a1e);
  hook_handoff(gb, 0x3f17);
}

void linkUpdateDamageToApplyForRings_hook(GB *gb) {
  CYC(0x4668, 0x466a); E = 0x25;
  CYC(0x466a, 0x466b); A = mem_rd(gb, DE);
  CYC(0x466b, 0x466c); alu_or(gb, A);
  if (F & FZ) { CYCT(0x466c, 0x466d); ret_effect(gb); return; }
  CYC(0x466c, 0x466d);
  CYC(0x466d, 0x466e); B = A;
  CYC(0x466e, 0x4671); SET_HL(0x46ae);
  CYC(0x4671, 0x4674); A = mem_rd(gb, 0xc6cb);
  CYC(0x4674, 0x4675); E = A;
  for (;;) {
    CYC(0x4675, 0x4676); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x4676, 0x4677); alu_or(gb, A);
    if (F & FZ) { CYCT(0x4677, 0x4679); goto no_match; }
    CYC(0x4677, 0x4679);
    CYC(0x4679, 0x467a); alu_cp(gb, E);
    if (F & FZ) { CYCT(0x467a, 0x467c); goto matching; }
    CYC(0x467a, 0x467c);
    CYC(0x467c, 0x467d); SET_HL(HL + 1);
    CYC(0x467d, 0x467f);
  }
no_match:
  CYC(0x467f, 0x4680); A = E;
  CYC(0x4680, 0x4682); alu_cp(gb, 0x08);
  if (F & FZ) { CYCT(0x4682, 0x4684); goto blue; }
  CYC(0x4682, 0x4684);
  CYC(0x4684, 0x4686); alu_cp(gb, 0x09);
  if (F & FZ) { CYCT(0x4686, 0x4688); goto green; }
  CYC(0x4686, 0x4688);
  CYC(0x4688, 0x468a); alu_cp(gb, 0x0a);
  if (!(F & FZ)) { CYCT(0x468a, 0x468b); ret_effect(gb); return; }
  CYC(0x468a, 0x468b);
  CYC(0x468b, 0x468c); A = B;
  CYC(0x468c, 0x468d); alu_add(gb, A);
  CYC(0x468d, 0x468f); goto write_damage;
blue:
  CYC(0x468f, 0x4690); A = B;
  CYC(0x4690, 0x4692); alu_sra(gb, A);
  CYC(0x4692, 0x4694); goto write_damage;
green:
  CYC(0x4694, 0x4695); A = B;
  CYC(0x4695, 0x4696); A = (uint8_t)~A;
  CYC(0x4696, 0x4697); A = alu_inc8(gb, A);
  CYC(0x4697, 0x4698); alu_add(gb, A);
  CYC(0x4698, 0x4699); alu_add(gb, A);
  CYC(0x4699, 0x469a); alu_add(gb, B);
  CYC(0x469a, 0x469c); alu_sra(gb, A);
  CYC(0x469c, 0x469e); alu_sra(gb, A);
  CYC(0x469e, 0x469f); A = (uint8_t)~A;
  CYC(0x469f, 0x46a0); A = alu_inc8(gb, A);
  CYC(0x46a0, 0x46a2); goto write_damage;
matching:
  CYC(0x46a2, 0x46a3); A = mem_rd(gb, HL);
  CYC(0x46a3, 0x46a4); alu_add(gb, B);
write_damage:
  CYC(0x46a4, 0x46a6); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x46a6, 0x46a8); goto store; }
  CYC(0x46a6, 0x46a8);
  CYC(0x46a8, 0x46aa); A = 0xff;
store:
  CYC(0x46aa, 0x46ac); E = 0x25;
  CYC(0x46ac, 0x46ad); mem_wr(gb, DE, A);
  CYC(0x46ad, 0x46ae);
  ret_effect(gb);
}

void getTransformedLinkID_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x462d, 0x4630); SET_HL(0xcc56);
  CYC(0x4630, 0x4631); A = mem_rd(gb, HL);
  CYC(0x4631, 0x4632); alu_or(gb, A);
  if (F & FZ) CYCT(0x4632, 0x4634);
  else {
    CYC(0x4632, 0x4634);
    CYC(0x4634, 0x4635); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    CYC(0x4635, 0x4637);
    goto zero;
  }
  CYC(0x4637, 0x463a); A = mem_rd(gb, 0xcc34);
  CYC(0x463a, 0x463c); alu_and(gb, 0x60);
  if (!(F & FZ)) { CYCT(0x463c, 0x463e); goto zero; }
  CYC(0x463c, 0x463e);
  CYC(0x463e, 0x4641); A = mem_rd(gb, 0xcc02);
  CYC(0x4641, 0x4642); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4642, 0x4644); goto zero; }
  CYC(0x4642, 0x4644);
  CYC(0x4644, 0x4647); A = mem_rd(gb, 0xccd3);
  CYC(0x4647, 0x4648); B = A;
  CYC(0x4648, 0x464b); A = mem_rd(gb, 0xcc5a);
  CYC(0x464b, 0x464c); alu_or(gb, B);
  if (!(F & FZ)) { CYCT(0x464c, 0x464e); goto zero; }
  CYC(0x464c, 0x464e);
  CYC(0x464e, 0x4651); A = mem_rd(gb, 0xc6cb);
  CYC(0x4651, 0x4652); E = A;
  CYC(0x4652, 0x4655); SET_HL(0x465d);
  CALL_C(0x4655, lookupKey_hook, 0x1e06, 0x4658);
  CYC(0x4658, 0x4659); B = A;
  CYC(0x4659, 0x465a);
  ret_effect(gb);
  return;
zero:
  CYC(0x465a, 0x465c); B = 0;
  CYC(0x465c, 0x465d);
  ret_effect(gb);
}

void func_4553_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4553, 0x4556); A = mem_rd(gb, 0xd001);
  CYC(0x4556, 0x4557); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4557, 0x4559); goto scan; }
  CYC(0x4557, 0x4559);
  CYC(0x4559, 0x455c); A = mem_rd(gb, 0xd031);
  CYC(0x455c, 0x455d); B = A;
  CYC(0x455d, 0x455e);
  ret_effect(gb);
  return;
scan:
  CYC(0x455e, 0x4561); SET_HL(0xd200);
  CYC(0x4561, 0x4564); SET_BC(0);
  do {
    CYC(0x4564, 0x4566); L = 0x3f;
    CYC(0x4566, 0x4567); A = mem_rd(gb, HL);
    CYC(0x4567, 0x4568); alu_cp(gb, C);
    if (F & FC) { CYCT(0x4568, 0x456a); }
    else {
      CYC(0x4568, 0x456a);
      CYC(0x456a, 0x456b); C = A;
      CYC(0x456b, 0x456d); L = 0x31;
      CYC(0x456d, 0x456e); B = mem_rd(gb, HL);
    }
    CYC(0x456e, 0x456f); H = alu_inc8(gb, H);
    CYC(0x456f, 0x4570); A = H;
    CYC(0x4570, 0x4572); alu_cp(gb, 0xd6);
    if (F & FC) { CYCT(0x4572, 0x4574); continue; }
    CYC(0x4572, 0x4574);
    break;
  } while (true);
  CYC(0x4574, 0x4577); A = mem_rd(gb, 0xd03f);
  CYC(0x4577, 0x4578); alu_cp(gb, C);
  if (F & FC) { CYCT(0x4578, 0x4579); ret_effect(gb); return; }
  CYC(0x4578, 0x4579);
  CYC(0x4579, 0x457c); A = mem_rd(gb, 0xd031);
  CYC(0x457c, 0x457d); B = A;
  CYC(0x457d, 0x4580); A = mem_rd(gb, 0xd030);
  CYC(0x4580, 0x4582); alu_cp(gb, 0x10);
  if (!(F & FZ)) { CYCT(0x4582, 0x4583); ret_effect(gb); return; }
  CYC(0x4582, 0x4583);
  CALL_ROM(0x4583, 0x4589);
  CYC(0x4586, 0x4587); alu_add(gb, B);
  CYC(0x4587, 0x4588); B = A;
  CYC(0x4588, 0x4589);
  ret_effect(gb);
}
