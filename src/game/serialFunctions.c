#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

#define ackPacket_bank16 SYM(ackPacket)
#define retryPacket_bank16 SYM(retryPacket)
#define ringFortuneTable_bank16 SYM(ringFortuneTable)

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
  BASE(func_4000_b16);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = H8(hSerialInterruptBehaviour);
  CYC(b_+2, b_+3); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); A = mem_rd(gb, IO_SVBK);
  CYC(b_+6, b_+7); push_effect(gb, AF);
  CYC(b_+7, b_+9); A = 0x04;
  CYC(b_+9, b_+11); mem_wr(gb, IO_SVBK, A);
  CYC(b_+11, b_+12); push_effect(gb, DE);
  CALL_C(b_+12, func_4036_hook, SYM(func_4036), b_+15);
  CYC(b_+15, b_+16); SET_DE(pop_effect(gb));
  CYC(b_+16, b_+18); A = mem_rd(gb, IO_SC);
  CYC(b_+18, b_+19); alu_rlca(gb);
  if (F & FC) { CYCT(b_+19, b_+21); goto restore_wram_bank; }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+23); A = H8(hSerialInterruptBehaviour);
  CYC(b_+23, b_+25); alu_cp(gb, 0xe0);
  if (F & FZ) { CYCT(b_+25, b_+27); goto write_serial_control; }
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+30); A = W8(w4d98b);
  CYC(b_+30, b_+31); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+31, b_+33); goto restore_wram_bank; }
  CYC(b_+31, b_+33);
  CYC(b_+33, b_+36); A = W8(w4d983);
  CYC(b_+36, b_+38); alu_xor(gb, 0x01);
  CYC(b_+38, b_+41); W8(w4d983) = A;
  if (F & FZ) { CYCT(b_+41, b_+43); goto restore_wram_bank; }
  CYC(b_+41, b_+43);
  CYC(b_+43, b_+45); A = H8(hSerialInterruptBehaviour);

write_serial_control:
  CYC(b_+45, b_+47); alu_and(gb, 0x81);
  CALL_C(b_+47, writeToSC_hook, SYM(writeToSC), b_+50);

restore_wram_bank:
  CYC(b_+50, b_+51); SET_AF(pop_effect(gb));
  CYC(b_+51, b_+53); mem_wr(gb, IO_SVBK, A);
  CYC(b_+53, b_+54); ret_effect(gb);
}

void func_4036_hook(GB *gb) {
  BASE(func_4036);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = H8(hFFBE);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  do { uint16_t jt_ = (serial_jump_table(gb));
    if (jt_ == SYM(FFBE_00) && hook_is(gb, SYM(FFBE_00), FFBE_00_hook)) { FFBE_00_hook(gb); return; }
    else if (jt_ == SYM(FFBE_02) && hook_is(gb, SYM(FFBE_02), FFBE_02_hook)) { FFBE_02_hook(gb); return; }
    else if (jt_ == SYM(FFBE_03) && hook_is(gb, SYM(FFBE_03), FFBE_03_hook)) { FFBE_03_hook(gb); return; }
    else if (jt_ == SYM(FFBE_04) && hook_is(gb, SYM(FFBE_04), FFBE_04_hook)) { FFBE_04_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void func_4043_hook(GB *gb) {
  BASE(func_4043);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, waitForSerialByte_hook, SYM(waitForSerialByte), b_+3);
  CYC(b_+3, b_+5); alu_cp(gb, 0x80);
  if (F & FZ) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  TAIL(sendPacketByte);
}

void sendPacketByte_hook(GB *gb) {
  BASE(sendPacketByte);
  CYC(b_+0, b_+3); A = W8(w4PacketByteIndex);
  CYC(b_+3, b_+6); SET_HL(w4PacketBuffer);
  CYC(b_+6, b_+7); serial_add_a_to_hl(gb, b_+7);
  CYC(b_+7, b_+10); A = W8(w4PacketByteIndex);
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+11, b_+13);
    goto next_byte;
  }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+15); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+15, b_+17);
    goto get_num_bytes;
  }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+18); A = alu_inc8(gb, A);
  CYC(b_+18, b_+21); W8(w4d98b) = A;
  CYC(b_+21, b_+22); ret_effect(gb); return;

get_num_bytes:
  CYC(b_+22, b_+25); W8(w4NumPacketBytes) = A;
  CYC(b_+25, b_+26); alu_xor(gb, A);
  CYC(b_+26, b_+29); W8(w4PacketChecksum) = A;

next_byte:
  CYC(b_+29, b_+30); A = alu_inc8(gb, A);
  CYC(b_+30, b_+33); W8(w4PacketByteIndex) = A;
  CYC(b_+33, b_+36); A = W8(w4NumPacketBytes);
  CYC(b_+36, b_+37); A = alu_dec8(gb, A);
  CYC(b_+37, b_+40); W8(w4NumPacketBytes) = A;
  CYC(b_+40, b_+41); A = mem_rd(gb, HL); SET_HL(HL + 1);
  if (!(F & FZ)) {
    CYCT(b_+41, b_+43);
  } else {
    CYC(b_+41, b_+43);
    CYC(b_+43, b_+44); alu_xor(gb, A);
    CYC(b_+44, b_+47); W8(w4WaitingForNextByte) = A;
    CYC(b_+47, b_+50); A = W8(w4PacketChecksum);
  }
  CYC(b_+50, b_+52); mem_wr(gb, IO_SB, A);
  CYC(b_+52, b_+55); SET_HL(w4PacketChecksum);
  CYC(b_+55, b_+56); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+56, b_+57); mem_wr(gb, HL, A);
  CYC(b_+57, b_+58); alu_xor(gb, A);
  CYC(b_+58, b_+61); W8(w4d98b) = A;
  CYC(b_+61, b_+62); ret_effect(gb);
}

void func_4087_hook(GB *gb) {
  BASE(func_4087);
  CYC(b_+0, b_+2); A = H8(hReceivedSerialByte);
  CYC(b_+2, b_+3); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); A = 0x01;
  CYC(b_+6, b_+9); W8(w4d98b) = A;
  CYC(b_+9, b_+10); alu_xor(gb, A);
  CYC(b_+10, b_+12); mem_wr(gb, IO_SB, A);
  CYC(b_+12, b_+14); H8(hReceivedSerialByte) = A;
  CYC(b_+14, b_+15); ret_effect(gb);
}

