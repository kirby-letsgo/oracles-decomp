#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x16, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x16, (from), (to), true)

#define ackPacket_bank16 0x44fd
#define retryPacket_bank16 0x4500
#define ringFortuneTable_bank16 0x4503

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
void func_44ac_hook(GB *gb);
void receiveLinkState00_hook(GB *gb);
void receiveLinkState03_hook(GB *gb);
void receiveLinkState06_hook(GB *gb);
void sendFileHeader_hook(GB *gb);
void determineRingFortuneRing_hook(GB *gb);
void receiveLinkState0b_hook(GB *gb);
void func_42c5_hook(GB *gb);
void receiveLinkState10_hook(GB *gb);
void gameLinkState08_hook(GB *gb);
void gameLinkState09_hook(GB *gb);
void receiveLinkState13_hook(GB *gb);
void gameLinkState0f_hook(GB *gb);
void gameLinkState12_hook(GB *gb);
void func_4350_hook(GB *gb);
void gameLinkState0b_hook(GB *gb);
void func_437b_hook(GB *gb);
void func_438e_hook(GB *gb);
void func_439a_hook(GB *gb);
void func_43ab_hook(GB *gb);
void func_43bd_hook(GB *gb);
void gameLinkState0c_hook(GB *gb);
void sendRetryPacket_hook(GB *gb);
void setPacketBuffer_hook(GB *gb);
void gameLink_getFile1_hook(GB *gb);
void gameLink_getFile2_hook(GB *gb);
void gameLink_getFile3_hook(GB *gb);
void markFileAsBlank_hook(GB *gb);
void compareFileHeader_hook(GB *gb);
void compareFileIDsAndNames_hook(GB *gb);

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
    case 0x440d: gameLink_getFile1_hook(gb); return;
    case 0x4280: waitForNextPacket_hook(gb); return;
    case 0x4411: gameLink_getFile2_hook(gb); return;
    case 0x4415: gameLink_getFile3_hook(gb); return;
    case 0x438e: func_438e_hook(gb); return;
    case 0x4087: func_4087_hook(gb); return;
    case 0x42ee: gameLinkState08_hook(gb); return;
    case 0x4305: gameLinkState09_hook(gb); return;
    case 0x4370: gameLinkState0b_hook(gb); return;
    case 0x43c5: gameLinkState0c_hook(gb); return;
    case 0x4329: gameLinkState0f_hook(gb); return;
    case 0x4096: func_4096_hook(gb); return;
    case 0x434d: gameLinkState12_hook(gb); return;
    case 0x43f5: sendAckPacket_hook(gb); return;
    case 0x437b: func_437b_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void FFBE_03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4143, 0x4145); A = H8(hSerialLinkState);
  CYC(0x4145, 0x4146); push_effect(gb, 0x4146);
  switch (serial_jump_table(gb)) {
    case 0x4176: receiveLinkState00_hook(gb); return;
    case 0x4280: waitForNextPacket_hook(gb); return;
    case 0x438e: func_438e_hook(gb); return;
    case 0x4179: receiveLinkState03_hook(gb); return;
    case 0x417d: receiveLinkState06_hook(gb); return;
    case 0x43f5: sendAckPacket_hook(gb); return;
    case 0x42a7: receiveLinkState0b_hook(gb); return;
    case 0x40a1: disableSerialIfByteReceived_hook(gb); return;
    case 0x4096: func_4096_hook(gb); return;
    case 0x42de: receiveLinkState10_hook(gb); return;
    case 0x430e: receiveLinkState13_hook(gb); return;
    case 0x437b: func_437b_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

static void receive_link_state_common(GB *gb, uint16_t sp0_) {
  CYC(0x417f, 0x4181); H8(hActiveFileSlot) = A;
  CALL_C(0x4181, loadFile_b00_hook, 0x09dc, 0x4184);
  CYC(0x4184, 0x4186); H8(hFF8B) = A;
  sendFileHeader_hook(gb);
}

void receiveLinkState00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4176, 0x4177); alu_xor(gb, A);
  CYC(0x4177, 0x4179);
  receive_link_state_common(gb, sp0_);
}

void receiveLinkState03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4179, 0x417b); A = 0x01;
  CYC(0x417b, 0x417d);
  receive_link_state_common(gb, sp0_);
}

void receiveLinkState06_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x417d, 0x417f); A = 0x02;
  receive_link_state_common(gb, sp0_);
}

