#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(setChannelWaitCounter), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(setChannelWaitCounter), (from), (to), true)

void getNextChannelByte_hook(GB *gb);
void isWaveChannelUnavailable_hook(GB *gb);
void stopSfx_hook(GB *gb);
void silenceAllChannels_hook(GB *gb);
void doNextChannelCommand_hook(GB *gb);
void updatePlayedFrequency_hook(GB *gb);
void wMusicReadFunction_hook(GB *gb);

void setChannelWaitCounter_hook(GB *gb);
void getWaitTimeForEnvelope_hook(GB *gb);
void setSoundFrequency_hook(GB *gb);
void handleEnvelopes_hook(GB *gb);
void updateSquareChannelVolume_hook(GB *gb);
void updateChannel4Volume_hook(GB *gb);
void getChannelVolume_hook(GB *gb);
void standardCmdChannels4To5_hook(GB *gb);
void getWaveChannelVolume_hook(GB *gb);
void standardCmdChannel6_hook(GB *gb);
void standardCmdChannel7_hook(GB *gb);
void channelCmdff_hook(GB *gb);
void silencePlayedSound_hook(GB *gb);
void setWaveform_hook(GB *gb);
void channelCmdfe_hook(GB *gb);
void multiplyHlByA_hook(GB *gb);
void playSound_b39_hook(GB *gb);
void readWordFromTable_hook(GB *gb);
void writeIndexedHighRamAndIncrement_hook(GB *gb);

// Read a byte, set the channel wait counter to the value
void setChannelWaitCounter_hook(GB *gb) {
  BASE(setChannelWaitCounter);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getNextChannelByte_hook, SYM(getNextChannelByte), b_+3);
  CYC(b_+3, b_+4); A = alu_dec8(gb, A);
  CYC(b_+4, b_+7); SET_HL(wChannelWaitCounters);
  CYC(b_+7, b_+8); push_effect(gb, AF);
  CYC(b_+8, b_+11); A = W8(wSoundChannel);
  CYC(b_+11, b_+12); E = A;
  CYC(b_+12, b_+14); D = 0x00;
  CYC(b_+14, b_+15); alu_add_hl(gb, DE);
  CYC(b_+15, b_+16); SET_AF(pop_effect(gb));
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  RET(b_+17);
}

// Determines the time to wait until the envelope with sweep pace c is
// expected to have reached the volume level in b
void getWaitTimeForEnvelope_hook(GB *gb) {
  BASE(getWaitTimeForEnvelope);
  CYC(b_+0, b_+3); SET_HL(SYM(envelopeWaitTable));
  CYC(b_+3, b_+4); A = B;
  CYC(b_+4, b_+6); A = alu_sla(gb, A);
  CYC(b_+6, b_+8); A = alu_sla(gb, A);
  CYC(b_+8, b_+10); A = alu_sla(gb, A);
  CYC(b_+10, b_+11); alu_add(gb, C);
  CYC(b_+11, b_+13); D = 0x00;
  CYC(b_+13, b_+14); E = A;
  CYC(b_+14, b_+15); alu_add_hl(gb, DE);
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  RET(b_+16);
}

// Sends wSoundFrequency to given value plus value in table at wChannelPitchShift.
void setSoundFrequency_hook(GB *gb) {
  BASE(setSoundFrequency);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); push_effect(gb, HL);
  CYC(b_+1, b_+4); SET_HL(wChannelPitchShift);
  CYC(b_+4, b_+7); A = W8(wSoundChannel);
  CYC(b_+7, b_+8); E = A;
  CYC(b_+8, b_+10); D = 0x00;
  CYC(b_+10, b_+11); alu_add_hl(gb, DE);
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+13); D = A;
  CYC(b_+13, b_+15); D = alu_sla(gb, D);
  if (F & FC) { CYCT(b_+15, b_+17); goto negOffset; } // jr c
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); D = 0x00;
  CYC(b_+19, b_+21); goto haveOffset; // jr
negOffset:
  CYC(b_+21, b_+23); D = 0xff;
haveOffset:
  CYC(b_+23, b_+24); E = A;
  CYC(b_+24, b_+25); SET_HL(pop_effect(gb));
  CYC(b_+25, b_+26); alu_add_hl(gb, DE);
  CYC(b_+26, b_+29); A = W8(wSoundChannel);
  CYC(b_+29, b_+31); A = alu_sla(gb, A);
  CYC(b_+31, b_+32); B = A;
  CYC(b_+32, b_+33); A = L;
  CYC(b_+33, b_+35); C = 0xf2;
  CALL_C(b_+35, writeIndexedHighRamAndIncrement_hook, SYM(writeIndexedHighRamAndIncrement), b_+38);
  CYC(b_+38, b_+39); A = H;
  CYC(b_+39, b_+40); mem_wr(gb, 0xff00 | C, A);
  CYC(b_+40, b_+41); C = alu_inc8(gb, C);
  CYC(b_+41, b_+42); A = L;
  CYC(b_+42, b_+45); W8(wSoundFrequencyL) = A;
  CYC(b_+45, b_+46); A = H;
  CYC(b_+46, b_+49); W8(wSoundFrequencyH) = A;
  RET(b_+49);
}

// Handles envelopes for square channels, and redirects channel 4 to updateChannel4Volume
void handleEnvelopes_hook(GB *gb) {
  BASE(handleEnvelopes);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wSoundChannel);
  CYC(b_+3, b_+5); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto notChannel4; } // jr nz
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); updateChannel4Volume_hook(gb); return; // jp
notChannel4:
  CYC(b_+10, b_+13); SET_HL(wChannelEnvelopeStates);
  CYC(b_+13, b_+16); A = W8(wSoundChannel);
  CYC(b_+16, b_+17); E = A;
  CYC(b_+17, b_+19); D = 0x00;
  CYC(b_+19, b_+20); alu_add_hl(gb, DE);
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CYC(b_+21, b_+23); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+23, b_+25); goto checkEnvelopeRequested; } // jr z
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+27); alu_cp(gb, 0x01);
  if (F & FZ) { CYCT(b_+27, b_+29); goto waitForNoteStartEnvelope; } // jr z
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+31); A = 0x00;
  CYC(b_+31, b_+34); W8(wSoundCmdEnvelope) = A;
  RET(b_+34);
  return;
checkEnvelopeRequested:
  CYC(b_+35, b_+38); SET_HL(wChannelEnvelopes);
  CYC(b_+38, b_+41); A = W8(wSoundChannel);
  CYC(b_+41, b_+42); E = A;
  CYC(b_+42, b_+44); D = 0x00;
  CYC(b_+44, b_+45); alu_add_hl(gb, DE);
  CYC(b_+45, b_+46); A = mem_rd(gb, HL);
  CYC(b_+46, b_+48); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+48, b_+50); goto checkAndStartNoteEndEnvelope; } // jr z
  CYC(b_+48, b_+50);
  CYC(b_+50, b_+51); C = A;
  CYC(b_+51, b_+53); alu_or(gb, 0x18);
  CYC(b_+53, b_+56); W8(wSoundCmdEnvelope) = A;
  CYC(b_+56, b_+57); push_effect(gb, BC);
  CALL_C(b_+57, getChannelVolume_hook, SYM(getChannelVolume), b_+60);
  CYC(b_+60, b_+61); SET_BC(pop_effect(gb));
  CYC(b_+61, b_+62); B = A;
  CALL_C(b_+62, getWaitTimeForEnvelope_hook, SYM(getWaitTimeForEnvelope), b_+65);
  CYC(b_+65, b_+68); SET_HL(wChannelEnvelopeWaitCounters);
  CYC(b_+68, b_+69); push_effect(gb, AF);
  CYC(b_+69, b_+72); A = W8(wSoundChannel);
  CYC(b_+72, b_+73); E = A;
  CYC(b_+73, b_+75); D = 0x00;
  CYC(b_+75, b_+76); alu_add_hl(gb, DE);
  CYC(b_+76, b_+77); SET_AF(pop_effect(gb));
  CYC(b_+77, b_+78); mem_wr(gb, HL, A);
  CYC(b_+78, b_+80); A = 0x01;
  CYC(b_+80, b_+83); SET_HL(wChannelEnvelopeStates);
  CYC(b_+83, b_+84); push_effect(gb, AF);
  CYC(b_+84, b_+87); A = W8(wSoundChannel);
  CYC(b_+87, b_+88); E = A;
  CYC(b_+88, b_+90); D = 0x00;
  CYC(b_+90, b_+91); alu_add_hl(gb, DE);
  CYC(b_+91, b_+92); SET_AF(pop_effect(gb));
  CYC(b_+92, b_+93); mem_wr(gb, HL, A);
  CYC(b_+93, b_+96); updateSquareChannelVolume_hook(gb); return; // jp