void func_4096_hook(GB *gb) {
  BASE(func_4096);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, waitForSerialByte_hook, SYM(waitForSerialByte), b_+3);
  CYC(b_+3, b_+5); alu_cp(gb, 0x80);
  if (F & FZ) {
    CYCT(b_+5, b_+8);
    TAIL(disableSerialPort);
  }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+11);
  TAIL(prepareForNextPacket);
}

void disableSerialIfByteReceived_hook(GB *gb) {
  BASE(disableSerialIfByteReceived);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, waitForSerialByte_hook, SYM(waitForSerialByte), b_+3);
  CYC(b_+3, b_+6);
  TAIL(disableSerialPort);
}

void receivePacketByte_hook(GB *gb) {
  BASE(receivePacketByte);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); W8(w4d98b) = A;
  CALL_C(b_+4, waitForSerialByte_hook, SYM(waitForSerialByte), b_+7);
  CYC(b_+7, b_+9); alu_cp(gb, 0x80);
  if (F & FZ) { CYCT(b_+9, b_+10); ret_effect(gb); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+13); A = W8(w4PacketByteIndex);
  CYC(b_+13, b_+14); B = A;
  CYC(b_+14, b_+15); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+15, b_+17);
    goto got_packet_byte;
  }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); A = H8(hSerialByte);
  CYC(b_+19, b_+21); alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(b_+21, b_+23);
    goto invalid_length;
  }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+24); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+24, b_+26);
    goto got_packet_length;
  }
  CYC(b_+24, b_+26);

invalid_length:
  CYC(b_+26, b_+29); A = W8(w4DisableLinkTimeout);
  CYC(b_+29, b_+30); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+30, b_+31); ret_effect(gb); return; }
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+34); SET_HL(w4d984);
  CYC(b_+34, b_+35); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+35, b_+36); ret_effect(gb); return; }
  CYC(b_+35, b_+36);
  CYC(b_+36, b_+38); A = 0x86;
  CYC(b_+38, b_+40); H8(hFFBD) = A;
  CYC(b_+40, b_+41); alu_xor(gb, A);
  CYC(b_+41, b_+44); W8(w4WaitingForNextByte) = A;
  CYC(b_+44, b_+45); ret_effect(gb); return;

got_packet_length:
  CYC(b_+45, b_+48); W8(w4NumPacketBytes) = A;

got_packet_byte:
  CYC(b_+48, b_+51); SET_HL(w4NumPacketBytes);
  CYC(b_+51, b_+52); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+52, b_+54);
    goto get_next_byte;
  }
  CYC(b_+52, b_+54);
  CYC(b_+54, b_+56); A = H8(hSerialByte);
  CYC(b_+56, b_+59); SET_HL(w4PacketChecksum);
  CYC(b_+59, b_+60); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+60, b_+62);
  } else {
    CYC(b_+60, b_+62);
    CYC(b_+62, b_+64); A = 0x81;
    CYC(b_+64, b_+66); H8(hFFBD) = A;
  }
  CYC(b_+66, b_+67); alu_xor(gb, A);
  CYC(b_+67, b_+70); W8(w4WaitingForNextByte) = A;
  CYC(b_+70, b_+73); W8(w4d984) = A;
  CYC(b_+73, b_+75); mem_wr(gb, IO_SB, A);
  CYC(b_+75, b_+76); ret_effect(gb); return;

get_next_byte:
  CYC(b_+76, b_+77); A = B;
  CYC(b_+77, b_+80); SET_DE(w4PacketBuffer);
  CALL_C(b_+80, addAToDe_hook, 0x0068, b_+83);
  CYC(b_+83, b_+84); A = B;
  CYC(b_+84, b_+85); A = alu_inc8(gb, A);
  CYC(b_+85, b_+88); W8(w4PacketByteIndex) = A;
  CYC(b_+88, b_+90); A = H8(hSerialByte);
  CYC(b_+90, b_+91); mem_wr(gb, DE, A);
  CYC(b_+91, b_+94); SET_HL(w4PacketChecksum);
  CYC(b_+94, b_+95); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+95, b_+96); mem_wr(gb, HL, A);
  CYC(b_+96, b_+97); alu_xor(gb, A);
  CYC(b_+97, b_+99); mem_wr(gb, IO_SB, A);
  CYC(b_+99, b_+102); W8(w4d984) = A;
  CYC(b_+102, b_+103); ret_effect(gb);
}