void sendFileHeader_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4186, prepareForNextPacket_hook, 0x4269, 0x4189);
  CYC(0x4189, 0x418c); SET_HL(w4PacketBuffer);
  CYC(0x418c, 0x418e); A = 0x21;
  CYC(0x418e, 0x418f); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x418f, 0x4190); C = A;
  CYC(0x4190, 0x4192); A = H8(hFF8B);
  CYC(0x4192, 0x4193); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4193, 0x4194); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4194, 0x4195); alu_add(gb, A);
  CYC(0x4195, 0x4196); alu_add(gb, C);
  CYC(0x4196, 0x4197); C = A;
  CYC(0x4197, 0x419a); A = W8(wLinkMaxHealth);
  CYC(0x419a, 0x419b); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x419b, 0x419c); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x419c, 0x419d); alu_add(gb, A);
  CYC(0x419d, 0x419e); alu_add(gb, C);
  CYC(0x419e, 0x419f); C = A;
  CYC(0x419f, 0x41a2); A = W8(wDeathCounter);
  CYC(0x41a2, 0x41a3); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x41a3, 0x41a4); alu_add(gb, C);
  CYC(0x41a4, 0x41a5); C = A;
  CYC(0x41a5, 0x41a8); A = WP(wDeathCounter)[1];
  CYC(0x41a8, 0x41a9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x41a9, 0x41aa); alu_add(gb, C);
  CYC(0x41aa, 0x41ab); C = A;
  CYC(0x41ab, 0x41ae); A = W8(wFileIsLinkedGame);
  CYC(0x41ae, 0x41af); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x41af, 0x41b0); alu_add(gb, C);
  CYC(0x41b0, 0x41b1); C = A;
  CYC(0x41b1, 0x41b4); A = W8(wFileIsHeroGame);
  CYC(0x41b4, 0x41b5); alu_add(gb, A);
  CYC(0x41b5, 0x41b6); E = A;
  CYC(0x41b6, 0x41b9); A = W8(wFileIsCompleted);
  CYC(0x41b9, 0x41ba); alu_or(gb, E);
  CYC(0x41ba, 0x41bb); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x41bb, 0x41bc); alu_add(gb, C);
  CYC(0x41bc, 0x41bd); C = A;
  CYC(0x41bd, 0x41c0); SET_DE(wGameID);
  CYC(0x41c0, 0x41c2); B = 0x16;

copy_file_header:
  CYC(0x41c2, 0x41c3); A = mem_rd(gb, DE);
  CYC(0x41c3, 0x41c4); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x41c4, 0x41c5); alu_add(gb, C);
  CYC(0x41c5, 0x41c6); C = A;
  CYC(0x41c6, 0x41c7); E = alu_inc8(gb, E);
  CYC(0x41c7, 0x41c8); B = alu_dec8(gb, B);
  if (!(F & FZ)) {
    CYCT(0x41c8, 0x41ca);
    goto copy_file_header;
  }
  CYC(0x41c8, 0x41ca);
  CYC(0x41ca, 0x41cc); A = 0xa1;
  CYC(0x41cc, 0x41cd); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x41cd, 0x41ce); alu_add(gb, C);
  CYC(0x41ce, 0x41cf); C = A;
  CYC(0x41cf, 0x41d1); A = H8(hActiveFileSlot);
  CYC(0x41d1, 0x41d2); mem_wr(gb, HL, A);
  CYC(0x41d2, 0x41d3); alu_add(gb, C);
  CYC(0x41d3, 0x41d4); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x41d4, 0x41d6); A = 0x01;
  CYC(0x41d6, 0x41d9); W8(w4WaitingForNextByte) = A;
  CYC(0x41d9, 0x41dc); sendPacketByte_hook(gb);
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
    case 0x4186: sendFileHeader_hook(gb); return;
    case 0x4280: waitForNextPacket_hook(gb); return;
    case 0x438e: func_438e_hook(gb); return;
    case 0x4293: func_4293_hook(gb); return;
    case 0x4096: func_4096_hook(gb); return;
    case 0x422f: determineRingFortuneRing_hook(gb); return;
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
    case 0x4186: sendFileHeader_hook(gb); return;
    case 0x438e: func_438e_hook(gb); return;
    case 0x422f: determineRingFortuneRing_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void determineRingFortuneRing_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x422f, disableSerialPort_hook, 0x0c7e, 0x4232);
  CYC(0x4232, 0x4233); alu_xor(gb, A);
  CYC(0x4233, 0x4235); H8(hFFBD) = A;
  CALL_C(0x4235, compareFileHeader_hook, 0x44ec, 0x4238);
  if (F & FZ) {
    CYCT(0x4238, 0x423a);
    goto no_match;
  }
  CYC(0x4238, 0x423a);
  CYC(0x423a, 0x423d); SET_HL(wGameID);
  CYC(0x423d, 0x4240); A = W8(w4RingFortuneStuff);
  CYC(0x4240, 0x4241); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4241, 0x4243); alu_and(gb, 0x7f);
  CYC(0x4243, 0x4245); B = 0x00;
  CYC(0x4245, 0x4247); alu_and(gb, 0x7c);
  if (F & FZ) {
    CYCT(0x4247, 0x4249);
    goto choose_ring;
  }
  CYC(0x4247, 0x4249);
  CYC(0x4249, 0x424a); B = alu_inc8(gb, B);
  CYC(0x424a, 0x424c); alu_and(gb, 0x60);
  if (F & FZ) {
    CYCT(0x424c, 0x424e);
    goto choose_ring;
  }
  CYC(0x424c, 0x424e);
  CYC(0x424e, 0x424f); B = alu_inc8(gb, B);