waitForNoteStartEnvelope:
  CYC(b_+96, b_+99); SET_HL(wChannelEnvelopeWaitCounters);
  CYC(b_+99, b_+102); A = W8(wSoundChannel);
  CYC(b_+102, b_+103); E = A;
  CYC(b_+103, b_+105); D = 0x00;
  CYC(b_+105, b_+106); alu_add_hl(gb, DE);
  CYC(b_+106, b_+107); A = mem_rd(gb, HL);
  CYC(b_+107, b_+109); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+109, b_+111); goto checkAndStartNoteEndEnvelope; } // jr z
  CYC(b_+109, b_+111);
  CYC(b_+111, b_+114); SET_HL(wChannelEnvelopeWaitCounters);
  CYC(b_+114, b_+117); A = W8(wSoundChannel);
  CYC(b_+117, b_+118); E = A;
  CYC(b_+118, b_+120); D = 0x00;
  CYC(b_+120, b_+121); alu_add_hl(gb, DE);
  CYC(b_+121, b_+122); A = mem_rd(gb, HL);
  CYC(b_+122, b_+123); A = alu_dec8(gb, A);
  CYC(b_+123, b_+124); mem_wr(gb, HL, A);
  CYC(b_+124, b_+126); A = 0x00;
  CYC(b_+126, b_+129); W8(wSoundCmdEnvelope) = A;
  RET(b_+129);
  return;
checkAndStartNoteEndEnvelope:
  CYC(b_+130, b_+133); SET_HL(wChannelEnvelopes2);
  CYC(b_+133, b_+136); A = W8(wSoundChannel);
  CYC(b_+136, b_+137); E = A;
  CYC(b_+137, b_+139); D = 0x00;
  CYC(b_+139, b_+140); alu_add_hl(gb, DE);
  CYC(b_+140, b_+141); A = mem_rd(gb, HL);
  CYC(b_+141, b_+143); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(b_+143, b_+145); goto notZeroEnvelope2; } // jr nz
  CYC(b_+143, b_+145);
  CYC(b_+145, b_+147); A = 0x02;
  CYC(b_+147, b_+149); goto haveEnvelopeState; // jr
notZeroEnvelope2:
  CYC(b_+149, b_+151); A = 0x03;
haveEnvelopeState:
  CYC(b_+151, b_+154); SET_HL(wChannelEnvelopeStates);
  CYC(b_+154, b_+155); push_effect(gb, AF);
  CYC(b_+155, b_+158); A = W8(wSoundChannel);
  CYC(b_+158, b_+159); E = A;
  CYC(b_+159, b_+161); D = 0x00;
  CYC(b_+161, b_+162); alu_add_hl(gb, DE);
  CYC(b_+162, b_+163); SET_AF(pop_effect(gb));
  CYC(b_+163, b_+164); mem_wr(gb, HL, A);
  CALL_C(b_+164, getChannelVolume_hook, SYM(getChannelVolume), b_+167);
  CYC(b_+167, b_+169); A = alu_sla(gb, A);
  CYC(b_+169, b_+171); A = alu_sla(gb, A);
  CYC(b_+171, b_+173); A = alu_sla(gb, A);
  CYC(b_+173, b_+175); A = alu_sla(gb, A);
  CYC(b_+175, b_+178); W8(wSoundCmdEnvelope) = A;
  CYC(b_+178, b_+181); SET_HL(wChannelEnvelopes2);
  CYC(b_+181, b_+184); A = W8(wSoundChannel);
  CYC(b_+184, b_+185); E = A;
  CYC(b_+185, b_+187); D = 0x00;
  CYC(b_+187, b_+188); alu_add_hl(gb, DE);
  CYC(b_+188, b_+189); A = mem_rd(gb, HL);
  CYC(b_+189, b_+190); C = A;
  CYC(b_+190, b_+193); A = W8(wSoundCmdEnvelope);
  CYC(b_+193, b_+194); alu_or(gb, C);
  CYC(b_+194, b_+197); W8(wSoundCmdEnvelope) = A;
  CYC(b_+197, SYM(updateSquareChannelVolume)); updateSquareChannelVolume_hook(gb); return; // jp
}

void updateSquareChannelVolume_hook(GB *gb) {
  BASE(updateSquareChannelVolume);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wSoundChannel);
  CYC(b_+3, b_+5); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(b_+5, b_+7); goto setVolume; } // jr nc
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); A = W8(wMusicVolume);
  CYC(b_+10, b_+12); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+12, b_+14); goto doRet; } // jr z
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+17); A = W8(wSoundChannel);
  CYC(b_+17, b_+18); A = alu_inc8(gb, A);
  CYC(b_+18, b_+19); A = alu_inc8(gb, A);
  CYC(b_+19, b_+20); E = A;
  CYC(b_+20, b_+23); SET_HL(wChannelsEnabled);
  CYC(b_+23, b_+25); D = 0x00;
  CYC(b_+25, b_+26); alu_add_hl(gb, DE);
  CYC(b_+26, b_+27); A = mem_rd(gb, HL);
  CYC(b_+27, b_+29); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+29, b_+31); goto setVolume; } // jr z
  CYC(b_+29, b_+31);
doRet:
  RET(b_+31);
  return;
setVolume:
  CYC(b_+32, b_+35); A = W8(wSoundChannel);
  CYC(b_+35, b_+37); alu_and(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+37, b_+39); goto sweepDone; } // jr nz
  CYC(b_+37, b_+39);
  CYC(b_+39, b_+41); A = 0x08;
  CYC(b_+41, b_+43); mem_wr(gb, 0xff10, A);
sweepDone:
  CYC(b_+43, b_+46); A = W8(wSoundChannel);
  CYC(b_+46, b_+48); alu_and(gb, 0x01);
  CYC(b_+48, b_+49); B = A;
  CYC(b_+49, b_+51); A = alu_sla(gb, A);
  CYC(b_+51, b_+53); A = alu_sla(gb, A);
  CYC(b_+53, b_+54); alu_add(gb, B);
  CYC(b_+54, b_+55); B = A;
  CYC(b_+55, b_+58); A = W8(wSoundCmdEnvelope);
  CYC(b_+58, b_+60); C = 0x12;
  CALL_C(b_+60, writeIndexedHighRamAndIncrement_hook, SYM(writeIndexedHighRamAndIncrement), b_+63);
  CYC(b_+63, b_+66); SET_HL(wChannelFrequencyModeAndLengthTimerEnabled);
  CYC(b_+66, b_+69); A = W8(wSoundChannel);
  CYC(b_+69, b_+70); E = A;
  CYC(b_+70, b_+72); D = 0x00;
  CYC(b_+72, b_+73); alu_add_hl(gb, DE);
  CYC(b_+73, b_+74); A = mem_rd(gb, HL);
  CYC(b_+74, b_+76); alu_and(gb, 0x40);
  CYC(b_+76, b_+78); alu_or(gb, 0x80);
  CYC(b_+78, b_+81); W8(wSoundCmdEnvelope) = A;
  RET(b_+81);
}

