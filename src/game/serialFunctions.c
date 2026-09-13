#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x16, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x16, (from), (to), true)

#define ackPacket_bank16 0x44fd

void func_4000_b16_hook(GB *gb);
void func_4036_hook(GB *gb);
void FFBE_04_hook(GB *gb);
void FFBE_03_hook(GB *gb);
void func_4043_hook(GB *gb);
void sendPacketByte_hook(GB *gb);
void func_4087_hook(GB *gb);
void func_4096_hook(GB *gb);
void disableSerialIfByteReceived_hook(GB *gb);
void receivePacketByte_hook(GB *gb);
void waitForSerialByte_hook(GB *gb);
void setLinkTimerTo180_hook(GB *gb);
void FFBE_00_hook(GB *gb);
void FFBE_02_hook(GB *gb);
void prepareForNextPacket_hook(GB *gb);
void func_426e_hook(GB *gb);
void waitForNextPacket_hook(GB *gb);
void func_4293_hook(GB *gb);
void sendAckPacket_hook(GB *gb);
void returnIfPacketNotComplete_hook(GB *gb);

static uint16_t serial_jump_table(GB *gb) {
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

static void serial_add_a_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb); return;
  }
  burn_rom(gb, 0x00, 0x0012, 0x0013, false);
  burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
  burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
}

void func_4000_b16_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4000, 0x4002); A = H8(hSerialInterruptBehaviour);
  CYC(0x4002, 0x4003); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4003, 0x4004); ret_effect(gb); return; }
  CYC(0x4003, 0x4004);
  CYC(0x4004, 0x4006); A = mem_rd(gb, IO_SVBK);
  CYC(0x4006, 0x4007); push_effect(gb, AF);
  CYC(0x4007, 0x4009); A = 0x04;
  CYC(0x4009, 0x400b); mem_wr(gb, IO_SVBK, A);
  CYC(0x400b, 0x400c); push_effect(gb, DE);
  CALL_C(0x400c, func_4036_hook, 0x4036, 0x400f);
  CYC(0x400f, 0x4010); SET_DE(pop_effect(gb));
  CYC(0x4010, 0x4012); A = mem_rd(gb, IO_SC);
  CYC(0x4012, 0x4013); alu_rlca(gb);
  if (F & FC) { CYCT(0x4013, 0x4015); goto restore_wram_bank; }
  CYC(0x4013, 0x4015);
  CYC(0x4015, 0x4017); A = H8(hSerialInterruptBehaviour);
  CYC(0x4017, 0x4019); alu_cp(gb, 0xe0);
  if (F & FZ) { CYCT(0x4019, 0x401b); goto write_serial_control; }
  CYC(0x4019, 0x401b);
  CYC(0x401b, 0x401e); A = W8(w4d98b);
  CYC(0x401e, 0x401f); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x401f, 0x4021); goto restore_wram_bank; }
  CYC(0x401f, 0x4021);
  CYC(0x4021, 0x4024); A = W8(w4d983);
  CYC(0x4024, 0x4026); alu_xor(gb, 0x01);
  CYC(0x4026, 0x4029); W8(w4d983) = A;
  if (F & FZ) { CYCT(0x4029, 0x402b); goto restore_wram_bank; }
  CYC(0x4029, 0x402b);
  CYC(0x402b, 0x402d); A = H8(hSerialInterruptBehaviour);

write_serial_control:
  CYC(0x402d, 0x402f); alu_and(gb, 0x81);
  CALL_C(0x402f, writeToSC_hook, 0x0c6a, 0x4032);

restore_wram_bank:
  CYC(0x4032, 0x4033); SET_AF(pop_effect(gb));
  CYC(0x4033, 0x4035); mem_wr(gb, IO_SVBK, A);
  CYC(0x4035, 0x4036); ret_effect(gb);
}

void func_4036_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4036, 0x4038); A = H8(hFFBE);
  CYC(0x4038, 0x4039); push_effect(gb, 0x4039);
  switch (serial_jump_table(gb)) {
    case 0x420d: FFBE_00_hook(gb); return;
    case 0x421e: FFBE_02_hook(gb); return;
    case 0x4143: FFBE_03_hook(gb); return;
    case 0x410e: FFBE_04_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void func_4043_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4043, waitForSerialByte_hook, 0x41dc, 0x4046);
  CYC(0x4046, 0x4048); alu_cp(gb, 0x80);
  if (F & FZ) { CYCT(0x4048, 0x4049); ret_effect(gb); return; }
  CYC(0x4048, 0x4049);
  sendPacketByte_hook(gb);
}