void FFBE_04_hook(GB *gb) {
  BASE(FFBE_04);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = H8(hSerialLinkState);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  do { uint16_t jt_ = (serial_jump_table(gb));
    if (jt_ == SYM(gameLink_getFile1) && hook_is(gb, SYM(gameLink_getFile1), gameLink_getFile1_hook)) { gameLink_getFile1_hook(gb); return; }
    else if (jt_ == SYM(waitForNextPacket) && hook_is(gb, SYM(waitForNextPacket), waitForNextPacket_hook)) { waitForNextPacket_hook(gb); return; }
    else if (jt_ == SYM(gameLink_getFile2) && hook_is(gb, SYM(gameLink_getFile2), gameLink_getFile2_hook)) { gameLink_getFile2_hook(gb); return; }
    else if (jt_ == SYM(gameLink_getFile3) && hook_is(gb, SYM(gameLink_getFile3), gameLink_getFile3_hook)) { gameLink_getFile3_hook(gb); return; }
    else if (jt_ == SYM(func_438e) && hook_is(gb, SYM(func_438e), func_438e_hook)) { func_438e_hook(gb); return; }
    else if (jt_ == SYM(func_4087) && hook_is(gb, SYM(func_4087), func_4087_hook)) { func_4087_hook(gb); return; }
    else if (jt_ == SYM(gameLinkState08) && hook_is(gb, SYM(gameLinkState08), gameLinkState08_hook)) { gameLinkState08_hook(gb); return; }
    else if (jt_ == SYM(gameLinkState09) && hook_is(gb, SYM(gameLinkState09), gameLinkState09_hook)) { gameLinkState09_hook(gb); return; }
    else if (jt_ == SYM(gameLinkState0b) && hook_is(gb, SYM(gameLinkState0b), gameLinkState0b_hook)) { gameLinkState0b_hook(gb); return; }
    else if (jt_ == SYM(gameLinkState0c) && hook_is(gb, SYM(gameLinkState0c), gameLinkState0c_hook)) { gameLinkState0c_hook(gb); return; }
    else if (jt_ == SYM(gameLinkState0f) && hook_is(gb, SYM(gameLinkState0f), gameLinkState0f_hook)) { gameLinkState0f_hook(gb); return; }
    else if (jt_ == SYM(func_4096) && hook_is(gb, SYM(func_4096), func_4096_hook)) { func_4096_hook(gb); return; }
    else if (jt_ == SYM(gameLinkState12) && hook_is(gb, SYM(gameLinkState12), gameLinkState12_hook)) { gameLinkState12_hook(gb); return; }
    else if (jt_ == SYM(sendAckPacket) && hook_is(gb, SYM(sendAckPacket), sendAckPacket_hook)) { sendAckPacket_hook(gb); return; }
    else if (jt_ == SYM(func_437b) && hook_is(gb, SYM(func_437b), func_437b_hook)) { func_437b_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void FFBE_03_hook(GB *gb) {
  BASE(FFBE_03);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = H8(hSerialLinkState);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  do { uint16_t jt_ = (serial_jump_table(gb));
    if (jt_ == SYM(receiveLinkState00) && hook_is(gb, SYM(receiveLinkState00), receiveLinkState00_hook)) { receiveLinkState00_hook(gb); return; }
    else if (jt_ == SYM(waitForNextPacket) && hook_is(gb, SYM(waitForNextPacket), waitForNextPacket_hook)) { waitForNextPacket_hook(gb); return; }
    else if (jt_ == SYM(func_438e) && hook_is(gb, SYM(func_438e), func_438e_hook)) { func_438e_hook(gb); return; }
    else if (jt_ == SYM(receiveLinkState03) && hook_is(gb, SYM(receiveLinkState03), receiveLinkState03_hook)) { receiveLinkState03_hook(gb); return; }
    else if (jt_ == SYM(receiveLinkState06) && hook_is(gb, SYM(receiveLinkState06), receiveLinkState06_hook)) { receiveLinkState06_hook(gb); return; }
    else if (jt_ == SYM(sendAckPacket) && hook_is(gb, SYM(sendAckPacket), sendAckPacket_hook)) { sendAckPacket_hook(gb); return; }
    else if (jt_ == SYM(receiveLinkState0b) && hook_is(gb, SYM(receiveLinkState0b), receiveLinkState0b_hook)) { receiveLinkState0b_hook(gb); return; }
    else if (jt_ == SYM(disableSerialIfByteReceived) && hook_is(gb, SYM(disableSerialIfByteReceived), disableSerialIfByteReceived_hook)) { disableSerialIfByteReceived_hook(gb); return; }
    else if (jt_ == SYM(func_4096) && hook_is(gb, SYM(func_4096), func_4096_hook)) { func_4096_hook(gb); return; }
    else if (jt_ == SYM(receiveLinkState10) && hook_is(gb, SYM(receiveLinkState10), receiveLinkState10_hook)) { receiveLinkState10_hook(gb); return; }
    else if (jt_ == SYM(receiveLinkState13) && hook_is(gb, SYM(receiveLinkState13), receiveLinkState13_hook)) { receiveLinkState13_hook(gb); return; }
    else if (jt_ == SYM(func_437b) && hook_is(gb, SYM(func_437b), func_437b_hook)) { func_437b_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

static void receive_link_state_common(GB *gb, uint16_t sp0_) {
  BASE(receiveLinkState06);
  CYC(b_+2, b_+4); H8(hActiveFileSlot) = A;
  CALL_C(b_+4, loadFile_b00_hook, SYM(loadFile_b00), b_+7);
  CYC(b_+7, b_+9); H8(hFF8B) = A;
  sendFileHeader_hook(gb);
}

void receiveLinkState00_hook(GB *gb) {
  BASE(receiveLinkState00);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3);
  receive_link_state_common(gb, sp0_);
}

void receiveLinkState03_hook(GB *gb) {
  BASE(receiveLinkState03);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+4);
  receive_link_state_common(gb, sp0_);
}

void receiveLinkState06_hook(GB *gb) {
  BASE(receiveLinkState06);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x02;
  receive_link_state_common(gb, sp0_);
}

void sendFileHeader_hook(GB *gb) {
  BASE(sendFileHeader);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, prepareForNextPacket_hook, SYM(prepareForNextPacket), b_+3);
  CYC(b_+3, b_+6); SET_HL(w4PacketBuffer);
  CYC(b_+6, b_+8); A = 0x21;
  CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+9, b_+10); C = A;
  CYC(b_+10, b_+12); A = H8(hFF8B);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+13, b_+14); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+14, b_+15); alu_add(gb, A);
  CYC(b_+15, b_+16); alu_add(gb, C);
  CYC(b_+16, b_+17); C = A;
  CYC(b_+17, b_+20); A = W8(wLinkMaxHealth);
  CYC(b_+20, b_+21); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+21, b_+22); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+22, b_+23); alu_add(gb, A);
  CYC(b_+23, b_+24); alu_add(gb, C);
  CYC(b_+24, b_+25); C = A;
  CYC(b_+25, b_+28); A = W8(wDeathCounter);
  CYC(b_+28, b_+29); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+29, b_+30); alu_add(gb, C);
  CYC(b_+30, b_+31); C = A;
  CYC(b_+31, b_+34); A = WP(wDeathCounter)[1];
  CYC(b_+34, b_+35); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+35, b_+36); alu_add(gb, C);
  CYC(b_+36, b_+37); C = A;
  CYC(b_+37, b_+40); A = W8(wFileIsLinkedGame);
  CYC(b_+40, b_+41); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+41, b_+42); alu_add(gb, C);
  CYC(b_+42, b_+43); C = A;
  CYC(b_+43, b_+46); A = W8(wFileIsHeroGame);
  CYC(b_+46, b_+47); alu_add(gb, A);
  CYC(b_+47, b_+48); E = A;
  CYC(b_+48, b_+51); A = W8(wFileIsCompleted);
  CYC(b_+51, b_+52); alu_or(gb, E);
  CYC(b_+52, b_+53); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+53, b_+54); alu_add(gb, C);
  CYC(b_+54, b_+55); C = A;
  CYC(b_+55, b_+58); SET_DE(wGameID);
  CYC(b_+58, b_+60); B = 0x16;