// Updates wWaveChannelVolume+4 and, if changed, writes to R_NR32 if possible
void updateChannel4Volume_hook(GB *gb) {
  BASE(updateChannel4Volume);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getWaveChannelVolume_hook, SYM(getWaveChannelVolume), b_+3);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+7); A = mem_rd(gb, wWaveChannelVolume + 4);
  CYC(b_+7, b_+8); alu_cp(gb, B);
  if (F & FZ) { CYCT(b_+8, b_+10); goto ret4; } // jr z
  CYC(b_+8, b_+10);
  CALL_C(b_+10, getWaveChannelVolume_hook, SYM(getWaveChannelVolume), b_+13);
  CYC(b_+13, b_+16); mem_wr(gb, wWaveChannelVolume + 4, A);
  CALL_C(b_+16, isWaveChannelUnavailable_hook, SYM(isWaveChannelUnavailable), b_+19);
  CYC(b_+19, b_+21); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(b_+21, b_+23); goto ret4; } // jr nz
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+26); A = mem_rd(gb, wWaveChannelVolume + 4);
  CYC(b_+26, b_+28); mem_wr(gb, 0xff1c, A);
ret4:
  RET(b_+28);
}

// Shared tail of getChannelVolume, reached directly by a real ROM `call $478c`
// from standardCmdChannel6 (NOT independently hooked).
static void getChannelVolume_fullVolume(GB *gb) {
  BASE(getChannelVolume);
  CYC(b_+24, b_+27); SET_HL(wChannelVolumes);
  CYC(b_+27, b_+30); A = W8(wSoundChannel);
  CYC(b_+30, b_+31); E = A;
  CYC(b_+31, b_+33); D = 0x00;
  CYC(b_+33, b_+34); alu_add_hl(gb, DE);
  CYC(b_+34, b_+35); A = mem_rd(gb, HL);
  RET(b_+35);
}

static void getChannelVolume_affectedByMusicVolume(GB *gb) {
  BASE(getChannelVolume);
  CYC(b_+9, b_+12); A = W8(wMusicVolume);
  CYC(b_+12, b_+14); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+14, b_+16); goto muted; } // jr z
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); alu_cp(gb, 0x01);
  if (F & FZ) { CYCT(b_+18, b_+20); goto quarterVolume; } // jr z
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+22); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+22, b_+24); goto halfVolume; } // jr z
  CYC(b_+22, b_+24);
  getChannelVolume_fullVolume(gb);
  return;
halfVolume:
  CYC(b_+36, b_+39); SET_HL(wChannelVolumes);
  CYC(b_+39, b_+42); A = W8(wSoundChannel);
  CYC(b_+42, b_+43); E = A;
  CYC(b_+43, b_+45); D = 0x00;
  CYC(b_+45, b_+46); alu_add_hl(gb, DE);
  CYC(b_+46, b_+47); A = mem_rd(gb, HL);
  CYC(b_+47, b_+49); A = alu_srl(gb, A);
  RET(b_+49);
  return;
quarterVolume:
  CYC(b_+50, b_+53); SET_HL(wChannelVolumes);
  CYC(b_+53, b_+56); A = W8(wSoundChannel);
  CYC(b_+56, b_+57); E = A;
  CYC(b_+57, b_+59); D = 0x00;
  CYC(b_+59, b_+60); alu_add_hl(gb, DE);
  CYC(b_+60, b_+61); A = mem_rd(gb, HL);
  CYC(b_+61, b_+63); A = alu_srl(gb, A);
  CYC(b_+63, b_+65); A = alu_srl(gb, A);
  RET(b_+65);
  return;
muted:
  CYC(b_+66, b_+68); A = 0x00;
  RET(b_+68);
}

// Intended for use with square and noise channels, but not used by channel 7.
// Channel 6 uses @affectedByMusicVolume as entry point.
void getChannelVolume_hook(GB *gb) {
  BASE(getChannelVolume);
  CYC(b_+0, b_+3); A = W8(wSoundChannel);
  CYC(b_+3, b_+4); alu_scf(gb);
  CYC(b_+4, b_+5); alu_ccf(gb);
  CYC(b_+5, b_+7); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(b_+7, b_+9); getChannelVolume_fullVolume(gb); return; } // jr nc
  CYC(b_+7, b_+9);
  getChannelVolume_affectedByMusicVolume(gb);
}

void standardCmdChannels4To5_hook(GB *gb) {
  BASE(standardCmdChannels4To5);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(wChannelFrequencyModeAndLengthTimerEnabled);
  CYC(b_+3, b_+6); A = W8(wSoundChannel);
  CYC(b_+6, b_+7); E = A;
  CYC(b_+7, b_+9); D = 0x00;
  CYC(b_+9, b_+10); alu_add_hl(gb, DE);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+13); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+13, b_+15); goto freqOrCmd; } // jr z
  CYC(b_+13, b_+15);
  CALL_C(b_+15, getNextChannelByte_hook, SYM(getNextChannelByte), b_+18);
  CYC(b_+18, b_+19); L = A;
  CYC(b_+19, b_+22); A = W8(wSoundCmd);
  CYC(b_+22, b_+23); H = A;
  CYC(b_+23, b_+26); goto arbitraryFrequency; // jp
freqOrCmd:
  CYC(b_+26, b_+29); A = W8(wSoundCmd);
  CYC(b_+29, b_+30); alu_scf(gb);
  CYC(b_+30, b_+31); alu_ccf(gb);
  CYC(b_+31, b_+33); alu_cp(gb, 0x60);
  if (!(F & FZ)) { CYCT(b_+33, b_+35); goto freqCommand; } // jr nz
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+37); A = 0x01;
  CYC(b_+37, b_+40); SET_HL(wChannelIsPlayingRest);
  CYC(b_+40, b_+41); push_effect(gb, AF);
  CYC(b_+41, b_+44); A = W8(wSoundChannel);
  CYC(b_+44, b_+45); E = A;
  CYC(b_+45, b_+47); D = 0x00;
  CYC(b_+47, b_+48); alu_add_hl(gb, DE);
  CYC(b_+48, b_+49); SET_AF(pop_effect(gb));
  CYC(b_+49, b_+50); mem_wr(gb, HL, A);
  CALL_C(b_+50, getWaveChannelVolume_hook, SYM(getWaveChannelVolume), b_+53);
  CYC(b_+53, b_+56); SET_HL(wWaveChannelVolume);
  CYC(b_+56, b_+57); push_effect(gb, AF);
  CYC(b_+57, b_+60); A = W8(wSoundChannel);
  CYC(b_+60, b_+61); E = A;
  CYC(b_+61, b_+63); D = 0x00;
  CYC(b_+63, b_+64); alu_add_hl(gb, DE);
  CYC(b_+64, b_+65); SET_AF(pop_effect(gb));
  CYC(b_+65, b_+66); mem_wr(gb, HL, A);
  CALL_C(b_+66, isWaveChannelUnavailable_hook, SYM(isWaveChannelUnavailable), b_+69);
  CYC(b_+69, b_+71); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(b_+71, b_+73); goto waitCmd60; } // jr nz
  CYC(b_+71, b_+73);
  CYC(b_+73, b_+76); SET_HL(wWaveChannelVolume);
  CYC(b_+76, b_+79); A = W8(wSoundChannel);
  CYC(b_+79, b_+80); E = A;
  CYC(b_+80, b_+82); D = 0x00;
  CYC(b_+82, b_+83); alu_add_hl(gb, DE);
  CYC(b_+83, b_+84); A = mem_rd(gb, HL);
  CYC(b_+84, b_+86); mem_wr(gb, 0xff1c, A);