void sendPacketByte_hook(GB *gb) {
  CYC(0x4049, 0x404c); A = W8(w4PacketByteIndex);
  CYC(0x404c, 0x404f); SET_HL(w4PacketBuffer);
  CYC(0x404f, 0x4050); serial_add_a_to_hl(gb, 0x4050);
  CYC(0x4050, 0x4053); A = W8(w4PacketByteIndex);
  CYC(0x4053, 0x4054); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4054, 0x4056);
    goto next_byte;
  }
  CYC(0x4054, 0x4056);
  CYC(0x4056, 0x4057); A = mem_rd(gb, HL);
  CYC(0x4057, 0x4058); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4058, 0x405a);
    goto get_num_bytes;
  }
  CYC(0x4058, 0x405a);
  CYC(0x405a, 0x405b); A = alu_inc8(gb, A);
  CYC(0x405b, 0x405e); W8(w4d98b) = A;
  CYC(0x405e, 0x405f); ret_effect(gb); return;

get_num_bytes:
  CYC(0x405f, 0x4062); W8(w4NumPacketBytes) = A;
  CYC(0x4062, 0x4063); alu_xor(gb, A);
  CYC(0x4063, 0x4066); W8(w4PacketChecksum) = A;

next_byte:
  CYC(0x4066, 0x4067); A = alu_inc8(gb, A);
  CYC(0x4067, 0x406a); W8(w4PacketByteIndex) = A;
  CYC(0x406a, 0x406d); A = W8(w4NumPacketBytes);
  CYC(0x406d, 0x406e); A = alu_dec8(gb, A);
  CYC(0x406e, 0x4071); W8(w4NumPacketBytes) = A;
  CYC(0x4071, 0x4072); A = mem_rd(gb, HL); SET_HL(HL + 1);
  if (!(F & FZ)) {
    CYCT(0x4072, 0x4074);
  } else {
    CYC(0x4072, 0x4074);
    CYC(0x4074, 0x4075); alu_xor(gb, A);
    CYC(0x4075, 0x4078); W8(w4WaitingForNextByte) = A;
    CYC(0x4078, 0x407b); A = W8(w4PacketChecksum);
  }
  CYC(0x407b, 0x407d); mem_wr(gb, IO_SB, A);
  CYC(0x407d, 0x4080); SET_HL(w4PacketChecksum);
  CYC(0x4080, 0x4081); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4081, 0x4082); mem_wr(gb, HL, A);
  CYC(0x4082, 0x4083); alu_xor(gb, A);
  CYC(0x4083, 0x4086); W8(w4d98b) = A;
  CYC(0x4086, 0x4087); ret_effect(gb);
}

void func_4087_hook(GB *gb) {
  CYC(0x4087, 0x4089); A = H8(hReceivedSerialByte);
  CYC(0x4089, 0x408a); alu_or(gb, A);
  if (F & FZ) { CYCT(0x408a, 0x408b); ret_effect(gb); return; }
  CYC(0x408a, 0x408b);
  CYC(0x408b, 0x408d); A = 0x01;
  CYC(0x408d, 0x4090); W8(w4d98b) = A;
  CYC(0x4090, 0x4091); alu_xor(gb, A);
  CYC(0x4091, 0x4093); mem_wr(gb, IO_SB, A);
  CYC(0x4093, 0x4095); H8(hReceivedSerialByte) = A;
  CYC(0x4095, 0x4096); ret_effect(gb);
}

void func_4096_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4096, waitForSerialByte_hook, 0x41dc, 0x4099);
  CYC(0x4099, 0x409b); alu_cp(gb, 0x80);
  if (F & FZ) {
    CYCT(0x409b, 0x409e);
    disableSerialPort_hook(gb);
    return;
  }
  CYC(0x409b, 0x409e);
  CYC(0x409e, 0x40a1);
  prepareForNextPacket_hook(gb);
}

void disableSerialIfByteReceived_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x40a1, waitForSerialByte_hook, 0x41dc, 0x40a4);
  CYC(0x40a4, 0x40a7);
  disableSerialPort_hook(gb);
}

