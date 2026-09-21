#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void b39_initSound_hook(GB *gb);
void b39_updateSound_hook(GB *gb);
void b39_playSound_hook(GB *gb);
void b39_stopSound_hook(GB *gb);
void func_39_400c_hook(GB *gb);
void b39_updateMusicVolume_hook(GB *gb);
void initSound_b39_hook(GB *gb);
void initSound__readFunction_b39_hook(GB *gb);
void updateMusicVolume_hook(GB *gb);
void silenceSquareMusicChannels_hook(GB *gb);
void stopSound_hook(GB *gb);
void silenceAllChannels_hook(GB *gb);
void stopSfx_hook(GB *gb);
void updateSound_hook(GB *gb);
void updateSound__decVolume_hook(GB *gb);
void updateSound__incVolume_hook(GB *gb);
void updateSound__stopSound_hook(GB *gb);
void updateSound__clearFadeVariables_hook(GB *gb);
void updateSound__updateChannels_hook(GB *gb);
void updateSound__channelLoop_hook(GB *gb);
void updateSound__continueSound_hook(GB *gb);
void updateSound__nextChannel_hook(GB *gb);
void updateSound__ret_hook(GB *gb);
void continuePlayingSound_hook(GB *gb);
void continuePlayingSound__ret_hook(GB *gb);
void updateSoundFrequencyAndPlay_hook(GB *gb);
void updateSoundFrequencyAndPlay__handleVibrato_hook(GB *gb);
void updateSoundFrequencyAndPlay__endVibratoWait_hook(GB *gb);
void updateSoundFrequencyAndPlay__useVibrato_hook(GB *gb);
void updateSoundFrequencyAndPlay__determineFrequencyOffset_hook(GB *gb);
void updateSoundFrequencyAndPlay__updateSoundFrequencyWithOffset_hook(GB *gb);
void updatePlayedFrequency_hook(GB *gb);
void updatePlayedFrequency__square_hook(GB *gb);
void updatePlayedFrequency__wave_hook(GB *gb);
void updatePlayedFrequency__ret_hook(GB *gb);
void isWaveChannelUnavailable_hook(GB *gb);
void isWaveChannelUnavailable__available_hook(GB *gb);
void isWaveChannelUnavailable__unavailable_hook(GB *gb);
void getNextChannelByte_hook(GB *gb);
void doNextChannelCommand_hook(GB *gb);
void doNextChannelCommand__cmdf0Toff_hook(GB *gb);
void channelCmdf1_hook(GB *gb);
void channelCmdf2_hook(GB *gb);
void channelCmdf3_hook(GB *gb);
void channelCmdf9_hook(GB *gb);
void channelCmdf8_hook(GB *gb);
void channelCmdfd_hook(GB *gb);
void cmde0Toef_hook(GB *gb);
void channelCmdf0_hook(GB *gb);
void channelCmdf0__disableLengthTimer_hook(GB *gb);
void channelCmdf0__channel7_hook(GB *gb);
void cmdVolume_hook(GB *gb);
void cmdVolume__next_hook(GB *gb);
void channelCmdf6_hook(GB *gb);
void channelCmdf6__wave_hook(GB *gb);
void standardSoundCmd_hook(GB *gb);
void standardSoundCmd__table_hook(GB *gb);
void standardSoundCmd__channel0To3_hook(GB *gb);
void standardSoundCmd__cmd60_hook(GB *gb);
void standardSoundCmd__cmd61_hook(GB *gb);
void standardSoundCmd__cmdFrequency_hook(GB *gb);
void standardSoundCmd__arbitraryFrequency_hook(GB *gb);

void playSound_b39_hook(GB *gb);
void writeIndexedHighRamAndIncrement_hook(GB *gb);
void readWordFromTable_hook(GB *gb);
void multiplyHlByA_hook(GB *gb);
void setWaveform_hook(GB *gb);
void setSoundFrequency_hook(GB *gb);
void handleEnvelopes_hook(GB *gb);
void getChannelVolume_hook(GB *gb);
void updateSquareChannelVolume_hook(GB *gb);
void setChannelWaitCounter_hook(GB *gb);
void standardCmdChannels4To5_hook(GB *gb);
void standardCmdChannel6_hook(GB *gb);
void standardCmdChannel7_hook(GB *gb);
void channelCmdff_hook(GB *gb);
void channelCmdfe_hook(GB *gb);
void silencePlayedSound_hook(GB *gb);

// 39:4000, bare global.
void b39_initSound_hook(GB *gb) {
  BASE(b39_initSound);
  CYC(b_+0, b_+3); TAIL(initSound_b39); // jp
}

// 39:4003, bare global.
void b39_updateSound_hook(GB *gb) {
  BASE(b39_updateSound);
  CYC(b_+0, b_+3); TAIL(updateSound); // jp
}

// 39:4006, bare global.
// @param a Sound to play
void b39_playSound_hook(GB *gb) {
  BASE(b39_playSound);
  CYC(b_+0, b_+3); TAIL(playSound_b39); // jp
}

// 39:4009, bare global.
void b39_stopSound_hook(GB *gb) {
  BASE(b39_stopSound);
  CYC(b_+0, b_+3); TAIL(stopSound); // jp
}

// 39:400c, bare global. Unused; the jump target disassembles as the raw bytes of
// noiseFrequencyTable/waveformTable, decoded here exactly as the real hardware would run them.
void func_39_400c_hook(GB *gb) {
  BASE(nonExistentFunction);
  uint16_t sp0_ = gb->sp;
  SET_AF(POP(SYM(func_39_400c)));
  CYC((SYM(func_39_400c) + 1), (SYM(func_39_400c) + 4)); goto garbage; // jp $4d3e

garbage:
  CYC(b_+6, b_+7); SET_HL(HL + 1); // inc hl
  CYC(b_+7, b_+8); mem_wr(gb, BC, A); // ld (bc),a
  CYC(b_+8, b_+9); B = mem_rd(gb, HL); // ld b,(hl)
  CYC(b_+9, b_+11); H = 0x02;
  CYC(b_+11, b_+13); H = 0x28;
  CYC(b_+13, b_+14); // nop
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CYC(b_+15, b_+16); alu_daa(gb);
  CYC(b_+16, b_+17); mem_wr(gb, BC, A);
  CYC(b_+17, b_+18); D = alu_inc8(gb, D);
  CYC(b_+18, b_+19); A = mem_rd(gb, HL); SET_HL(HL + 1); // ld a,(hl+)
  CYC(b_+19, b_+22); SET_BC(0x2e14);
  CYC(b_+22, b_+24); B = 0x07;
  CYC(b_+24, b_+25); D = D;
  CYC(b_+25, b_+26); SET_BC(BC + 1);
  CYC(b_+26, b_+27); alu_rla(gb);
  CYC(b_+27, b_+28); mem_wr(gb, HL, A); SET_HL(HL - 1); // ld (hl-),a
  CYC(b_+28, b_+29); mem_wr(gb, BC, A);
  CYC(b_+29, b_+30); alu_scf(gb);
  CYC(b_+30, b_+31); alu_cpl(gb);
  CYC(b_+31, b_+32); mem_wr(gb, BC, A);
  CYC(b_+32, b_+33); B = L;
  CYC(b_+33, b_+34); alu_add_hl(gb, HL);
  CYC(b_+34, b_+35); mem_wr(gb, BC, A);
  CYC(b_+35, b_+36); B = A;
  if (!(F & FC)) { CYCT(b_+36, b_+38); } else { CYC(b_+36, b_+38); } // jr nc,$4d5e (both sides land here)
  CYC(b_+38, b_+39); alu_rlca(gb);
  CALL_ASM_RST(b_+39, 0x0038, SYM(waveformTable));
  HANDOFF(SYM(waveformTable));
}

// 39:4010, bare global.
// @param a Volume (0-3)
void b39_updateMusicVolume_hook(GB *gb) {
  BASE(b39_updateMusicVolume);
  CYC(b_+0, b_+3); updateMusicVolume_hook(gb); return; // jp
}

// 39:4015, bare global (aliased here as initSound_b39 since bank0's initSound already exists).
void initSound_b39_hook(GB *gb) {
  BASE(initSound_b39);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); mem_wr(gb, hSoundDataBaseBank, A);
  CALL_C(b_+2, stopSound_hook, SYM(stopSound), b_+5);
  CYC(b_+5, b_+7); A = 0x03;
  CYC(b_+7, b_+10); mem_wr(gb, wMusicVolume, A);
  CYC(b_+10, b_+12); A = 0x00;
  CYC(b_+12, b_+15); mem_wr(gb, wSoundFadeDirection, A);
  CYC(b_+15, b_+18); mem_wr(gb, wSoundFadeCounter, A);
  CYC(b_+18, b_+21); mem_wr(gb, wSoundDisabled, A);
  CYC(b_+21, b_+24); mem_wr(gb, wMusicMuted, A);
  CYC(b_+24, b_+26); A = 0x8f;
  CYC(b_+26, b_+28); mem_wr(gb, 0xff26, A); // NR52
  CYC(b_+28, b_+30); A = 0x77;
  CYC(b_+30, b_+33); mem_wr(gb, wSoundVolume, A);
  CYC(b_+33, b_+35); mem_wr(gb, 0xff24, A); // NR50
  CYC(b_+35, b_+37); A = 0xff;
  CYC(b_+37, b_+39); mem_wr(gb, 0xff25, A); // NR51
  CYC(b_+39, b_+41); C = 0x14;
  CYC(b_+41, b_+44); SET_HL(b_+54); // @readFunction
  CYC(b_+44, b_+47); SET_DE(wMusicReadFunction);

copyLoop:
  CYC(b_+47, b_+48); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+48, b_+49); mem_wr(gb, DE, A);
  CYC(b_+49, b_+50); SET_DE(DE + 1);
  CYC(b_+50, b_+51); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(b_+51, b_+53); goto copyLoop; } // jr nz
  CYC(b_+51, b_+53);
  RET(b_+53); return; // ret
}