waitCmd60:
  CYC(b_+86, b_+89); setChannelWaitCounter_hook(gb); return; // jp
freqCommand:
  CYC(b_+89, b_+91); A = 0x00;
  CYC(b_+91, b_+94); SET_HL(wChannelIsPlayingRest);
  CYC(b_+94, b_+95); push_effect(gb, AF);
  CYC(b_+95, b_+98); A = W8(wSoundChannel);
  CYC(b_+98, b_+99); E = A;
  CYC(b_+99, b_+101); D = 0x00;
  CYC(b_+101, b_+102); alu_add_hl(gb, DE);
  CYC(b_+102, b_+103); SET_AF(pop_effect(gb));
  CYC(b_+103, b_+104); mem_wr(gb, HL, A);
  CYC(b_+104, b_+107); A = W8(wSoundCmd);
  CYC(b_+107, b_+110); SET_HL(SYM(soundFrequencyTable));
  CALL_C(b_+110, readWordFromTable_hook, SYM(readWordFromTable), b_+113);
arbitraryFrequency:
  CALL_C(b_+113, setSoundFrequency_hook, SYM(setSoundFrequency), b_+116);
  CYC(b_+116, b_+118); A = 0x00;
  CYC(b_+118, b_+121); SET_HL(wChannelVibratoActive);
  CYC(b_+121, b_+122); push_effect(gb, AF);
  CYC(b_+122, b_+125); A = W8(wSoundChannel);
  CYC(b_+125, b_+126); E = A;
  CYC(b_+126, b_+128); D = 0x00;
  CYC(b_+128, b_+129); alu_add_hl(gb, DE);
  CYC(b_+129, b_+130); SET_AF(pop_effect(gb));
  CYC(b_+130, b_+131); mem_wr(gb, HL, A);
  CYC(b_+131, b_+133); A = 0x00;
  CYC(b_+133, b_+136); SET_HL(wChannelVibratos);
  CYC(b_+136, b_+139); A = W8(wSoundChannel);
  CYC(b_+139, b_+140); E = A;
  CYC(b_+140, b_+142); D = 0x00;
  CYC(b_+142, b_+143); alu_add_hl(gb, DE);
  CYC(b_+143, b_+144); A = mem_rd(gb, HL);
  CYC(b_+144, b_+146); alu_and(gb, 0xf0);
  CYC(b_+146, b_+148); A = alu_srl(gb, A);
  CYC(b_+148, b_+150); A = alu_srl(gb, A);
  CYC(b_+150, b_+152); A = alu_srl(gb, A);
  CYC(b_+152, b_+155); SET_HL(wChannelVibratoCounters);
  CYC(b_+155, b_+156); push_effect(gb, AF);
  CYC(b_+156, b_+159); A = W8(wSoundChannel);
  CYC(b_+159, b_+160); E = A;
  CYC(b_+160, b_+162); D = 0x00;
  CYC(b_+162, b_+163); alu_add_hl(gb, DE);
  CYC(b_+163, b_+164); SET_AF(pop_effect(gb));
  CYC(b_+164, b_+165); mem_wr(gb, HL, A);
  CALL_C(b_+165, getWaveChannelVolume_hook, SYM(getWaveChannelVolume), b_+168);
  CYC(b_+168, b_+171); SET_HL(wWaveChannelVolume);
  CYC(b_+171, b_+172); push_effect(gb, AF);
  CYC(b_+172, b_+175); A = W8(wSoundChannel);
  CYC(b_+175, b_+176); E = A;
  CYC(b_+176, b_+178); D = 0x00;
  CYC(b_+178, b_+179); alu_add_hl(gb, DE);
  CYC(b_+179, b_+180); SET_AF(pop_effect(gb));
  CYC(b_+180, b_+181); mem_wr(gb, HL, A);
  CALL_C(b_+181, isWaveChannelUnavailable_hook, SYM(isWaveChannelUnavailable), b_+184);
  CYC(b_+184, b_+186); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(b_+186, b_+188); goto waitFreq; } // jr nz
  CYC(b_+186, b_+188);
  CYC(b_+188, b_+191); SET_HL(wWaveChannelVolume);
  CYC(b_+191, b_+194); A = W8(wSoundChannel);
  CYC(b_+194, b_+195); E = A;
  CYC(b_+195, b_+197); D = 0x00;
  CYC(b_+197, b_+198); alu_add_hl(gb, DE);
  CYC(b_+198, b_+199); A = mem_rd(gb, HL);
  CYC(b_+199, b_+201); mem_wr(gb, 0xff1c, A);
  CYC(b_+201, b_+204); A = W8(wSoundFrequencyL);
  CYC(b_+204, b_+206); mem_wr(gb, 0xff1d, A);
  CYC(b_+206, b_+209); A = W8(wSoundFrequencyH);
  CYC(b_+209, b_+211); mem_wr(gb, 0xff1e, A);
waitFreq:
  CYC(b_+211, SYM(getWaveChannelVolume)); setChannelWaitCounter_hook(gb); return; // jp
}

// @param[out] a Volume of channel wSoundChannel dependent of wMusicVolume,
// in a form that can be written to NR32.
void getWaveChannelVolume_hook(GB *gb) {
  BASE(getWaveChannelVolume);
  CYC(b_+0, b_+3); SET_HL(wChannelIsPlayingRest);
  CYC(b_+3, b_+6); A = W8(wSoundChannel);
  CYC(b_+6, b_+7); E = A;
  CYC(b_+7, b_+9); D = 0x00;
  CYC(b_+9, b_+10); alu_add_hl(gb, DE);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+13); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(b_+13, b_+15); goto mute; } // jr nz
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+18); A = W8(wSoundChannel);
  CYC(b_+18, b_+20); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(b_+20, b_+22); goto fullVolume; } // jr nc
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+25); A = W8(wMusicVolume);
  CYC(b_+25, b_+27); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+27, b_+29); goto mute; } // jr z
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+31); alu_cp(gb, 0x01);
  if (F & FZ) { CYCT(b_+31, b_+33); goto quarterVolume; } // jr z
  CYC(b_+31, b_+33);
  CYC(b_+33, b_+35); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+35, b_+37); goto halfVolume; } // jr z
  CYC(b_+35, b_+37);
fullVolume:
  CYC(b_+37, b_+39); A = 0x20;
  RET(b_+39);
  return;
halfVolume:
  CYC(b_+40, b_+42); A = 0x40;
  RET(b_+42);
  return;
quarterVolume:
  CYC(b_+43, b_+45); A = 0x60;
  RET(b_+45);
  return;
mute:
  CYC(b_+46, b_+48); A = 0x00;
  RET(b_+48);
}

void standardCmdChannel6_hook(GB *gb) {
  BASE(standardCmdChannel6);
  CYC(b_+0, b_+3); A = W8(wSoundCmd);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+7); SET_DE(SYM(nonExistentFunction));
loop:
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); SET_DE(DE + 1);
  CYC(b_+9, b_+11); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+11, b_+13); goto wait; } // jr z
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); alu_cp(gb, C);
  if (F & FZ) { CYCT(b_+14, b_+16); goto found; } // jr z
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+17); SET_DE(DE + 1);
  CYC(b_+17, b_+18); SET_DE(DE + 1);
  CYC(b_+18, b_+20); goto loop; // jr