void receivePacketByte_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x40a7, 0x40a8); alu_xor(gb, A);
  CYC(0x40a8, 0x40ab); W8(w4d98b) = A;
  CALL_C(0x40ab, waitForSerialByte_hook, 0x41dc, 0x40ae);
  CYC(0x40ae, 0x40b0); alu_cp(gb, 0x80);
  if (F & FZ) { CYCT(0x40b0, 0x40b1); ret_effect(gb); return; }
  CYC(0x40b0, 0x40b1);
  CYC(0x40b1, 0x40b4); A = W8(w4PacketByteIndex);
  CYC(0x40b4, 0x40b5); B = A;
  CYC(0x40b5, 0x40b6); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x40b6, 0x40b8);
    goto got_packet_byte;
  }
  CYC(0x40b6, 0x40b8);
  CYC(0x40b8, 0x40ba); A = H8(hSerialByte);
  CYC(0x40ba, 0x40bc); alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(0x40bc, 0x40be);
    goto invalid_length;
  }
  CYC(0x40bc, 0x40be);
  CYC(0x40be, 0x40bf); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x40bf, 0x40c1);
    goto got_packet_length;
  }
  CYC(0x40bf, 0x40c1);

invalid_length:
  CYC(0x40c1, 0x40c4); A = W8(w4DisableLinkTimeout);
  CYC(0x40c4, 0x40c5); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x40c5, 0x40c6); ret_effect(gb); return; }
  CYC(0x40c5, 0x40c6);
  CYC(0x40c6, 0x40c9); SET_HL(w4d984);
  CYC(0x40c9, 0x40ca); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x40ca, 0x40cb); ret_effect(gb); return; }
  CYC(0x40ca, 0x40cb);
  CYC(0x40cb, 0x40cd); A = 0x86;
  CYC(0x40cd, 0x40cf); H8(hFFBD) = A;
  CYC(0x40cf, 0x40d0); alu_xor(gb, A);
  CYC(0x40d0, 0x40d3); W8(w4WaitingForNextByte) = A;
  CYC(0x40d3, 0x40d4); ret_effect(gb); return;

got_packet_length:
  CYC(0x40d4, 0x40d7); W8(w4NumPacketBytes) = A;

got_packet_byte:
  CYC(0x40d7, 0x40da); SET_HL(w4NumPacketBytes);
  CYC(0x40da, 0x40db); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x40db, 0x40dd);
    goto get_next_byte;
  }
  CYC(0x40db, 0x40dd);
  CYC(0x40dd, 0x40df); A = H8(hSerialByte);
  CYC(0x40df, 0x40e2); SET_HL(w4PacketChecksum);
  CYC(0x40e2, 0x40e3); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x40e3, 0x40e5);
  } else {
    CYC(0x40e3, 0x40e5);
    CYC(0x40e5, 0x40e7); A = 0x81;
    CYC(0x40e7, 0x40e9); H8(hFFBD) = A;
  }
  CYC(0x40e9, 0x40ea); alu_xor(gb, A);
  CYC(0x40ea, 0x40ed); W8(w4WaitingForNextByte) = A;
  CYC(0x40ed, 0x40f0); W8(w4d984) = A;
  CYC(0x40f0, 0x40f2); mem_wr(gb, IO_SB, A);
  CYC(0x40f2, 0x40f3); ret_effect(gb); return;

get_next_byte:
  CYC(0x40f3, 0x40f4); A = B;
  CYC(0x40f4, 0x40f7); SET_DE(w4PacketBuffer);
  CALL_C(0x40f7, addAToDe_hook, 0x0068, 0x40fa);
  CYC(0x40fa, 0x40fb); A = B;
  CYC(0x40fb, 0x40fc); A = alu_inc8(gb, A);
  CYC(0x40fc, 0x40ff); W8(w4PacketByteIndex) = A;
  CYC(0x40ff, 0x4101); A = H8(hSerialByte);
  CYC(0x4101, 0x4102); mem_wr(gb, DE, A);
  CYC(0x4102, 0x4105); SET_HL(w4PacketChecksum);
  CYC(0x4105, 0x4106); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4106, 0x4107); mem_wr(gb, HL, A);
  CYC(0x4107, 0x4108); alu_xor(gb, A);
  CYC(0x4108, 0x410a); mem_wr(gb, IO_SB, A);
  CYC(0x410a, 0x410d); W8(w4d984) = A;
  CYC(0x410d, 0x410e); ret_effect(gb);
}