// 39:404b, @-local sub-label of initSound_b39. This block's bytes are copied to
// wMusicReadFunction and later executed there by wMusicReadFunction_hook.
void initSound__readFunction_b39_hook(GB *gb) {
  BASE(initSound_b39);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+54, b_+56); mem_wr(gb, hSoundDataBaseBank2, A);
  CYC(b_+56, b_+59); mem_wr(gb, 0x2000, A);
  CYC(b_+59, b_+60); A = mem_rd(gb, HL); SET_HL(HL + 1); // ld a,(hl+)
  CYC(b_+60, b_+61); C = A;
  CYC(b_+61, b_+63); A = mem_rd(gb, hSoundDataBaseBank);
  CYC(b_+63, b_+65); mem_wr(gb, hSoundDataBaseBank2, A);
  CYC(b_+65, b_+68); mem_wr(gb, 0x2000, A);
  CYC(b_+68, b_+69); A = C;
  RET(b_+69); return; // ret
}

// 39:405d, bare global.
// @param a Volume (0-3)
void updateMusicVolume_hook(GB *gb) {
  BASE(initSound_b39);
  uint16_t sp0_ = gb->sp;
  PUSH(b_+72, BC);
  PUSH(b_+73, DE);
  PUSH(b_+74, HL);
  PUSH(b_+75, AF);
  CALL_C(b_+76, silenceSquareMusicChannels_hook, SYM(silenceSquareMusicChannels), b_+79);
  SET_AF(POP(b_+79));
  CYC(b_+80, b_+83); mem_wr(gb, wMusicVolume, A);
  CYC(b_+83, b_+85); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(b_+85, b_+87); goto notMuted; } // jr nz
  CYC(b_+85, b_+87);
  CYC(b_+87, b_+89); A = 0x01;
  CYC(b_+89, b_+91); goto storeMuted; // jr

notMuted:
  CYC(b_+91, b_+93); A = 0x00;

storeMuted:
  CYC(b_+93, b_+96); mem_wr(gb, wMusicMuted, A);
  SET_HL(POP(b_+96));
  SET_DE(POP(b_+97));
  SET_BC(POP(b_+98));
  RET(b_+99); return; // ret
}

// 39:4079, bare global. Silences channels 0 and 1 if enabled.
void silenceSquareMusicChannels_hook(GB *gb) {
  BASE(silenceSquareMusicChannels);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x00;
  CYC(b_+2, b_+5); mem_wr(gb, wSoundChannel, A);
  CYC(b_+5, b_+8); SET_HL(wChannelsEnabled);
  CYC(b_+8, b_+11); A = mem_rd(gb, wSoundChannel);
  CYC(b_+11, b_+12); E = A;
  CYC(b_+12, b_+14); D = 0x00;
  CYC(b_+14, b_+15); alu_add_hl(gb, DE);
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+18); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+18, b_+20); goto square2; } // jr z
  CYC(b_+18, b_+20);
  CALL_C(b_+20, silencePlayedSound_hook, SYM(silencePlayedSound), b_+23);

square2:
  CYC(b_+23, b_+25); A = 0x01;
  CYC(b_+25, b_+28); mem_wr(gb, wSoundChannel, A);
  CYC(b_+28, b_+31); SET_HL(wChannelsEnabled);
  CYC(b_+31, b_+34); A = mem_rd(gb, wSoundChannel);
  CYC(b_+34, b_+35); E = A;
  CYC(b_+35, b_+37); D = 0x00;
  CYC(b_+37, b_+38); alu_add_hl(gb, DE);
  CYC(b_+38, b_+39); A = mem_rd(gb, HL);
  CYC(b_+39, b_+41); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+41, b_+43); goto ret; } // jr z
  CYC(b_+41, b_+43);
  CALL_C(b_+43, silencePlayedSound_hook, SYM(silencePlayedSound), b_+46);

ret:
  RET(b_+46); return; // ret
}

// 39:40a8, bare global.
void stopSound_hook(GB *gb) {
  BASE(stopSound);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x00;

loop:
  CYC(b_+2, b_+5); mem_wr(gb, wSoundChannel, A);
  CALL_C(b_+5, channelCmdff_hook, SYM(channelCmdff), b_+8);
  CYC(b_+8, b_+11); A = mem_rd(gb, wSoundChannel);
  CYC(b_+11, b_+12); A = alu_inc8(gb, A);
  CYC(b_+12, b_+14); alu_cp(gb, 0x08);
  if (!(F & FZ)) { CYCT(b_+14, b_+16); goto loop; } // jr nz
  CYC(b_+14, b_+16);
  RET(b_+16); return; // ret
}

// 39:40b9, bare global.
void silenceAllChannels_hook(GB *gb) {
  BASE(silenceAllChannels);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x00;

loop:
  CYC(b_+2, b_+5); mem_wr(gb, wSoundChannel, A);
  CALL_C(b_+5, silencePlayedSound_hook, SYM(silencePlayedSound), b_+8);
  CYC(b_+8, b_+11); A = mem_rd(gb, wSoundChannel);
  CYC(b_+11, b_+12); A = alu_inc8(gb, A);
  CYC(b_+12, b_+14); alu_cp(gb, 0x08);
  if (!(F & FZ)) { CYCT(b_+14, b_+16); goto loop; } // jr nz
  CYC(b_+14, b_+16);
  RET(b_+16); return; // ret
}

// 39:40ca, bare global. Disable all sound effect channels.
void stopSfx_hook(GB *gb) {
  BASE(stopSfx);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x02;
  CYC(b_+2, b_+5); mem_wr(gb, wSoundChannel, A);
  CYC(b_+5, b_+8); SET_HL(wChannelsEnabled);
  CYC(b_+8, b_+11); A = mem_rd(gb, wSoundChannel);
  CYC(b_+11, b_+12); E = A;
  CYC(b_+12, b_+14); D = 0x00;
  CYC(b_+14, b_+15); alu_add_hl(gb, DE);
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+18); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+18, b_+20); goto square2; } // jr z
  CYC(b_+18, b_+20);
  CALL_C(b_+20, channelCmdff_hook, SYM(channelCmdff), b_+23);

square2:
  CYC(b_+23, b_+25); A = 0x03;
  CYC(b_+25, b_+28); mem_wr(gb, wSoundChannel, A);
  CYC(b_+28, b_+31); SET_HL(wChannelsEnabled);
  CYC(b_+31, b_+34); A = mem_rd(gb, wSoundChannel);
  CYC(b_+34, b_+35); E = A;
  CYC(b_+35, b_+37); D = 0x00;
  CYC(b_+37, b_+38); alu_add_hl(gb, DE);
  CYC(b_+38, b_+39); A = mem_rd(gb, HL);
  CYC(b_+39, b_+41); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+41, b_+43); goto wave; } // jr z
  CYC(b_+41, b_+43);
  CALL_C(b_+43, channelCmdff_hook, SYM(channelCmdff), b_+46);

wave:
  CYC(b_+46, b_+48); A = 0x05;
  CYC(b_+48, b_+51); mem_wr(gb, wSoundChannel, A);
  CYC(b_+51, b_+54); SET_HL(wChannelsEnabled);
  CYC(b_+54, b_+57); A = mem_rd(gb, wSoundChannel);
  CYC(b_+57, b_+58); E = A;
  CYC(b_+58, b_+60); D = 0x00;
  CYC(b_+60, b_+61); alu_add_hl(gb, DE);
  CYC(b_+61, b_+62); A = mem_rd(gb, HL);
  CYC(b_+62, b_+64); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+64, b_+66); goto noise; } // jr z
  CYC(b_+64, b_+66);
  CALL_C(b_+66, channelCmdff_hook, SYM(channelCmdff), b_+69);

noise:
  CYC(b_+69, b_+71); A = 0x07;
  CYC(b_+71, b_+74); mem_wr(gb, wSoundChannel, A);
  CYC(b_+74, b_+77); SET_HL(wChannelsEnabled);
  CYC(b_+77, b_+80); A = mem_rd(gb, wSoundChannel);
  CYC(b_+80, b_+81); E = A;
  CYC(b_+81, b_+83); D = 0x00;
  CYC(b_+83, b_+84); alu_add_hl(gb, DE);
  CYC(b_+84, b_+85); A = mem_rd(gb, HL);
  CYC(b_+85, b_+87); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+87, b_+89); goto ret; } // jr z
  CYC(b_+87, b_+89);
  CALL_C(b_+89, channelCmdff_hook, SYM(channelCmdff), b_+92);

ret:
  RET(b_+92); return; // ret
}

// 39:4127, bare global.
void updateSound_hook(GB *gb) {
  BASE(updateSound);
  uint16_t sp0_ = gb->sp;
  PUSH(b_+0, BC);
  PUSH(b_+1, DE);
  PUSH(b_+2, HL);
  CYC(b_+3, b_+6); A = mem_rd(gb, wSoundDisabled);
  CYC(b_+6, b_+8); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+8, b_+10); goto notDisabled; } // jr z
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+13); TAIL(updateSound__ret); // jp

notDisabled:
  CYC(b_+13, b_+16); A = mem_rd(gb, wSoundVolume);
  CYC(b_+16, b_+18); mem_wr(gb, 0xff24, A); // NR50
  CYC(b_+18, b_+21); A = mem_rd(gb, wSoundFadeDirection);
  CYC(b_+21, b_+23); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+23, b_+25); updateSound__updateChannels_hook(gb); return; } // jr z
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+28); A = mem_rd(gb, wSoundFadeSpeed);
  CYC(b_+28, b_+29); B = A;
  CYC(b_+29, b_+32); A = mem_rd(gb, wSoundFadeCounter);
  CYC(b_+32, b_+33); A = alu_inc8(gb, A);
  CYC(b_+33, b_+36); mem_wr(gb, wSoundFadeCounter, A);
  CYC(b_+36, b_+37); alu_and(gb, B);
  CYC(b_+37, b_+38); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+38, b_+40); updateSound__updateChannels_hook(gb); return; } // jr nz
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+43); A = mem_rd(gb, wSoundFadeDirection);
  CYC(b_+43, b_+45); alu_cp(gb, 0x0a);
  if (F & FZ) { CYCT(b_+45, b_+47); updateSound__incVolume_hook(gb); return; } // jr z
  CYC(b_+45, b_+47);
  TAIL(updateSound__decVolume); // fallthrough
}