copy_file_header:
  CYC(b_+60, b_+61); A = mem_rd(gb, DE);
  CYC(b_+61, b_+62); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+62, b_+63); alu_add(gb, C);
  CYC(b_+63, b_+64); C = A;
  CYC(b_+64, b_+65); E = alu_inc8(gb, E);
  CYC(b_+65, b_+66); B = alu_dec8(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+66, b_+68);
    goto copy_file_header;
  }
  CYC(b_+66, b_+68);
  CYC(b_+68, b_+70); A = GV(0xa1, 0xa0);
  CYC(b_+70, b_+71); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+71, b_+72); alu_add(gb, C);
  CYC(b_+72, b_+73); C = A;
  CYC(b_+73, b_+75); A = H8(hActiveFileSlot);
  CYC(b_+75, b_+76); mem_wr(gb, HL, A);
  CYC(b_+76, b_+77); alu_add(gb, C);
  CYC(b_+77, b_+78); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+78, b_+80); A = 0x01;
  CYC(b_+80, b_+83); W8(w4WaitingForNextByte) = A;
  CYC(b_+83, b_+86); TAIL(sendPacketByte);
}

void waitForSerialByte_hook(GB *gb) {
  BASE(waitForSerialByte);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = H8(hReceivedSerialByte);
  CYC(b_+2, b_+3); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto byte_received; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+8); A = W8(w4DisableLinkTimeout);
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+9, b_+11); goto return_from_caller; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+14); SET_HL(w4FileLinkTimer);
  CALL_C(b_+14, decHlRef16WithCap_hook, SYM(decHlRef16WithCap), b_+17);
  if (F & FZ) { CYCT(b_+17, b_+19); goto timeout; }
  CYC(b_+17, b_+19);

return_from_caller:
  CYC(b_+19, b_+20); SET_AF(pop_effect(gb));
  CYC(b_+20, b_+21); ret_effect(gb); return;

timeout:
  CYC(b_+21, b_+22); alu_xor(gb, A);
  CYC(b_+22, b_+25); W8(w4WaitingForNextByte) = A;
  CYC(b_+25, b_+27); A = 0x80;
  CYC(b_+27, b_+29); H8(hFFBD) = A;
  CYC(b_+29, b_+30); ret_effect(gb); return;

byte_received:
  CYC(b_+30, b_+33); W8(w4WaitingForNextByte) = A;
  CYC(b_+33, b_+34); alu_xor(gb, A);
  CYC(b_+34, b_+36); H8(hReceivedSerialByte) = A;
  CYC(b_+36, b_+38); H8(hFFBD) = A;
  TAIL(setLinkTimerTo180);
}

void setLinkTimerTo180_hook(GB *gb) {
  BASE(setLinkTimerTo180);
  CYC(b_+0, b_+2); A = 0xb4;
  CYC(b_+2, b_+5); W8(w4FileLinkTimer) = A;
  CYC(b_+5, b_+7); A = 0x00;
  CYC(b_+7, b_+10); WP(w4FileLinkTimer)[1] = A;
  CYC(b_+10, b_+11); ret_effect(gb);
}

void FFBE_00_hook(GB *gb) {
  BASE(FFBE_00);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = H8(hSerialLinkState);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  do { uint16_t jt_ = (serial_jump_table(gb));
    if (jt_ == SYM(sendFileHeader) && hook_is(gb, SYM(sendFileHeader), sendFileHeader_hook)) { sendFileHeader_hook(gb); return; }
    else if (jt_ == SYM(waitForNextPacket) && hook_is(gb, SYM(waitForNextPacket), waitForNextPacket_hook)) { waitForNextPacket_hook(gb); return; }
    else if (jt_ == SYM(func_438e) && hook_is(gb, SYM(func_438e), func_438e_hook)) { func_438e_hook(gb); return; }
    else if (jt_ == SYM(func_4293) && hook_is(gb, SYM(func_4293), func_4293_hook)) { func_4293_hook(gb); return; }
    else if (jt_ == SYM(func_4096) && hook_is(gb, SYM(func_4096), func_4096_hook)) { func_4096_hook(gb); return; }
    else if (jt_ == SYM(determineRingFortuneRing) && hook_is(gb, SYM(determineRingFortuneRing), determineRingFortuneRing_hook)) { determineRingFortuneRing_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void FFBE_02_hook(GB *gb) {
  BASE(FFBE_02);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = H8(hSerialLinkState);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  do { uint16_t jt_ = (serial_jump_table(gb));
    if (jt_ == SYM(func_4293) && hook_is(gb, SYM(func_4293), func_4293_hook)) { func_4293_hook(gb); return; }
    else if (jt_ == SYM(waitForNextPacket) && hook_is(gb, SYM(waitForNextPacket), waitForNextPacket_hook)) { waitForNextPacket_hook(gb); return; }
    else if (jt_ == SYM(func_4096) && hook_is(gb, SYM(func_4096), func_4096_hook)) { func_4096_hook(gb); return; }
    else if (jt_ == SYM(sendFileHeader) && hook_is(gb, SYM(sendFileHeader), sendFileHeader_hook)) { sendFileHeader_hook(gb); return; }
    else if (jt_ == SYM(func_438e) && hook_is(gb, SYM(func_438e), func_438e_hook)) { func_438e_hook(gb); return; }
    else if (jt_ == SYM(determineRingFortuneRing) && hook_is(gb, SYM(determineRingFortuneRing), determineRingFortuneRing_hook)) { determineRingFortuneRing_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void determineRingFortuneRing_hook(GB *gb) {
  BASE(determineRingFortuneRing);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, disableSerialPort_hook, SYM(disableSerialPort), b_+3);
  CYC(b_+3, b_+4); alu_xor(gb, A);
  CYC(b_+4, b_+6); H8(hFFBD) = A;
  CALL_C(b_+6, compareFileHeader_hook, SYM(compareFileHeader), b_+9);
  if (F & FZ) {
    CYCT(b_+9, b_+11);
    goto no_match;
  }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+14); SET_HL(wGameID);
  CYC(b_+14, b_+17); A = W8(w4RingFortuneStuff);
  CYC(b_+17, b_+18); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+18, b_+20); alu_and(gb, 0x7f);
  CYC(b_+20, b_+22); B = 0x00;
  CYC(b_+22, b_+24); alu_and(gb, 0x7c);
  if (F & FZ) {
    CYCT(b_+24, b_+26);
    goto choose_ring;
  }
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+27); B = alu_inc8(gb, B);
  CYC(b_+27, b_+29); alu_and(gb, 0x60);
  if (F & FZ) {
    CYCT(b_+29, b_+31);
    goto choose_ring;
  }
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+32); B = alu_inc8(gb, B);

choose_ring:
  CYC(b_+32, b_+33); L = alu_inc8(gb, L);
  CYC(b_+33, b_+34); C = mem_rd(gb, HL);
  CYC(b_+34, b_+35); A = B;
  CYC(b_+35, b_+38); SET_HL(ringFortuneTable_bank16);
  CYC(b_+38, b_+39); serial_add_a_to_hl(gb, b_+39);
  CYC(b_+39, b_+40); A = mem_rd(gb, HL);
  CYC(b_+40, b_+41); serial_add_a_to_hl(gb, b_+41);
  CYC(b_+41, b_+44); A = mem_rd(gb, w4RingFortuneStuff + 1);
  CYC(b_+44, b_+45); alu_add(gb, C);
  CYC(b_+45, b_+47); alu_and(gb, 0x07);
  CYC(b_+47, b_+48); serial_add_a_to_hl(gb, b_+48);
  CYC(b_+48, b_+49); A = mem_rd(gb, HL);
  CYC(b_+49, b_+52); W8(w4RingFortuneStuff) = A;
  CYC(b_+52, b_+53); ret_effect(gb); return;

no_match:
  CYC(b_+53, b_+55); A = 0x84;
  CYC(b_+55, b_+57); H8(hFFBD) = A;
  CYC(b_+57, b_+58); ret_effect(gb);
}

void prepareForNextPacket_hook(GB *gb) {
  BASE(prepareForNextPacket);
  CYC(b_+0, b_+2); A = H8(hSerialLinkState);
  CYC(b_+2, b_+3); A = alu_inc8(gb, A);
  CYC(b_+3, b_+5); H8(hSerialLinkState) = A;
  TAIL(func_426e);
}

void func_426e_hook(GB *gb) {
  BASE(func_426e);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); W8(w4PacketByteIndex) = A;
  CYC(b_+4, b_+6); H8(hFFBD) = A;
  CYC(b_+6, b_+9); W8(w4PacketChecksum) = A;
  CYC(b_+9, b_+12); W8(w4d984) = A;
  CYC(b_+12, b_+13); A = alu_inc8(gb, A);
  CYC(b_+13, b_+16); W8(w4WaitingForNextByte) = A;
  CYC(b_+16, b_+18);
  TAIL(setLinkTimerTo180);
}