void FFBE_04_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x410e, 0x4110); A = H8(hSerialLinkState);
  CYC(0x4110, 0x4111); push_effect(gb, 0x4111);
  switch (serial_jump_table(gb)) {
    case 0x440d: gameLink_getFile1(gb); return;
    case 0x4280: waitForNextPacket_hook(gb); return;
    case 0x4411: gameLink_getFile2(gb); return;
    case 0x4415: gameLink_getFile3(gb); return;
    case 0x438e: func_438e(gb); return;
    case 0x4087: func_4087_hook(gb); return;
    case 0x42ee: gameLinkState08(gb); return;
    case 0x4305: gameLinkState09(gb); return;
    case 0x4370: gameLinkState0b(gb); return;
    case 0x43c5: gameLinkState0c(gb); return;
    case 0x4329: gameLinkState0f(gb); return;
    case 0x4096: func_4096_hook(gb); return;
    case 0x434d: gameLinkState12(gb); return;
    case 0x43f5: sendAckPacket_hook(gb); return;
    case 0x437b: func_437b(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void FFBE_03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4143, 0x4145); A = H8(hSerialLinkState);
  CYC(0x4145, 0x4146); push_effect(gb, 0x4146);
  switch (serial_jump_table(gb)) {
    case 0x4176: receiveLinkState00(gb); return;
    case 0x4280: waitForNextPacket_hook(gb); return;
    case 0x438e: func_438e(gb); return;
    case 0x4179: receiveLinkState03(gb); return;
    case 0x417d: receiveLinkState06(gb); return;
    case 0x43f5: sendAckPacket_hook(gb); return;
    case 0x42a7: receiveLinkState0b(gb); return;
    case 0x40a1: disableSerialIfByteReceived_hook(gb); return;
    case 0x4096: func_4096_hook(gb); return;
    case 0x42de: receiveLinkState10(gb); return;
    case 0x430e: receiveLinkState13(gb); return;
    case 0x437b: func_437b(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void waitForSerialByte_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x41dc, 0x41de); A = H8(hReceivedSerialByte);
  CYC(0x41de, 0x41df); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x41df, 0x41e1); goto byte_received; }
  CYC(0x41df, 0x41e1);
  CYC(0x41e1, 0x41e4); A = W8(w4DisableLinkTimeout);
  CYC(0x41e4, 0x41e5); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x41e5, 0x41e7); goto return_from_caller; }
  CYC(0x41e5, 0x41e7);
  CYC(0x41e7, 0x41ea); SET_HL(w4FileLinkTimer);
  CALL_C(0x41ea, decHlRef16WithCap_hook, 0x0237, 0x41ed);
  if (F & FZ) { CYCT(0x41ed, 0x41ef); goto timeout; }
  CYC(0x41ed, 0x41ef);

return_from_caller:
  CYC(0x41ef, 0x41f0); SET_AF(pop_effect(gb));
  CYC(0x41f0, 0x41f1); ret_effect(gb); return;

timeout:
  CYC(0x41f1, 0x41f2); alu_xor(gb, A);
  CYC(0x41f2, 0x41f5); W8(w4WaitingForNextByte) = A;
  CYC(0x41f5, 0x41f7); A = 0x80;
  CYC(0x41f7, 0x41f9); H8(hFFBD) = A;
  CYC(0x41f9, 0x41fa); ret_effect(gb); return;

byte_received:
  CYC(0x41fa, 0x41fd); W8(w4WaitingForNextByte) = A;
  CYC(0x41fd, 0x41fe); alu_xor(gb, A);
  CYC(0x41fe, 0x4200); H8(hReceivedSerialByte) = A;
  CYC(0x4200, 0x4202); H8(hFFBD) = A;
  setLinkTimerTo180_hook(gb);
}

void setLinkTimerTo180_hook(GB *gb) {
  CYC(0x4202, 0x4204); A = 0xb4;
  CYC(0x4204, 0x4207); W8(w4FileLinkTimer) = A;
  CYC(0x4207, 0x4209); A = 0x00;
  CYC(0x4209, 0x420c); WP(w4FileLinkTimer)[1] = A;
  CYC(0x420c, 0x420d); ret_effect(gb);
}