// 39:4156, @-local sub-label of updateSound.
void updateSound__decVolume_hook(GB *gb) {
  BASE(updateSound);
  uint16_t sp0_ = gb->sp;
  CYC(b_+47, b_+50); A = mem_rd(gb, wSoundVolume);
  CYC(b_+50, b_+52); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+52, b_+54); updateSound__stopSound_hook(gb); return; } // jr z
  CYC(b_+52, b_+54);
  CYC(b_+54, b_+56); alu_sub(gb, 0x11);
  CYC(b_+56, b_+59); mem_wr(gb, wSoundVolume, A);
  CYC(b_+59, b_+62); TAIL(updateSound__updateChannels); // jp
}

// 39:4165, @-local sub-label of updateSound.
void updateSound__incVolume_hook(GB *gb) {
  BASE(updateSound);
  uint16_t sp0_ = gb->sp;
  CYC(b_+62, b_+65); A = mem_rd(gb, wSoundVolume);
  CYC(b_+65, b_+67); alu_cp(gb, 0x77);
  if (F & FZ) { CYCT(b_+67, b_+69); updateSound__clearFadeVariables_hook(gb); return; } // jr z
  CYC(b_+67, b_+69);
  CYC(b_+69, b_+71); alu_add(gb, 0x11);
  CYC(b_+71, b_+74); mem_wr(gb, wSoundVolume, A);
  CYC(b_+74, b_+77); TAIL(updateSound__updateChannels); // jp
}

// 39:4174, @-local sub-label of updateSound.
void updateSound__stopSound_hook(GB *gb) {
  BASE(updateSound);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+77, stopSound_hook, SYM(stopSound), b_+80);
  TAIL(updateSound__clearFadeVariables); // fallthrough
}

// 39:4177, @-local sub-label of updateSound.
void updateSound__clearFadeVariables_hook(GB *gb) {
  BASE(updateSound);
  uint16_t sp0_ = gb->sp;
  CYC(b_+80, b_+82); A = 0x00;
  CYC(b_+82, b_+85); mem_wr(gb, wSoundFadeCounter, A);
  CYC(b_+85, b_+88); mem_wr(gb, wSoundFadeDirection, A);
  TAIL(updateSound__updateChannels); // fallthrough
}

// 39:417f, @-local sub-label of updateSound.
void updateSound__updateChannels_hook(GB *gb) {
  BASE(updateSound);
  uint16_t sp0_ = gb->sp;
  CYC(b_+88, b_+90); A = 0x00;
  TAIL(updateSound__channelLoop); // fallthrough
}

// 39:4181, @-local sub-label of updateSound.
void updateSound__channelLoop_hook(GB *gb) {
  BASE(updateSound);
  uint16_t sp0_ = gb->sp;
  CYC(b_+90, b_+93); mem_wr(gb, wSoundChannel, A);
  CYC(b_+93, b_+96); SET_HL(wChannelsEnabled);
  CYC(b_+96, b_+99); A = mem_rd(gb, wSoundChannel);
  CYC(b_+99, b_+100); E = A;
  CYC(b_+100, b_+102); D = 0x00;
  CYC(b_+102, b_+103); alu_add_hl(gb, DE);
  CYC(b_+103, b_+104); A = mem_rd(gb, HL);
  CYC(b_+104, b_+106); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+106, b_+108); updateSound__nextChannel_hook(gb); return; } // jr z
  CYC(b_+106, b_+108);
  CYC(b_+108, b_+111); SET_HL(wChannelWaitCounters);
  CYC(b_+111, b_+114); A = mem_rd(gb, wSoundChannel);
  CYC(b_+114, b_+115); E = A;
  CYC(b_+115, b_+117); D = 0x00;
  CYC(b_+117, b_+118); alu_add_hl(gb, DE);
  CYC(b_+118, b_+119); A = mem_rd(gb, HL);
  CYC(b_+119, b_+121); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(b_+121, b_+123); updateSound__continueSound_hook(gb); return; } // jr nz
  CYC(b_+121, b_+123);
  CALL_C(b_+123, doNextChannelCommand_hook, SYM(doNextChannelCommand), b_+126);
  CYC(b_+126, b_+128); TAIL(updateSound__nextChannel); // jr
}

// 39:41a7, @-local sub-label of updateSound.
void updateSound__continueSound_hook(GB *gb) {
  BASE(updateSound);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+128, continuePlayingSound_hook, SYM(continuePlayingSound), b_+131);
  TAIL(updateSound__nextChannel); // fallthrough
}

// 39:41aa, @-local sub-label of updateSound.
void updateSound__nextChannel_hook(GB *gb) {
  BASE(updateSound);
  uint16_t sp0_ = gb->sp;
  CYC(b_+131, b_+134); A = mem_rd(gb, wSoundChannel);
  CYC(b_+134, b_+135); A = alu_inc8(gb, A);
  CYC(b_+135, b_+137); alu_cp(gb, 0x08);
  if (!(F & FZ)) { CYCT(b_+137, b_+139); updateSound__channelLoop_hook(gb); return; } // jr nz
  CYC(b_+137, b_+139);
  CYC(b_+139, b_+142); A = mem_rd(gb, wMusicMuted);
  CYC(b_+142, b_+144); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+144, b_+146); updateSound__ret_hook(gb); return; } // jr nz
  CYC(b_+144, b_+146);
  CYC(b_+146, b_+148); A = 0x02;
  CYC(b_+148, b_+151); mem_wr(gb, wMusicMuted, A);
  TAIL(updateSound__ret); // fallthrough
}

// 39:41be, @-local sub-label of updateSound.
void updateSound__ret_hook(GB *gb) {
  BASE(updateSound);
  uint16_t sp0_ = gb->sp;
  SET_HL(POP(b_+151));
  SET_DE(POP(b_+152));
  SET_BC(POP(b_+153));
  RET(b_+154); return; // ret
}

// 39:41c2, bare global. Keep playing the current sound.
void continuePlayingSound_hook(GB *gb) {
  BASE(continuePlayingSound);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(wChannelWaitCounters);
  CYC(b_+3, b_+6); A = mem_rd(gb, wSoundChannel);
  CYC(b_+6, b_+7); E = A;
  CYC(b_+7, b_+9); D = 0x00;
  CYC(b_+9, b_+10); alu_add_hl(gb, DE);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+12); A = alu_dec8(gb, A);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A);
  CYC(b_+13, b_+16); A = mem_rd(gb, wSoundChannel);
  CYC(b_+16, b_+18); alu_cp(gb, 0x06);
  if (!(F & FC)) { CYCT(b_+18, b_+20); continuePlayingSound__ret_hook(gb); return; } // jr nc
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+23); SET_HL(wChannelFrequencyModeAndLengthTimerEnabled);
  CYC(b_+23, b_+26); A = mem_rd(gb, wSoundChannel);
  CYC(b_+26, b_+27); E = A;
  CYC(b_+27, b_+29); D = 0x00;
  CYC(b_+29, b_+30); alu_add_hl(gb, DE);
  CYC(b_+30, b_+31); A = mem_rd(gb, HL);
  CYC(b_+31, b_+33); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(b_+33, b_+35); continuePlayingSound__ret_hook(gb); return; } // jr nz
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+38); A = mem_rd(gb, wSoundChannel);
  CYC(b_+38, b_+40); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(b_+40, b_+42); goto skipEnvelopes; } // jr nc
  CYC(b_+40, b_+42);
  CALL_C(b_+42, handleEnvelopes_hook, SYM(handleEnvelopes), b_+45);

skipEnvelopes:
  CALL_C(b_+45, updateSoundFrequencyAndPlay_hook, SYM(updateSoundFrequencyAndPlay), b_+48);
  TAIL(continuePlayingSound__ret); // fallthrough
}

// 39:41f2, @-local sub-label of continuePlayingSound.
void continuePlayingSound__ret_hook(GB *gb) {
  BASE(continuePlayingSound);
  uint16_t sp0_ = gb->sp;
  RET(b_+48); return; // ret
}

// 39:41f3, bare global. Copies the channel's frequency value from hSoundData3 to
// wSoundFrequencyL,H after applying sweep and vibrato.
void updateSoundFrequencyAndPlay_hook(GB *gb) {
  BASE(updateSoundFrequencyAndPlay);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(wChannelSweep);
  CYC(b_+3, b_+6); A = mem_rd(gb, wSoundChannel);
  CYC(b_+6, b_+7); E = A;
  CYC(b_+7, b_+9); D = 0x00;
  CYC(b_+9, b_+10); alu_add_hl(gb, DE);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+12); C = A;
  CYC(b_+12, b_+14); alu_and(gb, 0x7f);
  if (F & FZ) { CYCT(b_+14, b_+16); updateSoundFrequencyAndPlay__handleVibrato_hook(gb); return; } // jr z
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+17); A = C;
  CYC(b_+17, b_+19); alu_and(gb, 0x80);
  if (!(F & FZ)) { CYCT(b_+19, b_+21); goto negativeSweep; } // jr nz
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+23); D = 0x00;
  CYC(b_+23, b_+25); goto sweepDeReady; // jr

negativeSweep:
  CYC(b_+25, b_+27); D = 0xff;