choose_ring:
  CYC(0x424f, 0x4250); L = alu_inc8(gb, L);
  CYC(0x4250, 0x4251); C = mem_rd(gb, HL);
  CYC(0x4251, 0x4252); A = B;
  CYC(0x4252, 0x4255); SET_HL(ringFortuneTable_bank16);
  CYC(0x4255, 0x4256); serial_add_a_to_hl(gb, 0x4256);
  CYC(0x4256, 0x4257); A = mem_rd(gb, HL);
  CYC(0x4257, 0x4258); serial_add_a_to_hl(gb, 0x4258);
  CYC(0x4258, 0x425b); A = mem_rd(gb, w4RingFortuneStuff + 1);
  CYC(0x425b, 0x425c); alu_add(gb, C);
  CYC(0x425c, 0x425e); alu_and(gb, 0x07);
  CYC(0x425e, 0x425f); serial_add_a_to_hl(gb, 0x425f);
  CYC(0x425f, 0x4260); A = mem_rd(gb, HL);
  CYC(0x4260, 0x4263); W8(w4RingFortuneStuff) = A;
  CYC(0x4263, 0x4264); ret_effect(gb); return;

no_match:
  CYC(0x4264, 0x4266); A = 0x84;
  CYC(0x4266, 0x4268); H8(hFFBD) = A;
  CYC(0x4268, 0x4269); ret_effect(gb);
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

void receiveLinkState0b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x42a7, 0x42aa); A = W8(w4PacketByteIndex);
  CYC(0x42aa, 0x42ab); alu_or(gb, A);
  CYC(0x42ab, 0x42ad); A = 0x00;
  if (!(F & FZ)) {
    CYCT(0x42ad, 0x42af);
    goto set_timeout_mode;
  }
  CYC(0x42ad, 0x42af);
  CYC(0x42af, 0x42b0); A = alu_inc8(gb, A);

set_timeout_mode:
  CYC(0x42b0, 0x42b3); W8(w4DisableLinkTimeout) = A;
  CALL_C(0x42b3, receivePacketByte_hook, 0x40a7, 0x42b6);
  CYC(0x42b6, 0x42b9); A = W8(w4WaitingForNextByte);
  CYC(0x42b9, 0x42ba); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x42ba, 0x42bb); ret_effect(gb); return; }
  CYC(0x42ba, 0x42bb);
  CYC(0x42bb, 0x42be); A = mem_rd(gb, w4PacketBuffer + 1);
  CYC(0x42be, 0x42c0); alu_cp(gb, 0xc0);
  if (!(F & FZ)) {
    CYCT(0x42c0, 0x42c2);
    func_42c5_hook(gb);
    return;
  }
  CYC(0x42c0, 0x42c2);
  CYC(0x42c2, 0x42c5); sendAckPacket_hook(gb);
}

void func_42c5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x42c5, 0x42c7); alu_cp(gb, 0xb0);
  if (!(F & FZ)) {
    CYCT(0x42c7, 0x42ca);
    sendRetryPacket_hook(gb);
    return;
  }
  CYC(0x42c7, 0x42ca);
  CYC(0x42ca, 0x42cd); A = mem_rd(gb, w4PacketBuffer + 2);
  CYC(0x42cd, 0x42cf); H8(hActiveFileSlot) = A;
  CYC(0x42cf, 0x42d1); alu_cp(gb, 0x03);
  if (!(F & FC)) {
    CYCT(0x42d1, 0x42d4);
    disableSerialPort_hook(gb);
    return;
  }
  CYC(0x42d1, 0x42d4);
  CALL_C(0x42d4, loadFile_b00_hook, 0x09dc, 0x42d7);
  CYC(0x42d7, 0x42d9); A = 0x0d;
  CYC(0x42d9, 0x42db); H8(hSerialLinkState) = A;
  CYC(0x42db, 0x42de); sendAckPacket_hook(gb);
}