found:
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); L = A;
  CYC(b_+22, b_+23); SET_DE(DE + 1);
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+25); H = A;
  CYC(b_+25, b_+28); A = mem_rd(gb, wChannelsEnabled + 7);
  CYC(b_+28, b_+30); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(b_+30, b_+32); goto wait; } // jr nz
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+33); push_effect(gb, HL);
  CYC(b_+33, b_+36);
  push_effect(gb, b_+36);
  getChannelVolume_affectedByMusicVolume(gb);
  CYC(b_+36, b_+37); SET_HL(pop_effect(gb));
  CYC(b_+37, b_+39); A = alu_sla(gb, A);
  CYC(b_+39, b_+41); A = alu_sla(gb, A);
  CYC(b_+41, b_+43); A = alu_sla(gb, A);
  CYC(b_+43, b_+45); A = alu_sla(gb, A);
  CYC(b_+45, b_+46); alu_or(gb, L);
  CYC(b_+46, b_+48); mem_wr(gb, 0xff21, A);
  CYC(b_+48, b_+49); A = H;
  CYC(b_+49, b_+51); mem_wr(gb, 0xff22, A);
  CYC(b_+51, b_+53); A = 0x80;
  CYC(b_+53, b_+55); mem_wr(gb, 0xff23, A);
wait:
  CYC(b_+55, SYM(standardCmdChannel7)); setChannelWaitCounter_hook(gb); return; // jp
}

void standardCmdChannel7_hook(GB *gb) {
  BASE(standardCmdChannel7);
  CYC(b_+0, b_+3); A = W8(wSoundCmd);
  CYC(b_+3, b_+5); mem_wr(gb, 0xff22, A);
  CYC(b_+5, b_+7); A = 0x00;
  CYC(b_+7, b_+9); mem_wr(gb, 0xff20, A);
  CYC(b_+9, b_+12); A = W8(wChannel7TriggerOnNextSound);
  CYC(b_+12, b_+14); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+14, b_+16); goto skipTrigger; } // jr z
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); mem_wr(gb, 0xff23, A);
skipTrigger:
  CYC(b_+18, b_+20); A = 0x00;
  CYC(b_+20, b_+23); W8(wChannel7TriggerOnNextSound) = A;
  CYC(b_+23, SYM(channelCmdff)); setChannelWaitCounter_hook(gb); return; // jp
}

// Disables and silences the current channel
void channelCmdff_hook(GB *gb) {
  BASE(channelCmdff);
  CYC(b_+0, b_+2); A = 0x00;
  CYC(b_+2, b_+5); SET_HL(wChannelsEnabled);
  CYC(b_+5, b_+6); push_effect(gb, AF);
  CYC(b_+6, b_+9); A = W8(wSoundChannel);
  CYC(b_+9, b_+10); E = A;
  CYC(b_+10, b_+12); D = 0x00;
  CYC(b_+12, b_+13); alu_add_hl(gb, DE);
  CYC(b_+13, b_+14); SET_AF(pop_effect(gb));
  CYC(b_+14, SYM(silencePlayedSound)); mem_wr(gb, HL, A);
  silencePlayedSound_hook(gb);
}

// Ensures no sound is audible on the current channel by setting the volume
// to $0 or turning off the wave channel DAC.
void silencePlayedSound_hook(GB *gb) {
  BASE(silencePlayedSound);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wSoundChannel);
  CYC(b_+3, b_+6); SET_HL(b_+10);
  CALL_C(b_+6, readWordFromTable_hook, SYM(readWordFromTable), b_+9);
  CYC(b_+9, b_+10);
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+26) { goto musicSquareChannel; }
    else if (jt_ == b_+44) { goto sfxSquareChannel; }
    else if (jt_ == b_+88) { goto musicWaveChannel; }
    else if (jt_ == b_+100) { goto sfxWaveChannel; }
    else if (jt_ == b_+134) { goto noiseChannel; }
    else { HANDOFF(HL); }
  } while (0);
musicSquareChannel:
  // Only update if the corresponding sfx channel is not enabled
  CYC(b_+26, b_+29); A = W8(wSoundChannel);
  CYC(b_+29, b_+30); A = alu_inc8(gb, A);
  CYC(b_+30, b_+31); A = alu_inc8(gb, A);
  CYC(b_+31, b_+32); E = A;
  CYC(b_+32, b_+35); SET_HL(wChannelsEnabled);
  CYC(b_+35, b_+37); D = 0x00;
  CYC(b_+37, b_+38); alu_add_hl(gb, DE);
  CYC(b_+38, b_+39); A = mem_rd(gb, HL);
  CYC(b_+39, b_+41); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+41, b_+43); goto silenceSharedTail; } // jr z
  CYC(b_+41, b_+43);
  RET(b_+43);
  return;
sfxSquareChannel:
  CYC(b_+44, b_+47); A = W8(wSoundChannel);
  CYC(b_+47, b_+48); A = alu_dec8(gb, A);
  CYC(b_+48, b_+49); A = alu_dec8(gb, A);
  CYC(b_+49, b_+50); E = A;
  CYC(b_+50, b_+53); SET_HL(wChannelsEnabled);
  CYC(b_+53, b_+55); D = 0x00;
  CYC(b_+55, b_+56); alu_add_hl(gb, DE);
  CYC(b_+56, b_+57); A = mem_rd(gb, HL);
  CYC(b_+57, b_+59); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+59, b_+61); } else { CYC(b_+59, b_+61); } // jr z
  goto silenceSharedTail;
silenceSharedTail:
  // If an envelope is active that decreases volume over time, allow the
  // sound to keep playing.
  CYC(b_+61, b_+64); SET_HL(wChannelEnvelopeStates);
  CYC(b_+64, b_+67); A = W8(wSoundChannel);
  CYC(b_+67, b_+68); E = A;
  CYC(b_+68, b_+70); D = 0x00;
  CYC(b_+70, b_+71); alu_add_hl(gb, DE);
  CYC(b_+71, b_+72); A = mem_rd(gb, HL);
  CYC(b_+72, b_+74); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+74, b_+76); goto silenceAndUpdate; } // jr nz
  CYC(b_+74, b_+76);
  RET(b_+76);
  return;
silenceAndUpdate:
  CYC(b_+77, b_+79); A = 0x08;
  CYC(b_+79, b_+82); W8(wSoundCmdEnvelope) = A;
  CALL_C(b_+82, updateSquareChannelVolume_hook, SYM(updateSquareChannelVolume), b_+85);
  CYC(b_+85, b_+88); updatePlayedFrequency_hook(gb); return; // jp
musicWaveChannel:
  CALL_C(b_+88, isWaveChannelUnavailable_hook, SYM(isWaveChannelUnavailable), b_+91);
  CYC(b_+91, b_+93); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(b_+93, b_+95); goto musicWaveChannelRet; } // jr nz
  CYC(b_+93, b_+95);
  CYC(b_+95, b_+97); A = 0x00;
  CYC(b_+97, b_+99); mem_wr(gb, 0xff1a, A);
musicWaveChannelRet:
  RET(b_+99);
  return;
sfxWaveChannel:
  CYC(b_+100, b_+103); A = mem_rd(gb, wChannelsEnabled + 4);
  CYC(b_+103, b_+105); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+105, b_+107); goto sfxWaveChannelDisable; } // jr z
  CYC(b_+105, b_+107);
  CYC(b_+107, b_+109); A = 0x04;
  CYC(b_+109, b_+110); E = A;
  CYC(b_+110, b_+113); SET_HL(wChannelDutyCycles);
  CYC(b_+113, b_+115); D = 0x00;
  CYC(b_+115, b_+116); alu_add_hl(gb, DE);
  CYC(b_+116, b_+117); A = mem_rd(gb, HL);
  CYC(b_+117, b_+120); W8(wWaveformIndex) = A;
  CALL_C(b_+120, setWaveform_hook, SYM(setWaveform), b_+123);
  CYC(b_+123, b_+126); A = mem_rd(gb, wWaveChannelVolume + 4);
  CYC(b_+126, b_+128); mem_wr(gb, 0xff1c, A);
  RET(b_+128);
  return;
sfxWaveChannelDisable:
  CYC(b_+129, b_+131); A = 0x00;
  CYC(b_+131, b_+133); mem_wr(gb, 0xff1a, A);
  RET(b_+133);
  return;