sweepDeReady:
  PUSH(b_+27, DE);
  CYC(b_+28, b_+31); SET_HL(wChannelSweep);
  CYC(b_+31, b_+34); A = mem_rd(gb, wSoundChannel);
  CYC(b_+34, b_+35); E = A;
  CYC(b_+35, b_+37); D = 0x00;
  CYC(b_+37, b_+38); alu_add_hl(gb, DE);
  CYC(b_+38, b_+39); A = mem_rd(gb, HL);
  SET_DE(POP(b_+39));
  CYC(b_+40, b_+41); E = A;
  CYC(b_+41, b_+44); A = mem_rd(gb, wSoundChannel);
  CYC(b_+44, b_+46); A = alu_sla(gb, A);
  CYC(b_+46, b_+47); B = A;
  CYC(b_+47, b_+48); A = B;
  CYC(b_+48, b_+50); alu_add(gb, hSoundData3 & 0xff);
  CYC(b_+50, b_+51); C = A;
  CYC(b_+51, b_+52); A = mem_rd(gb, 0xff00 | C);
  CYC(b_+52, b_+53); C = alu_inc8(gb, C);
  CYC(b_+53, b_+54); L = A;
  CYC(b_+54, b_+55); A = mem_rd(gb, 0xff00 | C);
  CYC(b_+55, b_+56); C = alu_inc8(gb, C);
  CYC(b_+56, b_+57); H = A;
  CYC(b_+57, b_+58); alu_add_hl(gb, DE);
  CYC(b_+58, b_+61); A = mem_rd(gb, wSoundChannel);
  CYC(b_+61, b_+63); A = alu_sla(gb, A);
  CYC(b_+63, b_+64); B = A;
  CYC(b_+64, b_+65); A = L;
  CYC(b_+65, b_+67); C = hSoundData3 & 0xff;
  CALL_C(b_+67, writeIndexedHighRamAndIncrement_hook, SYM(writeIndexedHighRamAndIncrement), b_+70);
  CYC(b_+70, b_+71); A = H;
  CYC(b_+71, b_+72); mem_wr(gb, 0xff00 | C, A);
  CYC(b_+72, b_+73); C = alu_inc8(gb, C);
  TAIL(updateSoundFrequencyAndPlay__handleVibrato); // fallthrough
}

// 39:423c, @-local sub-label of updateSoundFrequencyAndPlay.
void updateSoundFrequencyAndPlay__handleVibrato_hook(GB *gb) {
  BASE(updateSoundFrequencyAndPlay);
  uint16_t sp0_ = gb->sp;
  CYC(b_+73, b_+76); SET_HL(wChannelVibratoActive);
  CYC(b_+76, b_+79); A = mem_rd(gb, wSoundChannel);
  CYC(b_+79, b_+80); E = A;
  CYC(b_+80, b_+82); D = 0x00;
  CYC(b_+82, b_+83); alu_add_hl(gb, DE);
  CYC(b_+83, b_+84); A = mem_rd(gb, HL);
  CYC(b_+84, b_+86); alu_and(gb, 0x10);
  if (!(F & FZ)) { CYCT(b_+86, b_+88); updateSoundFrequencyAndPlay__useVibrato_hook(gb); return; } // jr nz
  CYC(b_+86, b_+88);
  CYC(b_+88, b_+91); SET_HL(wChannelVibratoCounters);
  CYC(b_+91, b_+94); A = mem_rd(gb, wSoundChannel);
  CYC(b_+94, b_+95); E = A;
  CYC(b_+95, b_+97); D = 0x00;
  CYC(b_+97, b_+98); alu_add_hl(gb, DE);
  CYC(b_+98, b_+99); A = mem_rd(gb, HL);
  CYC(b_+99, b_+101); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+101, b_+103); updateSoundFrequencyAndPlay__endVibratoWait_hook(gb); return; } // jr z
  CYC(b_+101, b_+103);
  CYC(b_+103, b_+104); A = alu_dec8(gb, A);
  CYC(b_+104, b_+107); SET_HL(wChannelVibratoCounters);
  PUSH(b_+107, AF);
  CYC(b_+108, b_+111); A = mem_rd(gb, wSoundChannel);
  CYC(b_+111, b_+112); E = A;
  CYC(b_+112, b_+114); D = 0x00;
  CYC(b_+114, b_+115); alu_add_hl(gb, DE);
  SET_AF(POP(b_+115));
  CYC(b_+116, b_+117); mem_wr(gb, HL, A);
  CYC(b_+117, b_+120); SET_HL(0x0000);
  CYC(b_+120, b_+123); TAIL(updateSoundFrequencyAndPlay__updateSoundFrequencyWithOffset); // jp
}

// 39:426e, @-local sub-label of updateSoundFrequencyAndPlay.
void updateSoundFrequencyAndPlay__endVibratoWait_hook(GB *gb) {
  BASE(updateSoundFrequencyAndPlay);
  uint16_t sp0_ = gb->sp;
  CYC(b_+123, b_+125); A = 0x10;
  CYC(b_+125, b_+128); SET_HL(wChannelVibratoActive);
  PUSH(b_+128, AF);
  CYC(b_+129, b_+132); A = mem_rd(gb, wSoundChannel);
  CYC(b_+132, b_+133); E = A;
  CYC(b_+133, b_+135); D = 0x00;
  CYC(b_+135, b_+136); alu_add_hl(gb, DE);
  SET_AF(POP(b_+136));
  CYC(b_+137, b_+138); mem_wr(gb, HL, A);
  CYC(b_+138, b_+140); A = 0x00;
  CYC(b_+140, b_+143); SET_HL(wChannelVibratoCounters);
  PUSH(b_+143, AF);
  CYC(b_+144, b_+147); A = mem_rd(gb, wSoundChannel);
  CYC(b_+147, b_+148); E = A;
  CYC(b_+148, b_+150); D = 0x00;
  CYC(b_+150, b_+151); alu_add_hl(gb, DE);
  SET_AF(POP(b_+151));
  CYC(b_+152, b_+153); mem_wr(gb, HL, A);
  TAIL(updateSoundFrequencyAndPlay__useVibrato); // fallthrough
}

// 39:428c, @-local sub-label of updateSoundFrequencyAndPlay.
void updateSoundFrequencyAndPlay__useVibrato_hook(GB *gb) {
  BASE(updateSoundFrequencyAndPlay);
  uint16_t sp0_ = gb->sp;
  CYC(b_+153, b_+156); SET_HL(wChannelVibratoCounters);
  CYC(b_+156, b_+159); A = mem_rd(gb, wSoundChannel);
  CYC(b_+159, b_+160); E = A;
  CYC(b_+160, b_+162); D = 0x00;
  CYC(b_+162, b_+163); alu_add_hl(gb, DE);
  CYC(b_+163, b_+164); A = mem_rd(gb, HL);
  CYC(b_+164, b_+166); alu_cp(gb, 0x08);
  if (!(F & FZ)) { CYCT(b_+166, b_+168); updateSoundFrequencyAndPlay__determineFrequencyOffset_hook(gb); return; } // jr nz
  CYC(b_+166, b_+168);
  CYC(b_+168, b_+170); A = 0x00;
  CYC(b_+170, b_+173); SET_HL(wChannelVibratoCounters);
  PUSH(b_+173, AF);
  CYC(b_+174, b_+177); A = mem_rd(gb, wSoundChannel);
  CYC(b_+177, b_+178); E = A;
  CYC(b_+178, b_+180); D = 0x00;
  CYC(b_+180, b_+181); alu_add_hl(gb, DE);
  SET_AF(POP(b_+181));
  CYC(b_+182, b_+183); mem_wr(gb, HL, A);
  CYC(b_+183, b_+185); A = 0x00;
  TAIL(updateSoundFrequencyAndPlay__determineFrequencyOffset); // fallthrough
}

// 39:42ac, @-local sub-label of updateSoundFrequencyAndPlay.
void updateSoundFrequencyAndPlay__determineFrequencyOffset_hook(GB *gb) {
  BASE(updateSoundFrequencyAndPlay);
  uint16_t sp0_ = gb->sp;
  CYC(b_+185, b_+188); SET_HL(SYM(vibratoOffsetTable)); // vibratoOffsetTable
  CALL_C(b_+188, readWordFromTable_hook, SYM(readWordFromTable), b_+191);
  PUSH(b_+191, HL);
  CYC(b_+192, b_+195); SET_HL(wChannelVibratoCounters);
  CYC(b_+195, b_+198); A = mem_rd(gb, wSoundChannel);
  CYC(b_+198, b_+199); E = A;
  CYC(b_+199, b_+201); D = 0x00;
  CYC(b_+201, b_+202); alu_add_hl(gb, DE);
  CYC(b_+202, b_+203); A = mem_rd(gb, HL);
  CYC(b_+203, b_+204); A = alu_inc8(gb, A);
  CYC(b_+204, b_+205); mem_wr(gb, HL, A);
  CYC(b_+205, b_+208); SET_HL(wChannelVibratos);
  CYC(b_+208, b_+211); A = mem_rd(gb, wSoundChannel);
  CYC(b_+211, b_+212); E = A;
  CYC(b_+212, b_+214); D = 0x00;
  CYC(b_+214, b_+215); alu_add_hl(gb, DE);
  CYC(b_+215, b_+216); A = mem_rd(gb, HL);
  CYC(b_+216, b_+218); alu_and(gb, 0x0f);
  SET_HL(POP(b_+218));
  CALL_C(b_+219, multiplyHlByA_hook, SYM(multiplyHlByA), b_+222);
  TAIL(updateSoundFrequencyAndPlay__updateSoundFrequencyWithOffset); // fallthrough
}