void receiveLinkState10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x42de, prepareForNextPacket_hook, 0x4269, 0x42e1);
  CYC(0x42e1, 0x42e4); SET_HL(w4RingFortuneStuff);
  CYC(0x42e4, 0x42e7); SET_DE(wRingsObtained);
  CYC(0x42e7, 0x42e9); B = 0x08;
  CALL_C(0x42e9, copyMemoryReverse_hook, 0x047f, 0x42ec);
  CYC(0x42ec, 0x42ee); func_4350_hook(gb);
}

void gameLinkState08_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x42ee, prepareForNextPacket_hook, 0x4269, 0x42f1);
  CYC(0x42f1, 0x42f4); SET_HL(w4PacketBuffer);
  CYC(0x42f4, 0x42f6); A = 0x03;
  CYC(0x42f6, 0x42f7); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x42f7, 0x42f9); A = 0xc0;
  CYC(0x42f9, 0x42fa); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x42fa, 0x42fc); A = 0xc3;
  CYC(0x42fc, 0x42fd); mem_wr(gb, HL, A);
  CYC(0x42fd, 0x42ff); A = 0x01;
  CYC(0x42ff, 0x4302); W8(w4WaitingForNextByte) = A;
  CYC(0x4302, 0x4305); sendPacketByte_hook(gb);
}

void gameLinkState09_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4305, func_4043_hook, 0x4043, 0x4308);
  CALL_C(0x4308, returnIfPacketNotComplete_hook, 0x44d7, 0x430b);
  CYC(0x430b, 0x430e); prepareForNextPacket_hook(gb);
}

void receiveLinkState13_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x430e, receivePacketByte_hook, 0x40a7, 0x4311);
  CALL_C(0x4311, returnIfPacketNotComplete_hook, 0x44d7, 0x4314);
  CYC(0x4314, 0x4316); A = H8(hFFBD);
  CYC(0x4316, 0x4318); alu_cp(gb, 0x81);
  if (F & FZ) {
    CYCT(0x4318, 0x431b);
    prepareForNextPacket_hook(gb);
    return;
  }
  CYC(0x4318, 0x431b);
  CYC(0x431b, 0x431e); SET_HL(w4RingFortuneStuff);
  CYC(0x431e, 0x4321); SET_DE(w4PacketBuffer + 1);
  CYC(0x4321, 0x4323); B = 0x08;
  CALL_C(0x4323, copyMemoryReverse_hook, 0x047f, 0x4326);
  CYC(0x4326, 0x4329); prepareForNextPacket_hook(gb);
}

void gameLinkState0f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4329, receivePacketByte_hook, 0x40a7, 0x432c);
  CALL_C(0x432c, returnIfPacketNotComplete_hook, 0x44d7, 0x432f);
  CYC(0x432f, 0x4332); SET_HL(wRingsObtained);
  CYC(0x4332, 0x4335); SET_DE(w4PacketBuffer + 1);
  CYC(0x4335, 0x4337); B = 0x08;

merge_rings:
  CYC(0x4337, 0x4338); A = mem_rd(gb, DE);
  CYC(0x4338, 0x4339); alu_or(gb, mem_rd(gb, HL));
  CYC(0x4339, 0x433a); mem_wr(gb, DE, A);
  CYC(0x433a, 0x433b); SET_HL(HL + 1);
  CYC(0x433b, 0x433c); SET_DE(DE + 1);
  CYC(0x433c, 0x433d); B = alu_dec8(gb, B);
  if (!(F & FZ)) {
    CYCT(0x433d, 0x433f);
    goto merge_rings;
  }
  CYC(0x433d, 0x433f);
  CYC(0x433f, 0x4342); SET_HL(w4RingFortuneStuff);
  CYC(0x4342, 0x4345); SET_DE(w4PacketBuffer + 1);
  CYC(0x4345, 0x4347); B = 0x08;
  CALL_C(0x4347, copyMemoryReverse_hook, 0x047f, 0x434a);
  CYC(0x434a, 0x434d); sendAckPacket_hook(gb);
}

void gameLinkState12_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x434d, prepareForNextPacket_hook, 0x4269, 0x4350);
  func_4350_hook(gb);
}