noiseChannel:
  CYC(b_+134, b_+136); A = 0x08;
  CYC(b_+136, b_+138); mem_wr(gb, 0xff21, A);
  CYC(b_+138, b_+140); A = 0x80;
  CYC(b_+140, b_+142); mem_wr(gb, 0xff23, A);
  RET(b_+142);
}

void setWaveform_hook(GB *gb) {
  BASE(setWaveform);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, isWaveChannelUnavailable_hook, SYM(isWaveChannelUnavailable), b_+3);
  CYC(b_+3, b_+5); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+5, b_+7); goto waitLoop; } // jr z
  CYC(b_+5, b_+7);
  RET(b_+7);
  return;
waitLoop:
  CYC(b_+8, b_+10); A = 0x00;
  CYC(b_+10, b_+12); mem_wr(gb, 0xff1a, A);
  CYC(b_+12, b_+14); A = mem_rd(gb, 0xff26);
  CYC(b_+14, b_+16); alu_and(gb, 0x04);
  if (!(F & FZ)) { CYCT(b_+16, b_+18); goto waitLoop; } // jr nz
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+21); A = W8(wWaveformIndex);
  CYC(b_+21, b_+24); SET_HL(SYM(waveformTable));
  CALL_C(b_+24, readWordFromTable_hook, SYM(readWordFromTable), b_+27);
  CYC(b_+27, b_+29); C = 0x10;
  CYC(b_+29, b_+32); SET_DE(0xff30);
copyLoop:
  CYC(b_+32, b_+33); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  CYC(b_+34, b_+35); SET_DE(DE + 1);
  CYC(b_+35, b_+36); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(b_+36, b_+38); goto copyLoop; } // jr nz
  CYC(b_+36, b_+38);
triggerWait:
  CYC(b_+38, b_+40); A = 0x80;
  CYC(b_+40, b_+42); mem_wr(gb, 0xff1a, A);
  CYC(b_+42, b_+44); A = mem_rd(gb, 0xff1a);
  CYC(b_+44, b_+46); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(b_+46, b_+48); goto triggerWait; } // jr z
  CYC(b_+46, b_+48);
  CYC(b_+48, b_+50); A = 0x80;
  CYC(b_+50, b_+52); mem_wr(gb, 0xff1e, A);
  RET(b_+52);
}

// Jump to word from argument
void channelCmdfe_hook(GB *gb) {
  BASE(channelCmdfe);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getNextChannelByte_hook, SYM(getNextChannelByte), b_+3);
  CYC(b_+3, b_+4); L = A;
  CALL_C(b_+4, getNextChannelByte_hook, SYM(getNextChannelByte), b_+7);
  CYC(b_+7, b_+8); H = A;
  CYC(b_+8, b_+11); A = W8(wSoundChannel);
  CYC(b_+11, b_+13); A = alu_sla(gb, A);
  CYC(b_+13, b_+14); B = A;
  CYC(b_+14, b_+15); A = L;
  CYC(b_+15, b_+17); C = 0xe2;
  CALL_C(b_+17, writeIndexedHighRamAndIncrement_hook, SYM(writeIndexedHighRamAndIncrement), b_+20);
  CYC(b_+20, b_+21); A = H;
  CYC(b_+21, b_+22); mem_wr(gb, 0xff00 | C, A);
  CYC(b_+22, b_+23); C = alu_inc8(gb, C);
  CYC(b_+23, SYM(multiplyHlByA)); doNextChannelCommand_hook(gb); return; // jp
}

void multiplyHlByA_hook(GB *gb) {
  BASE(multiplyHlByA);
  CYC(b_+0, b_+2); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(b_+2, b_+4); goto nonzero; } // jr nz
  CYC(b_+2, b_+4);
  CYC(b_+4, b_+7); SET_HL(0x0000);
  RET(b_+7);
  return;
nonzero:
  CYC(b_+8, b_+9); E = L;
  CYC(b_+9, b_+10); D = H;
loop:
  CYC(b_+10, b_+11); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+11, b_+13); goto done; } // jr z
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); alu_add_hl(gb, DE);
  CYC(b_+14, b_+17); goto loop; // jp
done:
  RET(b_+17);
}

// @param a The sound to play.
void playSound_b39_hook(GB *gb) {
  BASE(playSound_b39);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); push_effect(gb, BC);
  CYC(b_+1, b_+2); push_effect(gb, DE);
  CYC(b_+2, b_+3); push_effect(gb, HL);
  CYC(b_+3, b_+6); W8(wSoundTmp) = A;
  CYC(b_+6, b_+8); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(b_+8, b_+10); goto dispatch; } // jr nz
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+13); goto playSoundEnd; // jp
dispatch:
  CYC(b_+13, b_+15); alu_cp(gb, 0xf0);
  if (F & FZ) { CYCT(b_+15, b_+17); goto sndf0; } // jr z
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); alu_cp(gb, 0xf1);
  if (F & FZ) { CYCT(b_+19, b_+21); goto sndf1; } // jr z
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+23); alu_cp(gb, 0xf5);
  if (F & FZ) { CYCT(b_+23, b_+25); goto sndf5; } // jr z
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+27); alu_cp(gb, 0xf6);
  if (F & FZ) { CYCT(b_+27, b_+29); goto sndf6; } // jr z
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+31); alu_cp(gb, 0xf7);
  if (F & FZ) { CYCT(b_+31, b_+33); goto sndf7; } // jr z
  CYC(b_+31, b_+33);
  CYC(b_+33, b_+35); alu_cp(gb, 0xf8);
  if (F & FZ) { CYCT(b_+35, b_+37); goto sndf8; } // jr z
  CYC(b_+35, b_+37);
  CYC(b_+37, b_+39); alu_cp(gb, 0xf9);
  if (F & FZ) { CYCT(b_+39, b_+41); goto sndf9; } // jr z
  CYC(b_+39, b_+41);
  CYC(b_+41, b_+43); alu_cp(gb, 0xfa);
  if (F & FZ) { CYCT(b_+43, b_+45); goto sndfa; } // jr z
  CYC(b_+43, b_+45);
  CYC(b_+45, b_+47); alu_cp(gb, 0xfb);
  if (F & FZ) { CYCT(b_+47, b_+49); goto sndfb; } // jr z
  CYC(b_+47, b_+49);
  CYC(b_+49, b_+51); alu_cp(gb, 0xfc);
  if (F & FZ) { CYCT(b_+51, b_+53); goto sndfc; } // jr z
  CYC(b_+51, b_+53);
  CYC(b_+53, b_+55); goto normalSound; // jr
sndf0:
  CYC(b_+55, b_+57); A = 0xde; // stop music
  CYC(b_+57, b_+60); W8(wSoundTmp) = A;
  CYC(b_+60, b_+62); goto normalSound; // jr
sndf1:
  CALL_C(b_+62, stopSfx_hook, SYM(stopSfx), b_+65);
  CYC(b_+65, b_+68); goto playSoundEnd; // jp
sndf5:
  CALL_C(b_+68, silenceAllChannels_hook, SYM(silenceAllChannels), b_+71);
  CYC(b_+71, b_+73); A = 0x01;
  CYC(b_+73, b_+76); W8(wSoundDisabled) = A;
  CYC(b_+76, b_+79); goto setVolumeAndEnd; // jp
sndf6:
  CYC(b_+79, b_+81); A = 0x00;
  CYC(b_+81, b_+84); W8(wSoundDisabled) = A;
  CYC(b_+84, b_+87); goto setVolumeAndEnd; // jp
sndfa:
  CYC(b_+87, b_+89); A = 0x07;
  CYC(b_+89, b_+91); goto fadeSpeedSet; // jr