// 39:42d1, @-local sub-label of updateSoundFrequencyAndPlay.
void updateSoundFrequencyAndPlay__updateSoundFrequencyWithOffset_hook(GB *gb) {
  BASE(updateSoundFrequencyAndPlay);
  uint16_t sp0_ = gb->sp;
  CYC(b_+222, b_+225); A = mem_rd(gb, wSoundChannel);
  CYC(b_+225, b_+227); A = alu_sla(gb, A);
  CYC(b_+227, b_+228); B = A;
  CYC(b_+228, b_+229); A = B;
  CYC(b_+229, b_+231); alu_add(gb, hSoundData3 & 0xff);
  CYC(b_+231, b_+232); C = A;
  CYC(b_+232, b_+233); A = mem_rd(gb, 0xff00 | C);
  CYC(b_+233, b_+234); C = alu_inc8(gb, C);
  CYC(b_+234, b_+235); E = A;
  CYC(b_+235, b_+236); A = mem_rd(gb, 0xff00 | C);
  CYC(b_+236, b_+237); C = alu_inc8(gb, C);
  CYC(b_+237, b_+238); D = A;
  CYC(b_+238, b_+239); alu_add_hl(gb, DE);
  CYC(b_+239, b_+240); A = L;
  CYC(b_+240, b_+243); mem_wr(gb, wSoundFrequencyL, A);
  CYC(b_+243, b_+244); A = H;
  CYC(b_+244, b_+247); mem_wr(gb, wSoundFrequencyH, A);
  TAIL(updatePlayedFrequency); // fallthrough
}

// 39:42ea, bare global. When used for the wave channel, hl is expected to contain
// wSoundFrequencyL,H and is written to NR33 and NR34.
void updatePlayedFrequency_hook(GB *gb) {
  BASE(updatePlayedFrequency);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wSoundChannel);
  CYC(b_+3, b_+4); alu_scf(gb);
  CYC(b_+4, b_+5); alu_ccf(gb);
  CYC(b_+5, b_+7); alu_cp(gb, 0x04);
  if (!(F & FC)) { CYCT(b_+7, b_+9); updatePlayedFrequency__wave_hook(gb); return; } // jr nc
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(b_+11, b_+13); updatePlayedFrequency__square_hook(gb); return; } // jr nc
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); A = alu_inc8(gb, A);
  CYC(b_+14, b_+15); A = alu_inc8(gb, A);
  CYC(b_+15, b_+16); E = A;
  CYC(b_+16, b_+19); SET_HL(wChannelsEnabled);
  CYC(b_+19, b_+21); D = 0x00;
  CYC(b_+21, b_+22); alu_add_hl(gb, DE);
  CYC(b_+22, b_+23); A = mem_rd(gb, HL);
  CYC(b_+23, b_+25); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+25, b_+27); updatePlayedFrequency__square_hook(gb); return; } // jr z
  CYC(b_+25, b_+27);
  RET(b_+27); return; // ret
}

// 39:4306, @-local sub-label of updatePlayedFrequency.
void updatePlayedFrequency__square_hook(GB *gb) {
  BASE(updatePlayedFrequency);
  uint16_t sp0_ = gb->sp;
  CYC(b_+28, b_+31); A = mem_rd(gb, wSoundChannel);
  CYC(b_+31, b_+33); alu_and(gb, 0x01);
  CYC(b_+33, b_+34); B = A;
  CYC(b_+34, b_+36); A = alu_sla(gb, A);
  CYC(b_+36, b_+38); A = alu_sla(gb, A);
  CYC(b_+38, b_+39); alu_add(gb, B);
  CYC(b_+39, b_+40); B = A;
  PUSH(b_+40, BC);
  CYC(b_+41, b_+44); A = mem_rd(gb, wSoundFrequencyL);
  CYC(b_+44, b_+46); C = 0x13; // NR13
  CALL_C(b_+46, writeIndexedHighRamAndIncrement_hook, SYM(writeIndexedHighRamAndIncrement), b_+49);
  CYC(b_+49, b_+52); A = mem_rd(gb, wSoundCmdEnvelope);
  CYC(b_+52, b_+53); E = A;
  CYC(b_+53, b_+56); A = mem_rd(gb, wSoundFrequencyH);
  CYC(b_+56, b_+57); alu_or(gb, E);
  CYC(b_+57, b_+58); mem_wr(gb, 0xff00 | C, A); // NR14
  CYC(b_+58, b_+59); C = alu_inc8(gb, C);
  SET_BC(POP(b_+59));
  PUSH(b_+60, BC);
  CYC(b_+61, b_+64); SET_HL(wChannelDutyCycles);
  CYC(b_+64, b_+67); A = mem_rd(gb, wSoundChannel);
  CYC(b_+67, b_+68); E = A;
  CYC(b_+68, b_+70); D = 0x00;
  CYC(b_+70, b_+71); alu_add_hl(gb, DE);
  CYC(b_+71, b_+72); A = mem_rd(gb, HL);
  SET_BC(POP(b_+72));
  CYC(b_+73, b_+75); C = 0x11; // NR11
  CALL_C(b_+75, writeIndexedHighRamAndIncrement_hook, SYM(writeIndexedHighRamAndIncrement), b_+78);
  RET(b_+78); return; // ret
}

// 39:4339, @-local sub-label of updatePlayedFrequency.
void updatePlayedFrequency__wave_hook(GB *gb) {
  BASE(updatePlayedFrequency);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+79, isWaveChannelUnavailable_hook, SYM(isWaveChannelUnavailable), b_+82);
  CYC(b_+82, b_+84); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(b_+84, b_+86); updatePlayedFrequency__ret_hook(gb); return; } // jr nz
  CYC(b_+84, b_+86);
  CYC(b_+86, b_+87); A = L;
  CYC(b_+87, b_+89); mem_wr(gb, 0xff1d, A); // NR33
  CYC(b_+89, b_+90); A = H;
  CYC(b_+90, b_+92); mem_wr(gb, 0xff1e, A); // NR34
  CYC(b_+92, b_+94); A = 0x00;
  CYC(b_+94, b_+96); mem_wr(gb, 0xff1b, A); // NR31
  TAIL(updatePlayedFrequency__ret); // fallthrough
}

// 39:434a, @-local sub-label of updatePlayedFrequency.
void updatePlayedFrequency__ret_hook(GB *gb) {
  BASE(updatePlayedFrequency);
  uint16_t sp0_ = gb->sp;
  RET(b_+96); return; // ret
}

// 39:434b, bare global. Sounds can always play; music can only play if the wave channel
// is free and music has not been muted since the previous updateSound call.
// @param[out] a Whether wave channel registers may be written to (0 or 1)
void isWaveChannelUnavailable_hook(GB *gb) {
  BASE(isWaveChannelUnavailable);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wSoundChannel);
  CYC(b_+3, b_+5); alu_cp(gb, 0x05);
  if (F & FZ) { CYCT(b_+5, b_+7); isWaveChannelUnavailable__available_hook(gb); return; } // jr z
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); A = mem_rd(gb, wChannelsEnabled + 5); // wave channel's slot
  CYC(b_+10, b_+12); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(b_+12, b_+14); isWaveChannelUnavailable__unavailable_hook(gb); return; } // jr nz
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+17); A = mem_rd(gb, wMusicMuted);
  CYC(b_+17, b_+19); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+19, b_+21); isWaveChannelUnavailable__unavailable_hook(gb); return; } // jr z
  CYC(b_+19, b_+21);
  TAIL(isWaveChannelUnavailable__available); // fallthrough
}

// 39:4360, @-local sub-label of isWaveChannelUnavailable.
void isWaveChannelUnavailable__available_hook(GB *gb) {
  BASE(isWaveChannelUnavailable);
  uint16_t sp0_ = gb->sp;
  CYC(b_+21, b_+23); A = 0x00;
  RET(b_+23); return; // ret
}

// 39:4363, @-local sub-label of isWaveChannelUnavailable.
void isWaveChannelUnavailable__unavailable_hook(GB *gb) {
  BASE(isWaveChannelUnavailable);
  uint16_t sp0_ = gb->sp;
  CYC(b_+24, b_+26); A = 0x01;
  RET(b_+26); return; // ret
}

// 39:4366, bare global.
void getNextChannelByte_hook(GB *gb) {
  BASE(getNextChannelByte);
  uint16_t sp0_ = gb->sp;
  PUSH(b_+0, BC);
  PUSH(b_+1, DE);
  PUSH(b_+2, HL);
  CYC(b_+3, b_+6); A = mem_rd(gb, wSoundChannel);
  CYC(b_+6, b_+8); A = alu_sla(gb, A);
  CYC(b_+8, b_+10); alu_add(gb, hSoundChannelAddresses & 0xff);
  CYC(b_+10, b_+11); C = A;
  CYC(b_+11, b_+12); A = mem_rd(gb, 0xff00 | C);
  CYC(b_+12, b_+13); C = alu_inc8(gb, C);
  CYC(b_+13, b_+14); L = A;
  CYC(b_+14, b_+15); A = mem_rd(gb, 0xff00 | C);
  CYC(b_+15, b_+16); H = A;
  CYC(b_+16, b_+19); A = mem_rd(gb, wSoundChannel);
  CYC(b_+19, b_+21); alu_add(gb, hSoundChannelBanks & 0xff);
  CYC(b_+21, b_+22); C = A;
  CYC(b_+22, b_+23); A = mem_rd(gb, 0xff00 | C);
  CYC(b_+23, b_+24); C = alu_inc8(gb, C);
  CALL_C(b_+24, wMusicReadFunction_hook, wMusicReadFunction, b_+27);
  PUSH(b_+27, AF);
  CYC(b_+28, b_+31); A = mem_rd(gb, wSoundChannel);
  CYC(b_+31, b_+33); A = alu_sla(gb, A);
  CYC(b_+33, b_+34); B = A;
  CYC(b_+34, b_+35); A = L;
  CYC(b_+35, b_+37); C = hSoundChannelAddresses & 0xff;
  CALL_C(b_+37, writeIndexedHighRamAndIncrement_hook, SYM(writeIndexedHighRamAndIncrement), b_+40);
  CYC(b_+40, b_+41); A = H;
  CYC(b_+41, b_+42); mem_wr(gb, 0xff00 | C, A);
  CYC(b_+42, b_+43); C = alu_inc8(gb, C);
  SET_AF(POP(b_+43));
  SET_HL(POP(b_+44));
  SET_DE(POP(b_+45));
  SET_BC(POP(b_+46));
  RET(b_+47); return; // ret
}