void func_4350_hook(GB *gb) {
  CYC(0x4350, 0x4352); A = 0x0a;
  CYC(0x4352, 0x4353); C = A;
  CYC(0x4353, 0x4356); W8(w4PacketBuffer) = A;
  CYC(0x4356, 0x4359); SET_DE(w4PacketBuffer + 1);
  CYC(0x4359, 0x435c); SET_HL(w4RingFortuneStuff);
  CYC(0x435c, 0x435e); B = 0x08;

copy_ring_fortune:
  CYC(0x435e, 0x435f); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x435f, 0x4360); mem_wr(gb, DE, A);
  CYC(0x4360, 0x4361); SET_DE(DE + 1);
  CYC(0x4361, 0x4362); alu_add(gb, C);
  CYC(0x4362, 0x4363); C = A;
  CYC(0x4363, 0x4364); B = alu_dec8(gb, B);
  if (!(F & FZ)) {
    CYCT(0x4364, 0x4366);
    goto copy_ring_fortune;
  }
  CYC(0x4364, 0x4366);
  CYC(0x4366, 0x4367); A = C;
  CYC(0x4367, 0x4368); mem_wr(gb, DE, A);
  CYC(0x4368, 0x436a); A = 0x01;
  CYC(0x436a, 0x436d); W8(w4WaitingForNextByte) = A;
  CYC(0x436d, 0x4370); sendPacketByte_hook(gb);
}

void gameLinkState0b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4370, waitForSerialByte_hook, 0x41dc, 0x4373);
  CYC(0x4373, 0x4375); alu_cp(gb, 0x80);
  if (F & FZ) {
    CYCT(0x4375, 0x4378);
    disableSerialPort_hook(gb);
    return;
  }
  CYC(0x4375, 0x4378);
  CYC(0x4378, 0x437b); disableSerialPort_hook(gb);
}

void func_437b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x437b, disableSerialPort_hook, 0x0c7e, 0x437e);
  CYC(0x437e, 0x4380); H8(hFFBD) = A;
  CYC(0x4380, 0x4383); SET_DE(w4RingFortuneStuff);
  CYC(0x4383, 0x4386); SET_HL(wRingsObtained);
  CYC(0x4386, 0x4388); B = 0x08;
  CALL_C(0x4388, copyMemoryReverse_hook, 0x047f, 0x438b);
  CYC(0x438b, 0x438e); saveFile_b00_hook(gb);
}

void func_438e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x438e, func_439a_hook, 0x439a, 0x4391);
  CALL_C(0x4391, returnIfPacketNotComplete_hook, 0x44d7, 0x4394);
  CALL_C(0x4394, prepareForNextPacket_hook, 0x4269, 0x4397);
  CYC(0x4397, 0x439a); func_4036_hook(gb);
}

void func_439a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x439a, receivePacketByte_hook, 0x40a7, 0x439d);
  CYC(0x439d, 0x43a0); A = W8(w4WaitingForNextByte);
  CYC(0x43a0, 0x43a1); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x43a1, 0x43a2); ret_effect(gb); return; }
  CYC(0x43a1, 0x43a2);
  CYC(0x43a2, 0x43a4); A = H8(hFFBD);
  CYC(0x43a4, 0x43a5); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x43a5, 0x43a7);
    func_43ab_hook(gb);
    return;
  }
  CYC(0x43a5, 0x43a7);
  CYC(0x43a7, 0x43a8); SET_AF(pop_effect(gb));
  CYC(0x43a8, 0x43ab); disableSerialPort_hook(gb);
}

void func_43ab_hook(GB *gb) {
  CYC(0x43ab, 0x43ae); A = mem_rd(gb, w4PacketBuffer + 1);
  CYC(0x43ae, 0x43b0); alu_cp(gb, 0xb1);
  if (!(F & FZ)) {
    CYCT(0x43b0, 0x43b2);
    func_43bd_hook(gb);
    return;
  }
  CYC(0x43b0, 0x43b2);
  CYC(0x43b2, 0x43b3); alu_xor(gb, A);
  CYC(0x43b3, 0x43b6); W8(w4LinkRetryCounter) = A;
  CYC(0x43b6, 0x43b8); A = H8(hSerialLinkState);
  CYC(0x43b8, 0x43ba); alu_sub(gb, 0x02);
  CYC(0x43ba, 0x43bc); H8(hSerialLinkState) = A;
  CYC(0x43bc, 0x43bd); ret_effect(gb);
}