sndfb:
  CYC(b_+91, b_+93); A = 0x0f;
  CYC(b_+93, b_+95); goto fadeSpeedSet; // jr
sndfc:
  CYC(b_+95, b_+97); A = 0x1f;
fadeSpeedSet:
  CYC(b_+97, b_+100); W8(wSoundFadeSpeed) = A;
  CYC(b_+100, b_+102); A = 0x00;
  CYC(b_+102, b_+105); W8(wSoundFadeCounter) = A;
  CYC(b_+105, b_+107); A = 0x01;
  CYC(b_+107, b_+110); W8(wSoundFadeDirection) = A;
  CYC(b_+110, b_+112); A = 0x77;
  CYC(b_+112, b_+115); W8(wSoundVolume) = A;
  CYC(b_+115, b_+118); goto playSoundEnd; // jp
sndf7:
  CYC(b_+118, b_+120); A = 0x03;
  CYC(b_+120, b_+122); goto fadeInSet; // jr
sndf8:
  CYC(b_+122, b_+124); A = 0x07;
  CYC(b_+124, b_+126); goto fadeInSet; // jr
sndf9:
  CYC(b_+126, b_+128); A = 0x0f;
fadeInSet:
  CYC(b_+128, b_+131); W8(wSoundFadeSpeed) = A;
  CYC(b_+131, b_+133); A = 0x00;
  CYC(b_+133, b_+136); W8(wSoundFadeCounter) = A;
  CYC(b_+136, b_+138); A = 0x0a;
  CYC(b_+138, b_+141); W8(wSoundFadeDirection) = A;
  CYC(b_+141, b_+143); A = 0x00;
  CYC(b_+143, b_+146); W8(wSoundVolume) = A;
  CYC(b_+146, b_+149); goto playSoundEnd; // jp
normalSound:
  CYC(b_+149, b_+151); A = 0x00;
  CYC(b_+151, b_+154); W8(wSoundFadeDirection) = A;
  CYC(b_+154, b_+157); A = W8(wSoundTmp);
  CYC(b_+157, b_+159); D = 0x00;
  CYC(b_+159, b_+160); E = A;
  CYC(b_+160, b_+162); H = 0x00;
  CYC(b_+162, b_+163); L = A;
  CYC(b_+163, b_+165); L = alu_sla(gb, L);
  CYC(b_+165, b_+167); H = alu_rl(gb, H);
  CYC(b_+167, b_+168); alu_add_hl(gb, DE);
  CYC(b_+168, b_+169); D = H;
  CYC(b_+169, b_+170); E = L;
  CYC(b_+170, b_+173); SET_HL(SYM(soundPointers));
  CYC(b_+173, b_+174); alu_add_hl(gb, DE);
  CYC(b_+174, b_+175); A = mem_rd(gb, HL);
  CYC(b_+175, b_+177); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(b_+177, b_+179); goto skipWeirdCall; } // jr z
  CYC(b_+177, b_+179);
  CALL_ROM(b_+179, SYM(nonExistentFunction));
  CYC(b_+182, b_+185); goto setVolumeAndEnd; // jp
skipWeirdCall:
  CYC(b_+185, b_+186); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+186, b_+187); C = A;
  CYC(b_+187, b_+189); A = H8(hSoundDataBaseBank);
  CYC(b_+189, b_+190); alu_add(gb, C);
  CYC(b_+190, b_+193); W8(wLoadingSoundBank) = A;
  CYC(b_+193, b_+194); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+194, b_+195); C = A;
  CYC(b_+195, b_+196); A = mem_rd(gb, HL);
  CYC(b_+196, b_+197); B = A;
  CYC(b_+197, b_+198); L = C;
  CYC(b_+198, b_+199); H = B;
nextSoundChannel:
  CYC(b_+199, b_+201); A = H8(hSoundDataBaseBank);
  CALL_C(b_+201, wMusicReadFunction_hook, wMusicReadFunction, b_+204);
  CYC(b_+204, b_+206); alu_cp(gb, 0xff);
  if (!(F & FZ)) { CYCT(b_+206, b_+208); goto haveChannelByte; } // jr nz
  CYC(b_+206, b_+208);
  CYC(b_+208, b_+211); goto setVolumeAndEnd; // jp
haveChannelByte:
  CYC(b_+211, b_+214); W8(wSoundTmp) = A;
  CYC(b_+214, b_+216); alu_and(gb, 0xf0);
  CYC(b_+216, b_+218); A = alu_swap(gb, A);
  CYC(b_+218, b_+219); A = alu_inc8(gb, A);
  CYC(b_+219, b_+222); W8(wSoundChannelValue) = A;
  CYC(b_+222, b_+225); A = W8(wSoundTmp);
  CYC(b_+225, b_+227); alu_and(gb, 0x0f);
  CYC(b_+227, b_+230); W8(wSoundTmp) = A;
  CYC(b_+230, b_+231); E = A;
  CYC(b_+231, b_+232); push_effect(gb, HL);
  CYC(b_+232, b_+235); SET_HL(wChannelsEnabled);
  CYC(b_+235, b_+237); D = 0x00;
  CYC(b_+237, b_+238); alu_add_hl(gb, DE);
  CYC(b_+238, b_+239); A = mem_rd(gb, HL);
  CYC(b_+239, b_+240); SET_HL(pop_effect(gb));
  CYC(b_+240, b_+241); C = A;
  CYC(b_+241, b_+244); A = W8(wSoundChannelValue);
  CYC(b_+244, b_+245); alu_cp(gb, C);
  if (!(F & FC)) { CYCT(b_+245, b_+247); goto channelSlotFound; } // jr nc
  CYC(b_+245, b_+247);
  CYC(b_+247, b_+248); SET_HL(HL + 1);
  CYC(b_+248, b_+249); SET_HL(HL + 1);
  CYC(b_+249, b_+252); goto nextSoundChannel; // jp
channelSlotFound:
  CYC(b_+252, b_+253); push_effect(gb, HL);
  CYC(b_+253, b_+256); A = W8(wSoundTmp);
  CYC(b_+256, b_+257); E = A;
  CYC(b_+257, b_+260); A = W8(wSoundChannelValue);
  CYC(b_+260, b_+263); SET_HL(wChannelsEnabled);
  CYC(b_+263, b_+265); D = 0x00;
  CYC(b_+265, b_+266); alu_add_hl(gb, DE);
  CYC(b_+266, b_+267); mem_wr(gb, HL, A);
  CYC(b_+267, b_+269); A = 0x08;
  CYC(b_+269, b_+272); SET_HL(wChannelVolumes);
  CYC(b_+272, b_+274); D = 0x00;
  CYC(b_+274, b_+275); alu_add_hl(gb, DE);
  CYC(b_+275, b_+276); mem_wr(gb, HL, A);
  CYC(b_+276, b_+278); A = 0x00;
  CYC(b_+278, b_+281); SET_HL(wChannelWaitCounters);
  CYC(b_+281, b_+283); D = 0x00;
  CYC(b_+283, b_+284); alu_add_hl(gb, DE);
  CYC(b_+284, b_+285); mem_wr(gb, HL, A);
  CYC(b_+285, b_+288); A = W8(wSoundTmp);
  CYC(b_+288, b_+290); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+290, b_+292); goto squareChannel; } // jr z
  CYC(b_+290, b_+292);
  CYC(b_+292, b_+294); alu_cp(gb, 0x01);
  if (F & FZ) { CYCT(b_+294, b_+296); goto squareChannel; } // jr z
  CYC(b_+294, b_+296);
  CYC(b_+296, b_+298); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+298, b_+300); goto squareChannel; } // jr z
  CYC(b_+298, b_+300);
  CYC(b_+300, b_+302); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(b_+302, b_+304); goto squareChannel; } // jr z
  CYC(b_+302, b_+304);
  CYC(b_+304, b_+306); alu_cp(gb, 0x04);
  if (F & FZ) { CYCT(b_+306, b_+308); goto waveChannel; } // jr z
  CYC(b_+306, b_+308);
  CYC(b_+308, b_+310); alu_cp(gb, 0x05);
  if (F & FZ) { CYCT(b_+310, b_+312); goto waveChannel; } // jr z
  CYC(b_+310, b_+312);
  CYC(b_+312, b_+314); goto writeChannelPtrs; // jr