// 39:4396, bare global.
void doNextChannelCommand_hook(GB *gb) {
  BASE(doNextChannelCommand);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getNextChannelByte_hook, SYM(getNextChannelByte), b_+3);
  CYC(b_+3, b_+4); alu_scf(gb);
  CYC(b_+4, b_+5); alu_ccf(gb);
  CYC(b_+5, b_+7); alu_cp(gb, 0xf0);
  if (!(F & FC)) { CYCT(b_+7, b_+9); doNextChannelCommand__cmdf0Toff_hook(gb); return; } // jr nc
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); alu_scf(gb);
  CYC(b_+10, b_+11); alu_ccf(gb);
  CYC(b_+11, b_+13); alu_cp(gb, 0xe0);
  if (F & FC) { CYCT(b_+13, b_+15); goto lowRange; } // jr c
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+18); TAIL(cmde0Toef); // jp

lowRange:
  CYC(b_+18, b_+19); alu_scf(gb);
  CYC(b_+19, b_+20); alu_ccf(gb);
  CYC(b_+20, b_+22); alu_cp(gb, 0xd0);
  if (F & FC) { CYCT(b_+22, b_+24); goto standardRange; } // jr c
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+27); TAIL(cmdVolume); // jp

standardRange:
  CYC(b_+27, b_+30); mem_wr(gb, wSoundCmd, A);
  CYC(b_+30, b_+33); TAIL(standardSoundCmd); // jp
}

// 39:43b7, @-local sub-label of doNextChannelCommand.
void doNextChannelCommand__cmdf0Toff_hook(GB *gb) {
  BASE(doNextChannelCommand);
  uint16_t sp0_ = gb->sp;
  CYC(b_+33, b_+34); E = A;
  CYC(b_+34, b_+36); A = 0xff;
  CYC(b_+36, b_+37); alu_sub(gb, E);
  CYC(b_+37, b_+40); SET_HL(b_+44); // doNextChannelCommand@table
  CALL_C(b_+40, readWordFromTable_hook, SYM(readWordFromTable), b_+43);
  CYC(b_+43, b_+44);
  {
    uint16_t target = HL;
    if (target == SYM(channelCmdff)) { channelCmdff_hook(gb); return; }
    if (target == SYM(channelCmdfe)) { channelCmdfe_hook(gb); return; }
    if (target == SYM(channelCmdfd)) { channelCmdfd_hook(gb); return; }
    if (target == SYM(channelCmdf9)) { channelCmdf9_hook(gb); return; }
    if (target == SYM(channelCmdf8)) { channelCmdf8_hook(gb); return; }
    if (target == SYM(channelCmdf6)) { channelCmdf6_hook(gb); return; }
    if (target == SYM(channelCmdf3)) { channelCmdf3_hook(gb); return; }
    if (target == SYM(channelCmdf2)) { channelCmdf2_hook(gb); return; }
    if (target == SYM(channelCmdf1)) { channelCmdf1_hook(gb); return; }
    if (target == SYM(channelCmdf0)) { channelCmdf0_hook(gb); return; }
    HANDOFF(target);
  }
}

// 39:43e2, bare global.
void channelCmdf1_hook(GB *gb) {
  BASE(channelCmdf1);
  CYC(b_+0, b_+3); TAIL(doNextChannelCommand); // jp
}

// 39:43e5, bare global.
void channelCmdf2_hook(GB *gb) {
  BASE(channelCmdf2);
  CYC(b_+0, b_+3); TAIL(doNextChannelCommand); // jp
}

// 39:43e8, bare global.
void channelCmdf3_hook(GB *gb) {
  BASE(channelCmdf3);
  CYC(b_+0, b_+3); TAIL(doNextChannelCommand); // jp
}

// 39:43eb, bare global. Vibrato: sets vibrato to the argument value, does nothing for
// noise channels.
void channelCmdf9_hook(GB *gb) {
  BASE(channelCmdf9);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wSoundChannel);
  CYC(b_+3, b_+4); alu_scf(gb);
  CYC(b_+4, b_+5); alu_ccf(gb);
  CYC(b_+5, b_+7); alu_cp(gb, 0x06);
  if (!(F & FC)) { CYCT(b_+7, b_+9); goto skip; } // jr nc
  CYC(b_+7, b_+9);
  CALL_C(b_+9, getNextChannelByte_hook, SYM(getNextChannelByte), b_+12);
  CYC(b_+12, b_+15); SET_HL(wChannelVibratos);
  PUSH(b_+15, AF);
  CYC(b_+16, b_+19); A = mem_rd(gb, wSoundChannel);
  CYC(b_+19, b_+20); E = A;
  CYC(b_+20, b_+22); D = 0x00;
  CYC(b_+22, b_+23); alu_add_hl(gb, DE);
  SET_AF(POP(b_+23));
  CYC(b_+24, b_+25); mem_wr(gb, HL, A);
  CYC(b_+25, b_+28); TAIL(doNextChannelCommand); // jp

skip:
  CALL_C((SYM(channelCmdfd) + 28), getNextChannelByte_hook, SYM(getNextChannelByte), (SYM(channelCmdfd) + 31));
  CYC((SYM(channelCmdfd) + 31), (SYM(channelCmdfd) + 34)); TAIL(doNextChannelCommand); // jp
}

// 39:4407, bare global. Sets sweep to the argument value, does nothing for noise channels.
void channelCmdf8_hook(GB *gb) {
  BASE(channelCmdf8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wSoundChannel);
  CYC(b_+3, b_+4); alu_scf(gb);
  CYC(b_+4, b_+5); alu_ccf(gb);
  CYC(b_+5, b_+7); alu_cp(gb, 0x06);
  if (!(F & FC)) { CYCT(b_+7, b_+9); goto skip; } // jr nc
  CYC(b_+7, b_+9);
  CALL_C(b_+9, getNextChannelByte_hook, SYM(getNextChannelByte), b_+12);
  CYC(b_+12, b_+15); SET_HL(wChannelSweep);
  PUSH(b_+15, AF);
  CYC(b_+16, b_+19); A = mem_rd(gb, wSoundChannel);
  CYC(b_+19, b_+20); E = A;
  CYC(b_+20, b_+22); D = 0x00;
  CYC(b_+22, b_+23); alu_add_hl(gb, DE);
  SET_AF(POP(b_+23));
  CYC(b_+24, b_+25); mem_wr(gb, HL, A);
  CYC(b_+25, b_+28); TAIL(doNextChannelCommand); // jp

skip:
  CALL_C((SYM(channelCmdfd) + 28), getNextChannelByte_hook, SYM(getNextChannelByte), (SYM(channelCmdfd) + 31));
  CYC((SYM(channelCmdfd) + 31), (SYM(channelCmdfd) + 34)); TAIL(doNextChannelCommand); // jp
}

// 39:4423, bare global. Sets pitch shift to the argument value, does nothing for noise
// channels.
void channelCmdfd_hook(GB *gb) {
  BASE(channelCmdfd);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wSoundChannel);
  CYC(b_+3, b_+4); alu_scf(gb);
  CYC(b_+4, b_+5); alu_ccf(gb);
  CYC(b_+5, b_+7); alu_cp(gb, 0x06);
  if (!(F & FC)) { CYCT(b_+7, b_+9); goto skip; } // jr nc
  CYC(b_+7, b_+9);
  CALL_C(b_+9, getNextChannelByte_hook, SYM(getNextChannelByte), b_+12);
  CYC(b_+12, b_+15); SET_HL(wChannelPitchShift);
  PUSH(b_+15, AF);
  CYC(b_+16, b_+19); A = mem_rd(gb, wSoundChannel);
  CYC(b_+19, b_+20); E = A;
  CYC(b_+20, b_+22); D = 0x00;
  CYC(b_+22, b_+23); alu_add_hl(gb, DE);
  SET_AF(POP(b_+23));
  CYC(b_+24, b_+25); mem_wr(gb, HL, A);
  CYC(b_+25, b_+28); TAIL(doNextChannelCommand); // jp

skip:
  CALL_C(b_+28, getNextChannelByte_hook, SYM(getNextChannelByte), b_+31);
  CYC(b_+31, b_+34); TAIL(doNextChannelCommand); // jp
}

// 39:4445, bare global. Sets the channel envelopes to the lower 3 bits of the command
// value (note start) and the argument value (note end). Should not be used with wave or
// noise channels or else wChannelEnvelopes2 and wChannelsEnabled get messed up for square
// channels.
void cmde0Toef_hook(GB *gb) {
  BASE(cmde0Toef);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); alu_and(gb, 0x07);
  CYC(b_+2, b_+5); SET_HL(wChannelEnvelopes);
  PUSH(b_+5, AF);
  CYC(b_+6, b_+9); A = mem_rd(gb, wSoundChannel);
  CYC(b_+9, b_+10); E = A;
  CYC(b_+10, b_+12); D = 0x00;
  CYC(b_+12, b_+13); alu_add_hl(gb, DE);
  SET_AF(POP(b_+13));
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CALL_C(b_+15, getNextChannelByte_hook, SYM(getNextChannelByte), b_+18);
  CYC(b_+18, b_+20); alu_and(gb, 0x07);
  CYC(b_+20, b_+23); SET_HL(wChannelEnvelopes2);
  PUSH(b_+23, AF);
  CYC(b_+24, b_+27); A = mem_rd(gb, wSoundChannel);
  CYC(b_+27, b_+28); E = A;
  CYC(b_+28, b_+30); D = 0x00;
  CYC(b_+30, b_+31); alu_add_hl(gb, DE);
  SET_AF(POP(b_+31));
  CYC(b_+32, b_+33); mem_wr(gb, HL, A);
  CYC(b_+33, b_+36); TAIL(doNextChannelCommand); // jp
}