void func_43bd_hook(GB *gb) {
  CYC(0x43bd, 0x43bf); alu_cp(gb, 0xb0);
  if (F & FZ) { CYCT(0x43bf, 0x43c0); ret_effect(gb); return; }
  CYC(0x43bf, 0x43c0);
  CYC(0x43c0, 0x43c2); A = 0x82;
  CYC(0x43c2, 0x43c4); H8(hFFBD) = A;
  CYC(0x43c4, 0x43c5); ret_effect(gb);
}

void gameLinkState0c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x43c5, prepareForNextPacket_hook, 0x4269, 0x43c8);
  CYC(0x43c8, 0x43cb); SET_HL(w4PacketBuffer);
  CYC(0x43cb, 0x43cd); A = 0x04;
  CYC(0x43cd, 0x43ce); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x43ce, 0x43d0); A = 0xb0;
  CYC(0x43d0, 0x43d1); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x43d1, 0x43d4); A = W8(wFileSelect_cursorPos);
  CYC(0x43d4, 0x43d5); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x43d5, 0x43d7); alu_add(gb, 0xb4);
  CYC(0x43d7, 0x43d8); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x43d8, 0x43da); A = 0x01;
  CYC(0x43da, 0x43dd); W8(w4WaitingForNextByte) = A;
  CYC(0x43dd, 0x43e0); sendPacketByte_hook(gb);
}

void sendRetryPacket_hook(GB *gb) {
  CYC(0x43e0, 0x43e3); SET_HL(retryPacket_bank16);
  CYC(0x43e3, 0x43e6); A = W8(w4LinkRetryCounter);
  CYC(0x43e6, 0x43e7); A = alu_inc8(gb, A);
  CYC(0x43e7, 0x43ea); W8(w4LinkRetryCounter) = A;
  CYC(0x43ea, 0x43ec); alu_cp(gb, 0x05);
  if (F & FC) {
    CYCT(0x43ec, 0x43ee);
    setPacketBuffer_hook(gb);
    return;
  }
  CYC(0x43ec, 0x43ee);
  CYC(0x43ee, 0x43f0); A = 0x80;
  CYC(0x43f0, 0x43f2); H8(hFFBD) = A;
  CYC(0x43f2, 0x43f5); disableSerialPort_hook(gb);
}

void sendAckPacket_hook(GB *gb) {
  CYC(0x43f5, 0x43f6); alu_xor(gb, A);
  CYC(0x43f6, 0x43f9); W8(w4LinkRetryCounter) = A;
  CYC(0x43f9, 0x43fc); SET_HL(ackPacket_bank16);
  setPacketBuffer_hook(gb);
}

void setPacketBuffer_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x43fc, prepareForNextPacket_hook, 0x4269, 0x43ff);
  CYC(0x43ff, 0x4400); A = mem_rd(gb, HL);
  CYC(0x4400, 0x4401); B = A;
  CYC(0x4401, 0x4404); SET_DE(w4PacketBuffer);

copy_packet:
  CYC(0x4404, 0x4405); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4405, 0x4406); mem_wr(gb, DE, A);
  CYC(0x4406, 0x4407); SET_DE(DE + 1);
  CYC(0x4407, 0x4408); B = alu_dec8(gb, B);
  if (!(F & FZ)) {
    CYCT(0x4408, 0x440a);
    goto copy_packet;
  }
  CYC(0x4408, 0x440a);
  CYC(0x440a, 0x440d); sendPacketByte_hook(gb);
}