waveChannel:
  CYC(b_+314, b_+317); A = W8(wSoundTmp);
  CYC(b_+317, b_+318); E = A;
  CYC(b_+318, b_+320); A = 0x00;
  CYC(b_+320, b_+323); SET_HL(wChannelVibratos);
  CYC(b_+323, b_+325); D = 0x00;
  CYC(b_+325, b_+326); alu_add_hl(gb, DE);
  CYC(b_+326, b_+327); mem_wr(gb, HL, A);
  CYC(b_+327, b_+330); SET_HL(wChannelSweep);
  CYC(b_+330, b_+332); D = 0x00;
  CYC(b_+332, b_+333); alu_add_hl(gb, DE);
  CYC(b_+333, b_+334); mem_wr(gb, HL, A);
  CYC(b_+334, b_+337); SET_HL(wChannelPitchShift);
  CYC(b_+337, b_+339); D = 0x00;
  CYC(b_+339, b_+340); alu_add_hl(gb, DE);
  CYC(b_+340, b_+341); mem_wr(gb, HL, A);
  CYC(b_+341, b_+344); SET_HL(wChannelFrequencyModeAndLengthTimerEnabled);
  CYC(b_+344, b_+346); D = 0x00;
  CYC(b_+346, b_+347); alu_add_hl(gb, DE);
  CYC(b_+347, b_+348); mem_wr(gb, HL, A);
  CYC(b_+348, b_+350); goto writeChannelPtrs; // jr
squareChannel:
  CYC(b_+350, b_+353); A = W8(wSoundTmp);
  CYC(b_+353, b_+354); E = A;
  CYC(b_+354, b_+356); A = 0x00;
  CYC(b_+356, b_+359); SET_HL(wChannelEnvelopes);
  CYC(b_+359, b_+361); D = 0x00;
  CYC(b_+361, b_+362); alu_add_hl(gb, DE);
  CYC(b_+362, b_+363); mem_wr(gb, HL, A);
  CYC(b_+363, b_+366); SET_HL(wChannelEnvelopes2);
  CYC(b_+366, b_+368); D = 0x00;
  CYC(b_+368, b_+369); alu_add_hl(gb, DE);
  CYC(b_+369, b_+370); mem_wr(gb, HL, A);
  CYC(b_+370, b_+373); SET_HL(wChannelDutyCycles);
  CYC(b_+373, b_+375); D = 0x00;
  CYC(b_+375, b_+376); alu_add_hl(gb, DE);
  CYC(b_+376, b_+377); mem_wr(gb, HL, A);
  CYC(b_+377, b_+380); SET_HL(wChannelVibratos);
  CYC(b_+380, b_+382); D = 0x00;
  CYC(b_+382, b_+383); alu_add_hl(gb, DE);
  CYC(b_+383, b_+384); mem_wr(gb, HL, A);
  CYC(b_+384, b_+387); SET_HL(wChannelSweep);
  CYC(b_+387, b_+389); D = 0x00;
  CYC(b_+389, b_+390); alu_add_hl(gb, DE);
  CYC(b_+390, b_+391); mem_wr(gb, HL, A);
  CYC(b_+391, b_+394); SET_HL(wChannelPitchShift);
  CYC(b_+394, b_+396); D = 0x00;
  CYC(b_+396, b_+397); alu_add_hl(gb, DE);
  CYC(b_+397, b_+398); mem_wr(gb, HL, A);
  CYC(b_+398, b_+401); SET_HL(wChannelFrequencyModeAndLengthTimerEnabled);
  CYC(b_+401, b_+403); D = 0x00;
  CYC(b_+403, b_+404); alu_add_hl(gb, DE);
  CYC(b_+404, b_+405); mem_wr(gb, HL, A);
writeChannelPtrs:
  CYC(b_+405, b_+406); SET_HL(pop_effect(gb));
  CYC(b_+406, b_+409); A = W8(wSoundTmp);
  CYC(b_+409, b_+410); B = A;
  CYC(b_+410, b_+413); A = W8(wLoadingSoundBank);
  CYC(b_+413, b_+415); C = 0xda;
  CALL_C(b_+415, writeIndexedHighRamAndIncrement_hook, SYM(writeIndexedHighRamAndIncrement), b_+418);
  CYC(b_+418, b_+421); A = W8(wSoundTmp);
  CYC(b_+421, b_+423); A = alu_sla(gb, A);
  CYC(b_+423, b_+424); B = A;
  CYC(b_+424, b_+425); push_effect(gb, BC);
  CYC(b_+425, b_+427); A = H8(hSoundDataBaseBank);
  CALL_C(b_+427, wMusicReadFunction_hook, wMusicReadFunction, b_+430);
  CYC(b_+430, b_+431); SET_BC(pop_effect(gb));
  CYC(b_+431, b_+433); C = 0xe2;
  CALL_C(b_+433, writeIndexedHighRamAndIncrement_hook, SYM(writeIndexedHighRamAndIncrement), b_+436);
  CYC(b_+436, b_+437); push_effect(gb, BC);
  CYC(b_+437, b_+439); A = H8(hSoundDataBaseBank);
  CALL_C(b_+439, wMusicReadFunction_hook, wMusicReadFunction, b_+442);
  CYC(b_+442, b_+443); SET_BC(pop_effect(gb));
  CYC(b_+443, b_+444); mem_wr(gb, 0xff00 | C, A);
  CYC(b_+444, b_+445); C = alu_inc8(gb, C);
  CYC(b_+445, b_+448); goto nextSoundChannel; // jp
setVolumeAndEnd:
  CYC(b_+448, b_+450); A = 0x77;
  CYC(b_+450, b_+453); W8(wSoundVolume) = A;
playSoundEnd:
  CYC(b_+453, b_+454); SET_HL(pop_effect(gb));
  CYC(b_+454, b_+455); SET_DE(pop_effect(gb));
  CYC(b_+455, b_+456); SET_BC(pop_effect(gb));
  RET(b_+456);
}

// Reads a word at hl+a*2 into de and hl. Index can't be higher than $7f.
void readWordFromTable_hook(GB *gb) {
  BASE(readWordFromTable);
  CYC(b_+0, b_+2); A = alu_sla(gb, A);
  CYC(b_+2, b_+4); D = 0x00;
  CYC(b_+4, b_+5); E = A;
  CYC(b_+5, b_+6); alu_add_hl(gb, DE);
  CYC(b_+6, b_+7); E = mem_rd(gb, HL);
  CYC(b_+7, b_+8); SET_HL(HL + 1);
  CYC(b_+8, b_+9); D = mem_rd(gb, HL);
  CYC(b_+9, b_+10); H = D;
  CYC(b_+10, b_+11); L = E;
  RET(b_+11);
}

// Adds b to c, writes a to ($ff00+c), increments c.
void writeIndexedHighRamAndIncrement_hook(GB *gb) {
  BASE(writeIndexedHighRamAndIncrement);
  CYC(b_+0, b_+1); push_effect(gb, AF);
  CYC(b_+1, b_+2); A = B;
  CYC(b_+2, b_+3); alu_add(gb, C);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+5); SET_AF(pop_effect(gb));
  CYC(b_+5, b_+6); mem_wr(gb, 0xff00 | C, A);
  CYC(b_+6, b_+7); C = alu_inc8(gb, C);
  RET(b_+7);
}