// 39:4469, bare global. Command $f0 takes the next byte as argument and can do various
// things depending on the channel:
// Channels 0-5: enables a mode where standard commands are changed, and the note and beat
// macros stop working for the channel. Commands for playing a sound or rest must instead be
// of the form ".db $06 $0b $02" where, from left to right, the bytes correspond to the high
// byte of the frequency value, the low byte, and the sound length. There is no command for
// disabling this mode. The argument is written straight to wChannelDutyCycles.
// Channels 0-3: additionally, if bits 0-5 of the argument are not all cleared, the length
// timer gets enabled for the channel, and those 6 bits determine the initial value for each
// played sound. Otherwise, the length timer gets disabled. Sweep and vibrato are ignored
// with length timer. Envelopes with increasing volume should not be used with length timer
// as sounds would keep increasing to volume $f if played long enough.
// Channel 7: sets volume and envelope (argument gets written straight to NR42).
// Should not be used with channel 6 or else wChannelSweep and wChannelEnvelopeStates get
// messed up for channel 0.
void channelCmdf0_hook(GB *gb) {
  BASE(channelCmdf0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wSoundChannel);
  CYC(b_+3, b_+5); alu_cp(gb, 0x07);
  if (F & FZ) { CYCT(b_+5, b_+7); channelCmdf0__channel7_hook(gb); return; } // jr z
  CYC(b_+5, b_+7);
  CALL_C(b_+7, getNextChannelByte_hook, SYM(getNextChannelByte), b_+10);
  PUSH(b_+10, AF);
  CYC(b_+11, b_+13); alu_and(gb, 0x3f);
  if (F & FZ) { CYCT(b_+13, b_+15); channelCmdf0__disableLengthTimer_hook(gb); return; } // jr z
  CYC(b_+13, b_+15);
  SET_AF(POP(b_+15));
  CYC(b_+16, b_+19); SET_HL(wChannelDutyCycles);
  PUSH(b_+19, AF);
  CYC(b_+20, b_+23); A = mem_rd(gb, wSoundChannel);
  CYC(b_+23, b_+24); E = A;
  CYC(b_+24, b_+26); D = 0x00;
  CYC(b_+26, b_+27); alu_add_hl(gb, DE);
  SET_AF(POP(b_+27));
  CYC(b_+28, b_+29); mem_wr(gb, HL, A);
  CYC(b_+29, b_+31); A = 0x41;
  CYC(b_+31, b_+34); SET_HL(wChannelFrequencyModeAndLengthTimerEnabled);
  PUSH(b_+34, AF);
  CYC(b_+35, b_+38); A = mem_rd(gb, wSoundChannel);
  CYC(b_+38, b_+39); E = A;
  CYC(b_+39, b_+41); D = 0x00;
  CYC(b_+41, b_+42); alu_add_hl(gb, DE);
  SET_AF(POP(b_+42));
  CYC(b_+43, b_+44); mem_wr(gb, HL, A);
  CYC(b_+44, b_+47); TAIL(doNextChannelCommand); // jp
}

// 39:4498, @-local sub-label of channelCmdf0.
void channelCmdf0__disableLengthTimer_hook(GB *gb) {
  BASE(channelCmdf0);
  uint16_t sp0_ = gb->sp;
  SET_AF(POP(b_+47));
  CYC(b_+48, b_+50); alu_and(gb, 0xc0);
  CYC(b_+50, b_+53); SET_HL(wChannelDutyCycles);
  PUSH(b_+53, AF);
  CYC(b_+54, b_+57); A = mem_rd(gb, wSoundChannel);
  CYC(b_+57, b_+58); E = A;
  CYC(b_+58, b_+60); D = 0x00;
  CYC(b_+60, b_+61); alu_add_hl(gb, DE);
  SET_AF(POP(b_+61));
  CYC(b_+62, b_+63); mem_wr(gb, HL, A);
  CYC(b_+63, b_+65); A = 0x01;
  CYC(b_+65, b_+68); SET_HL(wChannelFrequencyModeAndLengthTimerEnabled);
  PUSH(b_+68, AF);
  CYC(b_+69, b_+72); A = mem_rd(gb, wSoundChannel);
  CYC(b_+72, b_+73); E = A;
  CYC(b_+73, b_+75); D = 0x00;
  CYC(b_+75, b_+76); alu_add_hl(gb, DE);
  SET_AF(POP(b_+76));
  CYC(b_+77, b_+78); mem_wr(gb, HL, A);
  CYC(b_+78, b_+81); TAIL(doNextChannelCommand); // jp
}

// 39:44ba, @-local sub-label of channelCmdf0.
void channelCmdf0__channel7_hook(GB *gb) {
  BASE(channelCmdf0);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+81, getNextChannelByte_hook, SYM(getNextChannelByte), b_+84);
  CYC(b_+84, b_+86); mem_wr(gb, 0xff21, A); // NR42
  CYC(b_+86, b_+88); A = 0x00;
  CYC(b_+88, b_+90); mem_wr(gb, 0xff20, A); // NR41
  CYC(b_+90, b_+92); A = 0x80;
  CYC(b_+92, b_+95); mem_wr(gb, wChannel7TriggerOnNextSound, A);
  CYC(b_+95, b_+98); TAIL(doNextChannelCommand); // jp
}

// 39:44cb, bare global. Command $d0-$df: sets volume to the lower 3 bits of the command
// value, does nothing for channel 4 (and is also useless for channel 5).
void cmdVolume_hook(GB *gb) {
  BASE(cmdVolume);
  uint16_t sp0_ = gb->sp;
  PUSH(b_+0, AF);
  CYC(b_+1, b_+4); A = mem_rd(gb, wSoundChannel);
  CYC(b_+4, b_+6); alu_cp(gb, 0x04);
  if (F & FZ) { CYCT(b_+6, b_+8); cmdVolume__next_hook(gb); return; } // jr z
  CYC(b_+6, b_+8);
  SET_AF(POP(b_+8));
  CYC(b_+9, b_+11); alu_and(gb, 0x0f);
  CYC(b_+11, b_+14); SET_HL(wChannelVolumes);
  PUSH(b_+14, AF);
  CYC(b_+15, b_+18); A = mem_rd(gb, wSoundChannel);
  CYC(b_+18, b_+19); E = A;
  CYC(b_+19, b_+21); D = 0x00;
  CYC(b_+21, b_+22); alu_add_hl(gb, DE);
  SET_AF(POP(b_+22));
  CYC(b_+23, b_+24); mem_wr(gb, HL, A);
  CYC(b_+24, b_+27); TAIL(doNextChannelCommand); // jp
}

// 39:44e6, @-local sub-label of cmdVolume.
void cmdVolume__next_hook(GB *gb) {
  BASE(cmdVolume);
  uint16_t sp0_ = gb->sp;
  SET_AF(POP(b_+27));
  CYC(b_+28, b_+31); TAIL(doNextChannelCommand); // jp
}

// 39:44ea, bare global. For square channels, sets wChannelDutyCycles to the argument value
// shifted 6 bits to the left. For wave channels, sets wChannelDutyCycles to the argument
// value and updates the waveform based on that index. Should not be used with noise
// channels or else wChannelEnvelopeStates gets messed up for channel 0 or 1.
void channelCmdf6_hook(GB *gb) {
  BASE(channelCmdf6);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wSoundChannel);
  CYC(b_+3, b_+5); alu_cp(gb, 0x04);
  if (F & FZ) { CYCT(b_+5, b_+7); channelCmdf6__wave_hook(gb); return; } // jr z
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); alu_cp(gb, 0x05);
  if (F & FZ) { CYCT(b_+9, b_+11); channelCmdf6__wave_hook(gb); return; } // jr z
  CYC(b_+9, b_+11);
  CALL_C(b_+11, getNextChannelByte_hook, SYM(getNextChannelByte), b_+14);
  CYC(b_+14, b_+16); alu_and(gb, 0x03);
  CYC(b_+16, b_+18); A = alu_swap(gb, A);
  CYC(b_+18, b_+20); A = alu_sla(gb, A);
  CYC(b_+20, b_+22); A = alu_sla(gb, A);
  CYC(b_+22, b_+25); SET_HL(wChannelDutyCycles);
  PUSH(b_+25, AF);
  CYC(b_+26, b_+29); A = mem_rd(gb, wSoundChannel);
  CYC(b_+29, b_+30); E = A;
  CYC(b_+30, b_+32); D = 0x00;
  CYC(b_+32, b_+33); alu_add_hl(gb, DE);
  SET_AF(POP(b_+33));
  CYC(b_+34, b_+35); mem_wr(gb, HL, A);
  CYC(b_+35, b_+38); TAIL(doNextChannelCommand); // jp
}

// 39:4510, @-local sub-label of channelCmdf6.
void channelCmdf6__wave_hook(GB *gb) {
  BASE(channelCmdf6);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+38, getNextChannelByte_hook, SYM(getNextChannelByte), b_+41);
  CYC(b_+41, b_+44); SET_HL(wChannelDutyCycles);
  PUSH(b_+44, AF);
  CYC(b_+45, b_+48); A = mem_rd(gb, wSoundChannel);
  CYC(b_+48, b_+49); E = A;
  CYC(b_+49, b_+51); D = 0x00;
  CYC(b_+51, b_+52); alu_add_hl(gb, DE);
  SET_AF(POP(b_+52));
  CYC(b_+53, b_+54); mem_wr(gb, HL, A);
  CYC(b_+54, b_+57); mem_wr(gb, wWaveformIndex, A);
  CALL_C(b_+57, setWaveform_hook, SYM(setWaveform), b_+60);
  CYC(b_+60, b_+63); TAIL(doNextChannelCommand); // jp
}

// 39:4529, bare global.
void standardSoundCmd_hook(GB *gb) {
  BASE(standardSoundCmd);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wSoundChannel);
  CYC(b_+3, b_+6); SET_HL(b_+10); // @table
  CALL_C(b_+6, readWordFromTable_hook, SYM(readWordFromTable), b_+9);
  CYC(b_+9, b_+10);
  {
    uint16_t target = HL;
    if (target == b_+26) { standardSoundCmd__channel0To3_hook(gb); return; }
    if (target == SYM(standardCmdChannels4To5)) { standardCmdChannels4To5_hook(gb); return; }
    if (target == SYM(standardCmdChannel6)) { standardCmdChannel6_hook(gb); return; }
    if (target == SYM(standardCmdChannel7)) { standardCmdChannel7_hook(gb); return; }
    HANDOFF(target);
  }
}