static void game_link_get_file_body(GB *gb, uint16_t sp0_) {
  CYC(0x4417, 0x4419); H8(hFF8B) = A;
  CALL_C(0x4419, receivePacketByte_hook, 0x40a7, 0x441c);
  CALL_C(0x441c, returnIfPacketNotComplete_hook, 0x44d7, 0x441f);
  CYC(0x441f, 0x4421); A = H8(hFF8B);
  CYC(0x4421, 0x4424); SET_HL(w2ColorComponentBuffer1 + 5);
  if (!(F & FZ)) {
    CYCT(0x4424, 0x4426);
    sendRetryPacket_hook(gb);
    return;
  }
  CYC(0x4424, 0x4426);
  CYC(0x4426, 0x4428); A = alu_swap(gb, A);
  CYC(0x4428, 0x4429); alu_rrca(gb);
  CYC(0x4429, 0x442c); SET_HL(w4FileDisplayVariables);
  CYC(0x442c, 0x442d); serial_add_a_to_hl(gb, 0x442d);
  CYC(0x442d, 0x4430); SET_DE(w4PacketBuffer + 1);
  CYC(0x4430, 0x4432); B = 0x08;

copy_file_status:
  CYC(0x4432, 0x4433); A = mem_rd(gb, DE);
  CYC(0x4433, 0x4434); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4434, 0x4435); SET_DE(DE + 1);
  CYC(0x4435, 0x4436); B = alu_dec8(gb, B);
  if (!(F & FZ)) {
    CYCT(0x4436, 0x4438);
    goto copy_file_status;
  }
  CYC(0x4436, 0x4438);
  CYC(0x4438, 0x443a); A = H8(hFF8B);
  CYC(0x443a, 0x443b); alu_add(gb, A);
  CYC(0x443b, 0x443c); E = A;
  CYC(0x443c, 0x443d); alu_add(gb, A);
  CYC(0x443d, 0x443e); alu_add(gb, E);
  CYC(0x443e, 0x4441); SET_HL(w4NameBuffer);
  CYC(0x4441, 0x4442); serial_add_a_to_hl(gb, 0x4442);
  CYC(0x4442, 0x4445); SET_DE(w4PacketBuffer + 0x0b);
  CYC(0x4445, 0x4447); B = 0x06;
  CALL_C(0x4447, copyMemoryReverse_hook, 0x047f, 0x444a);
  CYC(0x444a, 0x444c); A = H8(hFF8B);
  CYC(0x444c, 0x444d); A = alu_inc8(gb, A);
  CYC(0x444d, 0x4450); SET_HL(w4RingFortuneStuff);
  CYC(0x4450, 0x4453); SET_BC(0x0016);

seek_file_header:
  CYC(0x4453, 0x4454); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(0x4454, 0x4456);
    goto copy_file_header;
  }
  CYC(0x4454, 0x4456);
  CYC(0x4456, 0x4457); alu_add_hl(gb, BC);
  CYC(0x4457, 0x4459);
  goto seek_file_header;

copy_file_header:
  CYC(0x4459, 0x445b); B = 0x16;
  CYC(0x445b, 0x445e); SET_DE(w4PacketBuffer + 9);
  CALL_C(0x445e, copyMemoryReverse_hook, 0x047f, 0x4461);
  CYC(0x4461, 0x4464); A = W8(wOpenedMenuType);
  CYC(0x4464, 0x4466); alu_cp(gb, 0x08);
  if (!(F & FZ)) {
    CYCT(0x4466, 0x4468);
    goto verify_other_mode;
  }
  CYC(0x4466, 0x4468);
  CYC(0x4468, 0x446b); SET_DE(w4PacketBuffer + 9);
  CALL_C(0x446b, compareFileIDsAndNames_hook, 0x44ef, 0x446e);
  if (!(F & FZ)) {
    CYCT(0x446e, 0x4470);
    markFileAsBlank_hook(gb);
    return;
  }
  CYC(0x446e, 0x4470);
  CYC(0x4470, 0x4473); SET_HL(w2ColorComponentBuffer1);
  CYC(0x4473, 0x4474); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4474, 0x4475); alu_or(gb, mem_rd(gb, HL));
  CYC(0x4475, 0x4476); L = alu_inc8(gb, L);
  CYC(0x4476, 0x4477); alu_or(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x4477, 0x4479);
    markFileAsBlank_hook(gb);
    return;
  }
  CYC(0x4477, 0x4479);
  CYC(0x4479, 0x447c); sendAckPacket_hook(gb);
  return;

verify_other_mode:
  CYC(0x447c, 0x447f); A = mem_rd(gb, w2ColorComponentBuffer1 + 4);
  CYC(0x447f, 0x4481); alu_cp(gb, 0xa0);
  if (!(F & FZ)) {
    CYCT(0x4481, 0x4483);
    markFileAsBlank_hook(gb);
    return;
  }
  CYC(0x4481, 0x4483);
  CYC(0x4483, 0x4486); A = mem_rd(gb, w2ColorComponentBuffer1 + 2);
  CYC(0x4486, 0x4487); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4487, 0x4489);
    markFileAsBlank_hook(gb);
    return;
  }
  CYC(0x4487, 0x4489);
  CYC(0x4489, 0x448c); sendAckPacket_hook(gb);
}

void gameLink_getFile1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x440d, 0x440f); A = 0x00;
  CYC(0x440f, 0x4411);
  game_link_get_file_body(gb, sp0_);
}

void gameLink_getFile2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4411, 0x4413); A = 0x01;
  CYC(0x4413, 0x4415);
  game_link_get_file_body(gb, sp0_);
}

void gameLink_getFile3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4415, 0x4417); A = 0x02;
  game_link_get_file_body(gb, sp0_);
}