void FFBE_00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x420d, 0x420f); A = H8(hSerialLinkState);
  CYC(0x420f, 0x4210); push_effect(gb, 0x4210);
  switch (serial_jump_table(gb)) {
    case 0x4186: sendFileHeader(gb); return;
    case 0x4280: waitForNextPacket_hook(gb); return;
    case 0x438e: func_438e(gb); return;
    case 0x4293: func_4293_hook(gb); return;
    case 0x4096: func_4096_hook(gb); return;
    case 0x422f: determineRingFortuneRing(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void FFBE_02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x421e, 0x4220); A = H8(hSerialLinkState);
  CYC(0x4220, 0x4221); push_effect(gb, 0x4221);
  switch (serial_jump_table(gb)) {
    case 0x4293: func_4293_hook(gb); return;
    case 0x4280: waitForNextPacket_hook(gb); return;
    case 0x4096: func_4096_hook(gb); return;
    case 0x4186: sendFileHeader(gb); return;
    case 0x438e: func_438e(gb); return;
    case 0x422f: determineRingFortuneRing(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void prepareForNextPacket_hook(GB *gb) {
  CYC(0x4269, 0x426b); A = H8(hSerialLinkState);
  CYC(0x426b, 0x426c); A = alu_inc8(gb, A);
  CYC(0x426c, 0x426e); H8(hSerialLinkState) = A;
  func_426e_hook(gb);
}

void func_426e_hook(GB *gb) {
  CYC(0x426e, 0x426f); alu_xor(gb, A);
  CYC(0x426f, 0x4272); W8(w4PacketByteIndex) = A;
  CYC(0x4272, 0x4274); H8(hFFBD) = A;
  CYC(0x4274, 0x4277); W8(w4PacketChecksum) = A;
  CYC(0x4277, 0x427a); W8(w4d984) = A;
  CYC(0x427a, 0x427b); A = alu_inc8(gb, A);
  CYC(0x427b, 0x427e); W8(w4WaitingForNextByte) = A;
  CYC(0x427e, 0x4280);
  setLinkTimerTo180_hook(gb);
}

void waitForNextPacket_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4280, func_4043_hook, 0x4043, 0x4283);
  CALL_C(0x4283, returnIfPacketNotComplete_hook, 0x44d7, 0x4286);
  CYC(0x4286, 0x4289); A = W8(w4LinkRetryCounter);
  CYC(0x4289, 0x428a); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x428a, 0x428c);
    prepareForNextPacket_hook(gb);
    return;
  }
  CYC(0x428a, 0x428c);
  CYC(0x428c, 0x428e); A = H8(hSerialLinkState);
  CYC(0x428e, 0x428f); A = alu_dec8(gb, A);
  CYC(0x428f, 0x4291); H8(hSerialLinkState) = A;
  CYC(0x4291, 0x4293);
  func_426e_hook(gb);
}

void func_4293_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4293, receivePacketByte_hook, 0x40a7, 0x4296);
  CALL_C(0x4296, returnIfPacketNotComplete_hook, 0x44d7, 0x4299);
  CYC(0x4299, 0x429c); SET_HL(w4RingFortuneStuff);
  CYC(0x429c, 0x429f); SET_DE(w4PacketBuffer + 9);
  CYC(0x429f, 0x42a1); B = 0x07;
  CALL_C(0x42a1, copyMemoryReverse_hook, 0x047f, 0x42a4);
  CYC(0x42a4, 0x42a7);
  sendAckPacket_hook(gb);
}

void sendAckPacket_hook(GB *gb) {
  CYC(0x43f5, 0x43f6); alu_xor(gb, A);
  CYC(0x43f6, 0x43f9); W8(w4LinkRetryCounter) = A;
  CYC(0x43f9, 0x43fc); SET_HL(ackPacket_bank16);
  setPacketBuffer(gb);
}

void returnIfPacketNotComplete_hook(GB *gb) {
  CYC(0x44d7, 0x44da); A = W8(w4WaitingForNextByte);
  CYC(0x44da, 0x44db); alu_or(gb, A);
  if (F & FZ) { CYCT(0x44db, 0x44dd); goto packet_complete; }
  CYC(0x44db, 0x44dd);
  CYC(0x44dd, 0x44de); SET_AF(pop_effect(gb));
  CYC(0x44de, 0x44df); ret_effect(gb); return;

packet_complete:
  CYC(0x44df, 0x44e1); A = H8(hFFBD);
  CYC(0x44e1, 0x44e2); alu_or(gb, A);
  if (F & FZ) { CYCT(0x44e2, 0x44e3); ret_effect(gb); return; }
  CYC(0x44e2, 0x44e3);
  CYC(0x44e3, 0x44e5); alu_cp(gb, 0x81);
  if (F & FZ) {
    CYCT(0x44e5, 0x44e8);
    sendRetryPacket(gb);
    return;
  }
  CYC(0x44e5, 0x44e8);
  CYC(0x44e8, 0x44e9); SET_AF(pop_effect(gb));
  CYC(0x44e9, 0x44ec);
  disableSerialPort_hook(gb);
}