void waitForNextPacket_hook(GB *gb) {
  BASE(waitForNextPacket);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, func_4043_hook, SYM(func_4043), b_+3);
  CALL_C(b_+3, returnIfPacketNotComplete_hook, SYM(returnIfPacketNotComplete), b_+6);
  CYC(b_+6, b_+9); A = W8(w4LinkRetryCounter);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+10, b_+12);
    TAIL(prepareForNextPacket);
  }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); A = H8(hSerialLinkState);
  CYC(b_+14, b_+15); A = alu_dec8(gb, A);
  CYC(b_+15, b_+17); H8(hSerialLinkState) = A;
  CYC(b_+17, b_+19);
  TAIL(func_426e);
}

void func_4293_hook(GB *gb) {
  BASE(func_4293);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, receivePacketByte_hook, SYM(receivePacketByte), b_+3);
  CALL_C(b_+3, returnIfPacketNotComplete_hook, SYM(returnIfPacketNotComplete), b_+6);
  CYC(b_+6, b_+9); SET_HL(w4RingFortuneStuff);
  CYC(b_+9, b_+12); SET_DE(w4PacketBuffer + 9);
  CYC(b_+12, b_+14); B = 0x07;
  CALL_C(b_+14, copyMemoryReverse_hook, SYM(copyMemoryReverse), b_+17);
  CYC(b_+17, b_+20);
  TAIL(sendAckPacket);
}

void receiveLinkState0b_hook(GB *gb) {
  BASE(receiveLinkState0b);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(w4PacketByteIndex);
  CYC(b_+3, b_+4); alu_or(gb, A);
  CYC(b_+4, b_+6); A = 0x00;
  if (!(F & FZ)) {
    CYCT(b_+6, b_+8);
    goto set_timeout_mode;
  }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); A = alu_inc8(gb, A);

set_timeout_mode:
  CYC(b_+9, b_+12); W8(w4DisableLinkTimeout) = A;
  CALL_C(b_+12, receivePacketByte_hook, SYM(receivePacketByte), b_+15);
  CYC(b_+15, b_+18); A = W8(w4WaitingForNextByte);
  CYC(b_+18, b_+19); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+19, b_+20); ret_effect(gb); return; }
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+23); A = mem_rd(gb, w4PacketBuffer + 1);
  CYC(b_+23, b_+25); alu_cp(gb, 0xc0);
  if (!(F & FZ)) {
    CYCT(b_+25, b_+27);
    TAIL(func_42c5);
  }
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+30); TAIL(sendAckPacket);
}

void func_42c5_hook(GB *gb) {
  BASE(func_42c5);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); alu_cp(gb, 0xb0);
  if (!(F & FZ)) {
    CYCT(b_+2, b_+5);
    TAIL(sendRetryPacket);
  }
  CYC(b_+2, b_+5);
  CYC(b_+5, b_+8); A = mem_rd(gb, w4PacketBuffer + 2);
  CYC(b_+8, b_+10); H8(hActiveFileSlot) = A;
  CYC(b_+10, b_+12); alu_cp(gb, 0x03);
  if (!(F & FC)) {
    CYCT(b_+12, b_+15);
    TAIL(disableSerialPort);
  }
  CYC(b_+12, b_+15);
  CALL_C(b_+15, loadFile_b00_hook, SYM(loadFile_b00), b_+18);
  CYC(b_+18, b_+20); A = 0x0d;
  CYC(b_+20, b_+22); H8(hSerialLinkState) = A;
  CYC(b_+22, b_+25); TAIL(sendAckPacket);
}

void receiveLinkState10_hook(GB *gb) {
  BASE(receiveLinkState10);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, prepareForNextPacket_hook, SYM(prepareForNextPacket), b_+3);
  CYC(b_+3, b_+6); SET_HL(w4RingFortuneStuff);
  CYC(b_+6, b_+9); SET_DE(wRingsObtained);
  CYC(b_+9, b_+11); B = 0x08;
  CALL_C(b_+11, copyMemoryReverse_hook, SYM(copyMemoryReverse), b_+14);
  CYC(b_+14, b_+16); TAIL(func_4350);
}