void markFileAsBlank_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x448c, 0x448e); A = H8(hFF8B);
  CYC(0x448e, 0x4490); D = 0x00;
  CYC(0x4490, 0x4492); A = alu_swap(gb, A);
  CYC(0x4492, 0x4493); alu_rrca(gb);
  CYC(0x4493, 0x4494); alu_add(gb, D);
  CYC(0x4494, 0x4497); SET_HL(w4FileDisplayVariables);
  CYC(0x4497, 0x4498); serial_add_a_to_hl(gb, 0x4498);
  CYC(0x4498, 0x449a); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x80));
  CYC(0x449a, 0x449c); A = H8(hFF8B);
  CYC(0x449c, 0x449d); alu_add(gb, A);
  CYC(0x449d, 0x449e); E = A;
  CYC(0x449e, 0x449f); alu_add(gb, A);
  CYC(0x449f, 0x44a0); alu_add(gb, E);
  CYC(0x44a0, 0x44a3); SET_HL(w4NameBuffer);
  CYC(0x44a3, 0x44a4); serial_add_a_to_hl(gb, 0x44a4);
  CYC(0x44a4, 0x44a6); B = 0x06;
  CALL_C(0x44a6, clearMemory_hook, 0x046f, 0x44a9);
  CYC(0x44a9, 0x44ac); sendAckPacket_hook(gb);
}

void func_44ac_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x44ac, 0x44ae); A = mem_rd(gb, IO_SVBK);
  CYC(0x44ae, 0x44af); push_effect(gb, AF);
  CYC(0x44af, 0x44b1); A = 0x04;
  CYC(0x44b1, 0x44b3); mem_wr(gb, IO_SVBK, A);
  CYC(0x44b3, 0x44b4); alu_xor(gb, A);
  CYC(0x44b4, 0x44b7); SET_HL(w4d980);
  CYC(0x44b7, 0x44b8); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x44b8, 0x44b9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x44b9, 0x44ba); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x44ba, 0x44bb); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x44bb, 0x44bc); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x44bc, 0x44bd); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x44bd, 0x44be); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x44be, 0x44c0); H8(hFFBE) = A;
  CYC(0x44c0, 0x44c2); H8(hSerialLinkState) = A;
  CYC(0x44c2, 0x44c4); H8(hFFBD) = A;
  CALL_C(0x44c4, setLinkTimerTo180_hook, 0x4202, 0x44c7);
  CYC(0x44c7, 0x44c9); A = 0xe1;
  CYC(0x44c9, 0x44cb); mem_wr(gb, IO_SB, A);
  CYC(0x44cb, 0x44cd); A = 0x80;
  CYC(0x44cd, 0x44d0); W8(w4WaitingForNextByte) = A;
  CALL_C(0x44d0, writeToSC_hook, 0x0c6a, 0x44d3);
  CYC(0x44d3, 0x44d4); SET_AF(pop_effect(gb));
  CYC(0x44d4, 0x44d6); mem_wr(gb, IO_SVBK, A);
  CYC(0x44d6, 0x44d7); ret_effect(gb);
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
    sendRetryPacket_hook(gb);
    return;
  }
  CYC(0x44e5, 0x44e8);
  CYC(0x44e8, 0x44e9); SET_AF(pop_effect(gb));
  CYC(0x44e9, 0x44ec);
  disableSerialPort_hook(gb);
}

void compareFileHeader_hook(GB *gb) {
  CYC(0x44ec, 0x44ef); SET_DE(w4RingFortuneStuff);
  compareFileIDsAndNames_hook(gb);
}

void compareFileIDsAndNames_hook(GB *gb) {
  CYC(0x44ef, 0x44f2); SET_HL(wGameID);
  CYC(0x44f2, 0x44f4); B = 0x07;

compare_byte:
  CYC(0x44f4, 0x44f5); A = mem_rd(gb, DE);
  CYC(0x44f5, 0x44f6); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x44f6, 0x44f7); ret_effect(gb); return; }
  CYC(0x44f6, 0x44f7);
  CYC(0x44f7, 0x44f8); SET_DE(DE + 1);
  CYC(0x44f8, 0x44f9); L = alu_inc8(gb, L);
  CYC(0x44f9, 0x44fa); B = alu_dec8(gb, B);
  if (!(F & FZ)) {
    CYCT(0x44fa, 0x44fc);
    goto compare_byte;
  }
  CYC(0x44fa, 0x44fc);
  CYC(0x44fc, 0x44fd); ret_effect(gb);
}