// 39:4533, @-local sub-label of standardSoundCmd. This is the jump table's own raw bytes,
// decoded exactly as real hardware would if control ever reached here as code (it never
// does in practice: standardSoundCmd only ever indexes into this table, it doesn't jump to
// its start).
void standardSoundCmd__table_hook(GB *gb) {
  BASE(standardSoundCmd);
  uint16_t sp0_ = gb->sp;
  CYC(b_+10, b_+11); B = E; // ld b,e
  CYC(b_+11, b_+12); B = L; // ld b,l
  CYC(b_+12, b_+13); B = E;
  CYC(b_+13, b_+14); B = L;
  CYC(b_+14, b_+15); B = E;
  CYC(b_+15, b_+16); B = L;
  CYC(b_+16, b_+17); B = E;
  CYC(b_+17, b_+18); B = L;
  if (F & FZ) { RET_TAKEN(b_+18); return; } // ret z
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+20); B = A;
  if (F & FZ) { RET_TAKEN(b_+20); return; } // ret z
  CYC(b_+20, b_+21);
  CYC(b_+21, b_+22); B = A;
  CALL_ASM_RST(b_+22, 0x0008, b_+23);
  CYC(b_+23, b_+24); C = B;
  CYC(b_+24, b_+25); alu_add_hl(gb, BC);
  CYC(b_+25, b_+26); C = C;
  TAIL(standardSoundCmd__channel0To3); // fallthrough
}

// 39:4543, @-local sub-label of standardSoundCmd.
void standardSoundCmd__channel0To3_hook(GB *gb) {
  BASE(standardSoundCmd);
  uint16_t sp0_ = gb->sp;
  CYC(b_+26, b_+29); SET_HL(wChannelFrequencyModeAndLengthTimerEnabled);
  CYC(b_+29, b_+32); A = mem_rd(gb, wSoundChannel);
  CYC(b_+32, b_+33); E = A;
  CYC(b_+33, b_+35); D = 0x00;
  CYC(b_+35, b_+36); alu_add_hl(gb, DE);
  CYC(b_+36, b_+37); A = mem_rd(gb, HL);
  CYC(b_+37, b_+39); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+39, b_+41); goto notArbitraryMode; } // jr z
  CYC(b_+39, b_+41);
  CALL_C(b_+41, getNextChannelByte_hook, SYM(getNextChannelByte), b_+44);
  CYC(b_+44, b_+45); L = A;
  CYC(b_+45, b_+48); A = mem_rd(gb, wSoundCmd);
  CYC(b_+48, b_+49); H = A;
  CYC(b_+49, b_+52); TAIL(standardSoundCmd__arbitraryFrequency); // jp

notArbitraryMode:
  CYC(b_+52, b_+55); A = mem_rd(gb, wSoundCmd);
  CYC(b_+55, b_+57); alu_cp(gb, 0x60);
  if (F & FZ) { CYCT(b_+57, b_+59); standardSoundCmd__cmd60_hook(gb); return; } // jr z
  CYC(b_+57, b_+59);
  CYC(b_+59, b_+61); alu_cp(gb, 0x61);
  if (F & FZ) { CYCT(b_+61, b_+63); standardSoundCmd__cmd61_hook(gb); return; } // jr z
  CYC(b_+61, b_+63);
  CYC(b_+63, b_+66); TAIL(standardSoundCmd__cmdFrequency); // jp
}

// 39:456b, @-local sub-label of standardSoundCmd.
void standardSoundCmd__cmd60_hook(GB *gb) {
  BASE(standardSoundCmd);
  uint16_t sp0_ = gb->sp;
  CYC(b_+66, b_+69); SET_HL(wChannelEnvelopes2);
  CYC(b_+69, b_+72); A = mem_rd(gb, wSoundChannel);
  CYC(b_+72, b_+73); E = A;
  CYC(b_+73, b_+75); D = 0x00;
  CYC(b_+75, b_+76); alu_add_hl(gb, DE);
  CYC(b_+76, b_+77); A = mem_rd(gb, HL);
  CYC(b_+77, b_+79); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(b_+79, b_+81); standardSoundCmd__cmd61_hook(gb); return; } // jr nz
  CYC(b_+79, b_+81);
  CYC(b_+81, b_+83); A = 0x02;
  CYC(b_+83, b_+86); SET_HL(wChannelEnvelopeStates);
  PUSH(b_+86, AF);
  CYC(b_+87, b_+90); A = mem_rd(gb, wSoundChannel);
  CYC(b_+90, b_+91); E = A;
  CYC(b_+91, b_+93); D = 0x00;
  CYC(b_+93, b_+94); alu_add_hl(gb, DE);
  SET_AF(POP(b_+94));
  CYC(b_+95, b_+96); mem_wr(gb, HL, A);
  CALL_C(b_+96, getChannelVolume_hook, SYM(getChannelVolume), b_+99);
  CYC(b_+99, b_+101); A = alu_sla(gb, A);
  CYC(b_+101, b_+103); A = alu_sla(gb, A);
  CYC(b_+103, b_+105); A = alu_sla(gb, A);
  CYC(b_+105, b_+107); A = alu_sla(gb, A);
  CYC(b_+107, b_+109); C = 0x01;
  CYC(b_+109, b_+110); alu_or(gb, C);
  CYC(b_+110, b_+113); mem_wr(gb, wSoundCmdEnvelope, A);
  CALL_C(b_+113, updateSquareChannelVolume_hook, SYM(updateSquareChannelVolume), b_+116);
  CALL_C(b_+116, updateSoundFrequencyAndPlay_hook, SYM(updateSoundFrequencyAndPlay), b_+119);
  TAIL(standardSoundCmd__cmd61); // fallthrough
}

// 39:45a0, @-local sub-label of standardSoundCmd.
void standardSoundCmd__cmd61_hook(GB *gb) {
  BASE(standardSoundCmd);
  CYC(b_+119, b_+122); TAIL(setChannelWaitCounter); // jp
}

// 39:45a3, @-local sub-label of standardSoundCmd.
void standardSoundCmd__cmdFrequency_hook(GB *gb) {
  BASE(standardSoundCmd);
  uint16_t sp0_ = gb->sp;
  CYC(b_+122, b_+125); A = mem_rd(gb, wSoundCmd);
  CYC(b_+125, b_+127); alu_sub(gb, 0x0c);
  CYC(b_+127, b_+130); SET_HL(SYM(soundFrequencyTable)); // soundFrequencyTable
  CALL_C(b_+130, readWordFromTable_hook, SYM(readWordFromTable), b_+133);
  TAIL(standardSoundCmd__arbitraryFrequency); // fallthrough
}

// 39:45ae, @-local sub-label of standardSoundCmd.
void standardSoundCmd__arbitraryFrequency_hook(GB *gb) {
  BASE(standardSoundCmd);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+133, setSoundFrequency_hook, SYM(setSoundFrequency), b_+136);
  CYC(b_+136, b_+138); A = 0x00;
  CYC(b_+138, b_+141); SET_HL(wChannelEnvelopeStates);
  PUSH(b_+141, AF);
  CYC(b_+142, b_+145); A = mem_rd(gb, wSoundChannel);
  CYC(b_+145, b_+146); E = A;
  CYC(b_+146, b_+148); D = 0x00;
  CYC(b_+148, b_+149); alu_add_hl(gb, DE);
  SET_AF(POP(b_+149));
  CYC(b_+150, b_+151); mem_wr(gb, HL, A);
  CALL_C(b_+151, handleEnvelopes_hook, SYM(handleEnvelopes), b_+154);
  CYC(b_+154, b_+156); A = 0x00;
  CYC(b_+156, b_+159); SET_HL(wChannelVibratoActive);
  PUSH(b_+159, AF);
  CYC(b_+160, b_+163); A = mem_rd(gb, wSoundChannel);
  CYC(b_+163, b_+164); E = A;
  CYC(b_+164, b_+166); D = 0x00;
  CYC(b_+166, b_+167); alu_add_hl(gb, DE);
  SET_AF(POP(b_+167));
  CYC(b_+168, b_+169); mem_wr(gb, HL, A);
  CYC(b_+169, b_+171); A = 0x00;
  CYC(b_+171, b_+174); SET_HL(wChannelVibratos);
  CYC(b_+174, b_+177); A = mem_rd(gb, wSoundChannel);
  CYC(b_+177, b_+178); E = A;
  CYC(b_+178, b_+180); D = 0x00;
  CYC(b_+180, b_+181); alu_add_hl(gb, DE);
  CYC(b_+181, b_+182); A = mem_rd(gb, HL);
  CYC(b_+182, b_+184); alu_and(gb, 0xf0);
  CYC(b_+184, b_+186); A = alu_srl(gb, A);
  CYC(b_+186, b_+188); A = alu_srl(gb, A);
  CYC(b_+188, b_+190); A = alu_srl(gb, A);
  CYC(b_+190, b_+193); SET_HL(wChannelVibratoCounters);
  PUSH(b_+193, AF);
  CYC(b_+194, b_+197); A = mem_rd(gb, wSoundChannel);
  CYC(b_+197, b_+198); E = A;
  CYC(b_+198, b_+200); D = 0x00;
  CYC(b_+200, b_+201); alu_add_hl(gb, DE);
  SET_AF(POP(b_+201));
  CYC(b_+202, b_+203); mem_wr(gb, HL, A);
  CALL_C(b_+203, updatePlayedFrequency_hook, SYM(updatePlayedFrequency), SYM(setChannelWaitCounter));
  TAIL(setChannelWaitCounter); // fallthrough
}