void gameLinkState08_hook(GB *gb) {
  BASE(gameLinkState08);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, prepareForNextPacket_hook, SYM(prepareForNextPacket), b_+3);
  CYC(b_+3, b_+6); SET_HL(w4PacketBuffer);
  CYC(b_+6, b_+8); A = 0x03;
  CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+9, b_+11); A = 0xc0;
  CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+12, b_+14); A = 0xc3;
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CYC(b_+15, b_+17); A = 0x01;
  CYC(b_+17, b_+20); W8(w4WaitingForNextByte) = A;
  CYC(b_+20, b_+23); TAIL(sendPacketByte);
}

void gameLinkState09_hook(GB *gb) {
  BASE(gameLinkState09);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, func_4043_hook, SYM(func_4043), b_+3);
  CALL_C(b_+3, returnIfPacketNotComplete_hook, SYM(returnIfPacketNotComplete), b_+6);
  CYC(b_+6, b_+9); TAIL(prepareForNextPacket);
}

void receiveLinkState13_hook(GB *gb) {
  BASE(receiveLinkState13);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, receivePacketByte_hook, SYM(receivePacketByte), b_+3);
  CALL_C(b_+3, returnIfPacketNotComplete_hook, SYM(returnIfPacketNotComplete), b_+6);
  CYC(b_+6, b_+8); A = H8(hFFBD);
  CYC(b_+8, b_+10); alu_cp(gb, 0x81);
  if (F & FZ) {
    CYCT(b_+10, b_+13);
    TAIL(prepareForNextPacket);
  }
  CYC(b_+10, b_+13);
  CYC(b_+13, b_+16); SET_HL(w4RingFortuneStuff);
  CYC(b_+16, b_+19); SET_DE(w4PacketBuffer + 1);
  CYC(b_+19, b_+21); B = 0x08;
  CALL_C(b_+21, copyMemoryReverse_hook, SYM(copyMemoryReverse), b_+24);
  CYC(b_+24, b_+27); TAIL(prepareForNextPacket);
}

void gameLinkState0f_hook(GB *gb) {
  BASE(gameLinkState0f);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, receivePacketByte_hook, SYM(receivePacketByte), b_+3);
  CALL_C(b_+3, returnIfPacketNotComplete_hook, SYM(returnIfPacketNotComplete), b_+6);
  CYC(b_+6, b_+9); SET_HL(wRingsObtained);
  CYC(b_+9, b_+12); SET_DE(w4PacketBuffer + 1);
  CYC(b_+12, b_+14); B = 0x08;

merge_rings:
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CYC(b_+17, b_+18); SET_HL(HL + 1);
  CYC(b_+18, b_+19); SET_DE(DE + 1);
  CYC(b_+19, b_+20); B = alu_dec8(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+20, b_+22);
    goto merge_rings;
  }
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+25); SET_HL(w4RingFortuneStuff);
  CYC(b_+25, b_+28); SET_DE(w4PacketBuffer + 1);
  CYC(b_+28, b_+30); B = 0x08;
  CALL_C(b_+30, copyMemoryReverse_hook, SYM(copyMemoryReverse), b_+33);
  CYC(b_+33, b_+36); TAIL(sendAckPacket);
}

void gameLinkState12_hook(GB *gb) {
  BASE(gameLinkState12);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, prepareForNextPacket_hook, SYM(prepareForNextPacket), SYM(func_4350));
  TAIL(func_4350);
}

void func_4350_hook(GB *gb) {
  BASE(func_4350);
  CYC(b_+0, b_+2); A = 0x0a;
  CYC(b_+2, b_+3); C = A;
  CYC(b_+3, b_+6); W8(w4PacketBuffer) = A;
  CYC(b_+6, b_+9); SET_DE(w4PacketBuffer + 1);
  CYC(b_+9, b_+12); SET_HL(w4RingFortuneStuff);
  CYC(b_+12, b_+14); B = 0x08;

copy_ring_fortune:
  CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CYC(b_+16, b_+17); SET_DE(DE + 1);
  CYC(b_+17, b_+18); alu_add(gb, C);
  CYC(b_+18, b_+19); C = A;
  CYC(b_+19, b_+20); B = alu_dec8(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+20, b_+22);
    goto copy_ring_fortune;
  }
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+23); A = C;
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  CYC(b_+24, b_+26); A = 0x01;
  CYC(b_+26, b_+29); W8(w4WaitingForNextByte) = A;
  CYC(b_+29, b_+32); TAIL(sendPacketByte);
}

void gameLinkState0b_hook(GB *gb) {
  BASE(gameLinkState0b);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, waitForSerialByte_hook, SYM(waitForSerialByte), b_+3);
  CYC(b_+3, b_+5); alu_cp(gb, 0x80);
  if (F & FZ) {
    CYCT(b_+5, b_+8);
    TAIL(disableSerialPort);
  }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+11); TAIL(disableSerialPort);
}

void func_437b_hook(GB *gb) {
  BASE(func_437b);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, disableSerialPort_hook, SYM(disableSerialPort), b_+3);
  CYC(b_+3, b_+5); H8(hFFBD) = A;
  CYC(b_+5, b_+8); SET_DE(w4RingFortuneStuff);
  CYC(b_+8, b_+11); SET_HL(wRingsObtained);
  CYC(b_+11, b_+13); B = 0x08;
  CALL_C(b_+13, copyMemoryReverse_hook, SYM(copyMemoryReverse), b_+16);
  CYC(b_+16, b_+19); TAIL(saveFile_b00);
}

void func_438e_hook(GB *gb) {
  BASE(func_438e);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, func_439a_hook, SYM(func_439a), b_+3);
  CALL_C(b_+3, returnIfPacketNotComplete_hook, SYM(returnIfPacketNotComplete), b_+6);
  CALL_C(b_+6, prepareForNextPacket_hook, SYM(prepareForNextPacket), b_+9);
  CYC(b_+9, b_+12); TAIL(func_4036);
}

void func_439a_hook(GB *gb) {
  BASE(func_439a);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, receivePacketByte_hook, SYM(receivePacketByte), b_+3);
  CYC(b_+3, b_+6); A = W8(w4WaitingForNextByte);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+7, b_+8); ret_effect(gb); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); A = H8(hFFBD);
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+11, b_+13);
    TAIL(func_43ab);
  }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); SET_AF(pop_effect(gb));
  CYC(b_+14, b_+17); TAIL(disableSerialPort);
}

