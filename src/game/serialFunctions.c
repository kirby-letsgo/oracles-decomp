#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x16, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x16, (from), (to), true)

void func_4043_hook(GB *gb);
void sendPacketByte_hook(GB *gb);
void func_4087_hook(GB *gb);
void func_4096_hook(GB *gb);
void disableSerialIfByteReceived_hook(GB *gb);
void receivePacketByte_hook(GB *gb);
void prepareForNextPacket_hook(GB *gb);
void func_426e_hook(GB *gb);
void waitForNextPacket_hook(GB *gb);
void func_4293_hook(GB *gb);

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

void func_4043_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4043, waitForSerialByte, 0x41dc, 0x4046);
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
  CALL_C(0x4096, waitForSerialByte, 0x41dc, 0x4099);
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
  CALL_C(0x40a1, waitForSerialByte, 0x41dc, 0x40a4);
  CYC(0x40a4, 0x40a7);
  disableSerialPort_hook(gb);
}

void receivePacketByte_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x40a7, 0x40a8); alu_xor(gb, A);
  CYC(0x40a8, 0x40ab); W8(w4d98b) = A;
  CALL_C(0x40ab, waitForSerialByte, 0x41dc, 0x40ae);
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
  setLinkTimerTo180(gb);
}

void waitForNextPacket_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4280, func_4043_hook, 0x4043, 0x4283);
  CALL_C(0x4283, returnIfPacketNotComplete, 0x44d7, 0x4286);
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
  CALL_C(0x4296, returnIfPacketNotComplete, 0x44d7, 0x4299);
  CYC(0x4299, 0x429c); SET_HL(w4RingFortuneStuff);
  CYC(0x429c, 0x429f); SET_DE(w4PacketBuffer + 9);
  CYC(0x429f, 0x42a1); B = 0x07;
  CALL_C(0x42a1, copyMemoryReverse_hook, 0x047f, 0x42a4);
  CYC(0x42a4, 0x42a7);
  sendAckPacket(gb);
}