void func_43ab_hook(GB *gb) {
  BASE(func_43ab);
  CYC(b_+0, b_+3); A = mem_rd(gb, w4PacketBuffer + 1);
  CYC(b_+3, b_+5); alu_cp(gb, 0xb1);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7);
    TAIL(func_43bd);
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); alu_xor(gb, A);
  CYC(b_+8, b_+11); W8(w4LinkRetryCounter) = A;
  CYC(b_+11, b_+13); A = H8(hSerialLinkState);
  CYC(b_+13, b_+15); alu_sub(gb, 0x02);
  CYC(b_+15, b_+17); H8(hSerialLinkState) = A;
  CYC(b_+17, b_+18); ret_effect(gb);
}

void func_43bd_hook(GB *gb) {
  BASE(func_43bd);
  CYC(b_+0, b_+2); alu_cp(gb, 0xb0);
  if (F & FZ) { CYCT(b_+2, b_+3); ret_effect(gb); return; }
  CYC(b_+2, b_+3);
  CYC(b_+3, b_+5); A = 0x82;
  CYC(b_+5, b_+7); H8(hFFBD) = A;
  CYC(b_+7, b_+8); ret_effect(gb);
}

void gameLinkState0c_hook(GB *gb) {
  BASE(gameLinkState0c);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, prepareForNextPacket_hook, SYM(prepareForNextPacket), b_+3);
  CYC(b_+3, b_+6); SET_HL(w4PacketBuffer);
  CYC(b_+6, b_+8); A = 0x04;
  CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+9, b_+11); A = 0xb0;
  CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+12, b_+15); A = W8(wFileSelect_cursorPos);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+16, b_+18); alu_add(gb, 0xb4);
  CYC(b_+18, b_+19); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+19, b_+21); A = 0x01;
  CYC(b_+21, b_+24); W8(w4WaitingForNextByte) = A;
  CYC(b_+24, b_+27); TAIL(sendPacketByte);
}

void sendRetryPacket_hook(GB *gb) {
  BASE(sendRetryPacket);
  CYC(b_+0, b_+3); SET_HL(retryPacket_bank16);
  CYC(b_+3, b_+6); A = W8(w4LinkRetryCounter);
  CYC(b_+6, b_+7); A = alu_inc8(gb, A);
  CYC(b_+7, b_+10); W8(w4LinkRetryCounter) = A;
  CYC(b_+10, b_+12); alu_cp(gb, 0x05);
  if (F & FC) {
    CYCT(b_+12, b_+14);
    TAIL(setPacketBuffer);
  }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+16); A = 0x80;
  CYC(b_+16, b_+18); H8(hFFBD) = A;
  CYC(b_+18, b_+21); TAIL(disableSerialPort);
}

void sendAckPacket_hook(GB *gb) {
  BASE(sendAckPacket);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); W8(w4LinkRetryCounter) = A;
  CYC(b_+4, b_+7); SET_HL(ackPacket_bank16);
  TAIL(setPacketBuffer);
}

void setPacketBuffer_hook(GB *gb) {
  BASE(setPacketBuffer);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, prepareForNextPacket_hook, SYM(prepareForNextPacket), b_+3);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); B = A;
  CYC(b_+5, b_+8); SET_DE(w4PacketBuffer);

copy_packet:
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  CYC(b_+10, b_+11); SET_DE(DE + 1);
  CYC(b_+11, b_+12); B = alu_dec8(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+12, b_+14);
    goto copy_packet;
  }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+17); TAIL(sendPacketByte);
}

static void game_link_get_file_body(GB *gb, uint16_t sp0_) {
  BASE(gameLink_getFile3);
  CYC(b_+2, b_+4); H8(hFF8B) = A;
  CALL_C(b_+4, receivePacketByte_hook, SYM(receivePacketByte), b_+7);
  CALL_C(b_+7, returnIfPacketNotComplete_hook, SYM(returnIfPacketNotComplete), b_+10);
  CYC(b_+10, b_+12); A = H8(hFF8B);
  CYC(b_+12, b_+15); SET_HL(w2ColorComponentBuffer1 + 5);
  if (!(F & FZ)) {
    CYCT(b_+15, b_+17);
    TAIL(sendRetryPacket);
  }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); A = alu_swap(gb, A);
  CYC(b_+19, b_+20); alu_rrca(gb);
  CYC(b_+20, b_+23); SET_HL(w4FileDisplayVariables);
  CYC(b_+23, b_+24); serial_add_a_to_hl(gb, b_+24);
  CYC(b_+24, b_+27); SET_DE(w4PacketBuffer + 1);
  CYC(b_+27, b_+29); B = 0x08;

copy_file_status:
  CYC(b_+29, b_+30); A = mem_rd(gb, DE);
  CYC(b_+30, b_+31); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+31, b_+32); SET_DE(DE + 1);
  CYC(b_+32, b_+33); B = alu_dec8(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+33, b_+35);
    goto copy_file_status;
  }
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+37); A = H8(hFF8B);
  CYC(b_+37, b_+38); alu_add(gb, A);
  CYC(b_+38, b_+39); E = A;
  CYC(b_+39, b_+40); alu_add(gb, A);
  CYC(b_+40, b_+41); alu_add(gb, E);
  CYC(b_+41, b_+44); SET_HL(w4NameBuffer);
  CYC(b_+44, b_+45); serial_add_a_to_hl(gb, b_+45);
  CYC(b_+45, b_+48); SET_DE(w4PacketBuffer + 0x0b);
  CYC(b_+48, b_+50); B = 0x06;
  CALL_C(b_+50, copyMemoryReverse_hook, SYM(copyMemoryReverse), b_+53);
  CYC(b_+53, b_+55); A = H8(hFF8B);
  CYC(b_+55, b_+56); A = alu_inc8(gb, A);
  CYC(b_+56, b_+59); SET_HL(w4RingFortuneStuff);
  CYC(b_+59, b_+62); SET_BC(0x0016);

seek_file_header:
  CYC(b_+62, b_+63); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(b_+63, b_+65);
    goto copy_file_header;
  }
  CYC(b_+63, b_+65);
  CYC(b_+65, b_+66); alu_add_hl(gb, BC);
  CYC(b_+66, b_+68);
  goto seek_file_header;

copy_file_header:
  CYC(b_+68, b_+70); B = 0x16;
  CYC(b_+70, b_+73); SET_DE(w4PacketBuffer + 9);
  CALL_C(b_+73, copyMemoryReverse_hook, SYM(copyMemoryReverse), b_+76);
  CYC(b_+76, b_+79); A = W8(wOpenedMenuType);
  CYC(b_+79, b_+81); alu_cp(gb, 0x08);
  if (!(F & FZ)) {
    CYCT(b_+81, b_+83);
    goto verify_other_mode;
  }
  CYC(b_+81, b_+83);
  CYC(b_+83, b_+86); SET_DE(w4PacketBuffer + 9);
  CALL_C(b_+86, compareFileIDsAndNames_hook, SYM(compareFileIDsAndNames), b_+89);
  if (!(F & FZ)) {
    CYCT(b_+89, b_+91);
    TAIL(markFileAsBlank);
  }
  CYC(b_+89, b_+91);
  CYC(b_+91, b_+94); SET_HL(w2ColorComponentBuffer1);
  CYC(b_+94, b_+95); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+95, b_+96); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+96, b_+97); L = alu_inc8(gb, L);
  CYC(b_+97, b_+98); alu_or(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+98, b_+100);
    TAIL(markFileAsBlank);
  }
  CYC(b_+98, b_+100);
  CYC(b_+100, b_+103); sendAckPacket_hook(gb);
  return;

verify_other_mode:
  CYC(b_+103, b_+106); A = mem_rd(gb, w2ColorComponentBuffer1 + 4);
  CYC(b_+106, b_+108); alu_cp(gb, GV(0xa0, 0xa1));
  if (!(F & FZ)) {
    CYCT(b_+108, b_+110);
    TAIL(markFileAsBlank);
  }
  CYC(b_+108, b_+110);
  CYC(b_+110, b_+113); A = mem_rd(gb, w2ColorComponentBuffer1 + 2);
  CYC(b_+113, b_+114); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+114, b_+116);
    TAIL(markFileAsBlank);
  }
  CYC(b_+114, b_+116);
  CYC(b_+116, b_+119); sendAckPacket_hook(gb);
}

void gameLink_getFile1_hook(GB *gb) {
  BASE(gameLink_getFile1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x00;
  CYC(b_+2, b_+4);
  game_link_get_file_body(gb, sp0_);
}

void gameLink_getFile2_hook(GB *gb) {
  BASE(gameLink_getFile2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+4);
  game_link_get_file_body(gb, sp0_);
}

void gameLink_getFile3_hook(GB *gb) {
  BASE(gameLink_getFile3);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x02;
  game_link_get_file_body(gb, sp0_);
}

void markFileAsBlank_hook(GB *gb) {
  BASE(markFileAsBlank);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = H8(hFF8B);
  CYC(b_+2, b_+4); D = 0x00;
  CYC(b_+4, b_+6); A = alu_swap(gb, A);
  CYC(b_+6, b_+7); alu_rrca(gb);
  CYC(b_+7, b_+8); alu_add(gb, D);
  CYC(b_+8, b_+11); SET_HL(w4FileDisplayVariables);
  CYC(b_+11, b_+12); serial_add_a_to_hl(gb, b_+12);
  CYC(b_+12, b_+14); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x80));
  CYC(b_+14, b_+16); A = H8(hFF8B);
  CYC(b_+16, b_+17); alu_add(gb, A);
  CYC(b_+17, b_+18); E = A;
  CYC(b_+18, b_+19); alu_add(gb, A);
  CYC(b_+19, b_+20); alu_add(gb, E);
  CYC(b_+20, b_+23); SET_HL(w4NameBuffer);
  CYC(b_+23, b_+24); serial_add_a_to_hl(gb, b_+24);
  CYC(b_+24, b_+26); B = 0x06;
  CALL_C(b_+26, clearMemory_hook, SYM(clearMemory), b_+29);
  CYC(b_+29, b_+32); TAIL(sendAckPacket);
}

void func_44ac_hook(GB *gb) {
  BASE(func_44ac);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = mem_rd(gb, IO_SVBK);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+5); A = 0x04;
  CYC(b_+5, b_+7); mem_wr(gb, IO_SVBK, A);
  CYC(b_+7, b_+8); alu_xor(gb, A);
  CYC(b_+8, b_+11); SET_HL(w4d980);
  CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+13, b_+14); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+17, b_+18); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+18, b_+20); H8(hFFBE) = A;
  CYC(b_+20, b_+22); H8(hSerialLinkState) = A;
  CYC(b_+22, b_+24); H8(hFFBD) = A;
  CALL_C(b_+24, setLinkTimerTo180_hook, SYM(setLinkTimerTo180), b_+27);
  CYC(b_+27, b_+29); A = 0xe1;
  CYC(b_+29, b_+31); mem_wr(gb, IO_SB, A);
  CYC(b_+31, b_+33); A = 0x80;
  CYC(b_+33, b_+36); W8(w4WaitingForNextByte) = A;
  CALL_C(b_+36, writeToSC_hook, SYM(writeToSC), b_+39);
  CYC(b_+39, b_+40); SET_AF(pop_effect(gb));
  CYC(b_+40, b_+42); mem_wr(gb, IO_SVBK, A);
  CYC(b_+42, b_+43); ret_effect(gb);
}

void returnIfPacketNotComplete_hook(GB *gb) {
  BASE(returnIfPacketNotComplete);
  CYC(b_+0, b_+3); A = W8(w4WaitingForNextByte);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto packet_complete; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); SET_AF(pop_effect(gb));
  CYC(b_+7, b_+8); ret_effect(gb); return;

packet_complete:
  CYC(b_+8, b_+10); A = H8(hFFBD);
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+11, b_+12); ret_effect(gb); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); alu_cp(gb, 0x81);
  if (F & FZ) {
    CYCT(b_+14, b_+17);
    TAIL(sendRetryPacket);
  }
  CYC(b_+14, b_+17);
  CYC(b_+17, b_+18); SET_AF(pop_effect(gb));
  CYC(b_+18, b_+21);
  TAIL(disableSerialPort);
}

void compareFileHeader_hook(GB *gb) {
  BASE(compareFileHeader);
  CYC(b_+0, b_+3); SET_DE(w4RingFortuneStuff);
  TAIL(compareFileIDsAndNames);
}

void compareFileIDsAndNames_hook(GB *gb) {
  BASE(compareFileIDsAndNames);
  CYC(b_+0, b_+3); SET_HL(wGameID);
  CYC(b_+3, b_+5); B = 0x07;

compare_byte:
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+7, b_+8); ret_effect(gb); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+9); SET_DE(DE + 1);
  CYC(b_+9, b_+10); L = alu_inc8(gb, L);
  CYC(b_+10, b_+11); B = alu_dec8(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+11, b_+13);
    goto compare_byte;
  }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); ret_effect(gb);
}
