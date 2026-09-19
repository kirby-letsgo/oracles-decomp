#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x39, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x39, (from), (to), true)

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
  CYC(0x4000, 0x4003); initSound_b39_hook(gb); return; // jp
}

// 39:4003, bare global.
void b39_updateSound_hook(GB *gb) {
  CYC(0x4003, 0x4006); updateSound_hook(gb); return; // jp
}

// 39:4006, bare global.
// @param a Sound to play
void b39_playSound_hook(GB *gb) {
  CYC(0x4006, 0x4009); playSound_b39_hook(gb); return; // jp
}

// 39:4009, bare global.
void b39_stopSound_hook(GB *gb) {
  CYC(0x4009, 0x400c); stopSound_hook(gb); return; // jp
}

// 39:400c, bare global. Unused; the jump target disassembles as the raw bytes of
// noiseFrequencyTable/waveformTable, decoded here exactly as the real hardware would run them.
void func_39_400c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  SET_AF(POP(0x400c));
  CYC(0x400d, 0x4010); goto garbage; // jp $4d3e

garbage:
  CYC(0x4d3e, 0x4d3f); SET_HL(HL + 1); // inc hl
  CYC(0x4d3f, 0x4d40); mem_wr(gb, BC, A); // ld (bc),a
  CYC(0x4d40, 0x4d41); B = mem_rd(gb, HL); // ld b,(hl)
  CYC(0x4d41, 0x4d43); H = 0x02;
  CYC(0x4d43, 0x4d45); H = 0x28;
  CYC(0x4d45, 0x4d46); // nop
  CYC(0x4d46, 0x4d47); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CYC(0x4d47, 0x4d48); alu_daa(gb);
  CYC(0x4d48, 0x4d49); mem_wr(gb, BC, A);
  CYC(0x4d49, 0x4d4a); D = alu_inc8(gb, D);
  CYC(0x4d4a, 0x4d4b); A = mem_rd(gb, HL); SET_HL(HL + 1); // ld a,(hl+)
  CYC(0x4d4b, 0x4d4e); SET_BC(0x2e14);
  CYC(0x4d4e, 0x4d50); B = 0x07;
  CYC(0x4d50, 0x4d51); D = D;
  CYC(0x4d51, 0x4d52); SET_BC(BC + 1);
  CYC(0x4d52, 0x4d53); alu_rla(gb);
  CYC(0x4d53, 0x4d54); mem_wr(gb, HL, A); SET_HL(HL - 1); // ld (hl-),a
  CYC(0x4d54, 0x4d55); mem_wr(gb, BC, A);
  CYC(0x4d55, 0x4d56); alu_scf(gb);
  CYC(0x4d56, 0x4d57); alu_cpl(gb);
  CYC(0x4d57, 0x4d58); mem_wr(gb, BC, A);
  CYC(0x4d58, 0x4d59); B = L;
  CYC(0x4d59, 0x4d5a); alu_add_hl(gb, HL);
  CYC(0x4d5a, 0x4d5b); mem_wr(gb, BC, A);
  CYC(0x4d5b, 0x4d5c); B = A;
  if (!(F & FC)) { CYCT(0x4d5c, 0x4d5e); } else { CYC(0x4d5c, 0x4d5e); } // jr nc,$4d5e (both sides land here)
  CYC(0x4d5e, 0x4d5f); alu_rlca(gb);
  CALL_ASM_RST(0x4d5f, 0x0038, 0x4d60);
  HANDOFF(0x4d60);
}

// 39:4010, bare global.
// @param a Volume (0-3)
void b39_updateMusicVolume_hook(GB *gb) {
  CYC(0x4010, 0x4013); updateMusicVolume_hook(gb); return; // jp
}

// 39:4015, bare global (aliased here as initSound_b39 since bank0's initSound already exists).
void initSound_b39_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4015, 0x4017); mem_wr(gb, hSoundDataBaseBank, A);
  CALL_C(0x4017, stopSound_hook, 0x40a8, 0x401a);
  CYC(0x401a, 0x401c); A = 0x03;
  CYC(0x401c, 0x401f); mem_wr(gb, wMusicVolume, A);
  CYC(0x401f, 0x4021); A = 0x00;
  CYC(0x4021, 0x4024); mem_wr(gb, wSoundFadeDirection, A);
  CYC(0x4024, 0x4027); mem_wr(gb, wSoundFadeCounter, A);
  CYC(0x4027, 0x402a); mem_wr(gb, wSoundDisabled, A);
  CYC(0x402a, 0x402d); mem_wr(gb, wMusicMuted, A);
  CYC(0x402d, 0x402f); A = 0x8f;
  CYC(0x402f, 0x4031); mem_wr(gb, 0xff26, A); // NR52
  CYC(0x4031, 0x4033); A = 0x77;
  CYC(0x4033, 0x4036); mem_wr(gb, wSoundVolume, A);
  CYC(0x4036, 0x4038); mem_wr(gb, 0xff24, A); // NR50
  CYC(0x4038, 0x403a); A = 0xff;
  CYC(0x403a, 0x403c); mem_wr(gb, 0xff25, A); // NR51
  CYC(0x403c, 0x403e); C = 0x14;
  CYC(0x403e, 0x4041); SET_HL(0x404b); // @readFunction
  CYC(0x4041, 0x4044); SET_DE(wMusicReadFunction);

copyLoop:
  CYC(0x4044, 0x4045); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x4045, 0x4046); mem_wr(gb, DE, A);
  CYC(0x4046, 0x4047); SET_DE(DE + 1);
  CYC(0x4047, 0x4048); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(0x4048, 0x404a); goto copyLoop; } // jr nz
  CYC(0x4048, 0x404a);
  RET(0x404a); return; // ret
}

// 39:404b, @-local sub-label of initSound_b39. This block's bytes are copied to
// wMusicReadFunction and later executed there by wMusicReadFunction_hook.
void initSound__readFunction_b39_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x404b, 0x404d); mem_wr(gb, hSoundDataBaseBank2, A);
  CYC(0x404d, 0x4050); mem_wr(gb, 0x2000, A);
  CYC(0x4050, 0x4051); A = mem_rd(gb, HL); SET_HL(HL + 1); // ld a,(hl+)
  CYC(0x4051, 0x4052); C = A;
  CYC(0x4052, 0x4054); A = mem_rd(gb, hSoundDataBaseBank);
  CYC(0x4054, 0x4056); mem_wr(gb, hSoundDataBaseBank2, A);
  CYC(0x4056, 0x4059); mem_wr(gb, 0x2000, A);
  CYC(0x4059, 0x405a); A = C;
  RET(0x405a); return; // ret
}

// 39:405d, bare global.
// @param a Volume (0-3)
void updateMusicVolume_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  PUSH(0x405d, BC);
  PUSH(0x405e, DE);
  PUSH(0x405f, HL);
  PUSH(0x4060, AF);
  CALL_C(0x4061, silenceSquareMusicChannels_hook, 0x4079, 0x4064);
  SET_AF(POP(0x4064));
  CYC(0x4065, 0x4068); mem_wr(gb, wMusicVolume, A);
  CYC(0x4068, 0x406a); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(0x406a, 0x406c); goto notMuted; } // jr nz
  CYC(0x406a, 0x406c);
  CYC(0x406c, 0x406e); A = 0x01;
  CYC(0x406e, 0x4070); goto storeMuted; // jr

notMuted:
  CYC(0x4070, 0x4072); A = 0x00;

storeMuted:
  CYC(0x4072, 0x4075); mem_wr(gb, wMusicMuted, A);
  SET_HL(POP(0x4075));
  SET_DE(POP(0x4076));
  SET_BC(POP(0x4077));
  RET(0x4078); return; // ret
}

// 39:4079, bare global. Silences channels 0 and 1 if enabled.
void silenceSquareMusicChannels_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4079, 0x407b); A = 0x00;
  CYC(0x407b, 0x407e); mem_wr(gb, wSoundChannel, A);
  CYC(0x407e, 0x4081); SET_HL(wChannelsEnabled);
  CYC(0x4081, 0x4084); A = mem_rd(gb, wSoundChannel);
  CYC(0x4084, 0x4085); E = A;
  CYC(0x4085, 0x4087); D = 0x00;
  CYC(0x4087, 0x4088); alu_add_hl(gb, DE);
  CYC(0x4088, 0x4089); A = mem_rd(gb, HL);
  CYC(0x4089, 0x408b); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(0x408b, 0x408d); goto square2; } // jr z
  CYC(0x408b, 0x408d);
  CALL_C(0x408d, silencePlayedSound_hook, 0x4932, 0x4090);

square2:
  CYC(0x4090, 0x4092); A = 0x01;
  CYC(0x4092, 0x4095); mem_wr(gb, wSoundChannel, A);
  CYC(0x4095, 0x4098); SET_HL(wChannelsEnabled);
  CYC(0x4098, 0x409b); A = mem_rd(gb, wSoundChannel);
  CYC(0x409b, 0x409c); E = A;
  CYC(0x409c, 0x409e); D = 0x00;
  CYC(0x409e, 0x409f); alu_add_hl(gb, DE);
  CYC(0x409f, 0x40a0); A = mem_rd(gb, HL);
  CYC(0x40a0, 0x40a2); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(0x40a2, 0x40a4); goto ret; } // jr z
  CYC(0x40a2, 0x40a4);
  CALL_C(0x40a4, silencePlayedSound_hook, 0x4932, 0x40a7);

ret:
  RET(0x40a7); return; // ret
}

// 39:40a8, bare global.
void stopSound_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x40a8, 0x40aa); A = 0x00;

loop:
  CYC(0x40aa, 0x40ad); mem_wr(gb, wSoundChannel, A);
  CALL_C(0x40ad, channelCmdff_hook, 0x4923, 0x40b0);
  CYC(0x40b0, 0x40b3); A = mem_rd(gb, wSoundChannel);
  CYC(0x40b3, 0x40b4); A = alu_inc8(gb, A);
  CYC(0x40b4, 0x40b6); alu_cp(gb, 0x08);
  if (!(F & FZ)) { CYCT(0x40b6, 0x40b8); goto loop; } // jr nz
  CYC(0x40b6, 0x40b8);
  RET(0x40b8); return; // ret
}

// 39:40b9, bare global.
void silenceAllChannels_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x40b9, 0x40bb); A = 0x00;

loop:
  CYC(0x40bb, 0x40be); mem_wr(gb, wSoundChannel, A);
  CALL_C(0x40be, silencePlayedSound_hook, 0x4932, 0x40c1);
  CYC(0x40c1, 0x40c4); A = mem_rd(gb, wSoundChannel);
  CYC(0x40c4, 0x40c5); A = alu_inc8(gb, A);
  CYC(0x40c5, 0x40c7); alu_cp(gb, 0x08);
  if (!(F & FZ)) { CYCT(0x40c7, 0x40c9); goto loop; } // jr nz
  CYC(0x40c7, 0x40c9);
  RET(0x40c9); return; // ret
}

// 39:40ca, bare global. Disable all sound effect channels.
void stopSfx_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x40ca, 0x40cc); A = 0x02;
  CYC(0x40cc, 0x40cf); mem_wr(gb, wSoundChannel, A);
  CYC(0x40cf, 0x40d2); SET_HL(wChannelsEnabled);
  CYC(0x40d2, 0x40d5); A = mem_rd(gb, wSoundChannel);
  CYC(0x40d5, 0x40d6); E = A;
  CYC(0x40d6, 0x40d8); D = 0x00;
  CYC(0x40d8, 0x40d9); alu_add_hl(gb, DE);
  CYC(0x40d9, 0x40da); A = mem_rd(gb, HL);
  CYC(0x40da, 0x40dc); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(0x40dc, 0x40de); goto square2; } // jr z
  CYC(0x40dc, 0x40de);
  CALL_C(0x40de, channelCmdff_hook, 0x4923, 0x40e1);

square2:
  CYC(0x40e1, 0x40e3); A = 0x03;
  CYC(0x40e3, 0x40e6); mem_wr(gb, wSoundChannel, A);
  CYC(0x40e6, 0x40e9); SET_HL(wChannelsEnabled);
  CYC(0x40e9, 0x40ec); A = mem_rd(gb, wSoundChannel);
  CYC(0x40ec, 0x40ed); E = A;
  CYC(0x40ed, 0x40ef); D = 0x00;
  CYC(0x40ef, 0x40f0); alu_add_hl(gb, DE);
  CYC(0x40f0, 0x40f1); A = mem_rd(gb, HL);
  CYC(0x40f1, 0x40f3); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(0x40f3, 0x40f5); goto wave; } // jr z
  CYC(0x40f3, 0x40f5);
  CALL_C(0x40f5, channelCmdff_hook, 0x4923, 0x40f8);

wave:
  CYC(0x40f8, 0x40fa); A = 0x05;
  CYC(0x40fa, 0x40fd); mem_wr(gb, wSoundChannel, A);
  CYC(0x40fd, 0x4100); SET_HL(wChannelsEnabled);
  CYC(0x4100, 0x4103); A = mem_rd(gb, wSoundChannel);
  CYC(0x4103, 0x4104); E = A;
  CYC(0x4104, 0x4106); D = 0x00;
  CYC(0x4106, 0x4107); alu_add_hl(gb, DE);
  CYC(0x4107, 0x4108); A = mem_rd(gb, HL);
  CYC(0x4108, 0x410a); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(0x410a, 0x410c); goto noise; } // jr z
  CYC(0x410a, 0x410c);
  CALL_C(0x410c, channelCmdff_hook, 0x4923, 0x410f);

noise:
  CYC(0x410f, 0x4111); A = 0x07;
  CYC(0x4111, 0x4114); mem_wr(gb, wSoundChannel, A);
  CYC(0x4114, 0x4117); SET_HL(wChannelsEnabled);
  CYC(0x4117, 0x411a); A = mem_rd(gb, wSoundChannel);
  CYC(0x411a, 0x411b); E = A;
  CYC(0x411b, 0x411d); D = 0x00;
  CYC(0x411d, 0x411e); alu_add_hl(gb, DE);
  CYC(0x411e, 0x411f); A = mem_rd(gb, HL);
  CYC(0x411f, 0x4121); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(0x4121, 0x4123); goto ret; } // jr z
  CYC(0x4121, 0x4123);
  CALL_C(0x4123, channelCmdff_hook, 0x4923, 0x4126);

ret:
  RET(0x4126); return; // ret
}

// 39:4127, bare global.
void updateSound_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  PUSH(0x4127, BC);
  PUSH(0x4128, DE);
  PUSH(0x4129, HL);
  CYC(0x412a, 0x412d); A = mem_rd(gb, wSoundDisabled);
  CYC(0x412d, 0x412f); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(0x412f, 0x4131); goto notDisabled; } // jr z
  CYC(0x412f, 0x4131);
  CYC(0x4131, 0x4134); updateSound__ret_hook(gb); return; // jp

notDisabled:
  CYC(0x4134, 0x4137); A = mem_rd(gb, wSoundVolume);
  CYC(0x4137, 0x4139); mem_wr(gb, 0xff24, A); // NR50
  CYC(0x4139, 0x413c); A = mem_rd(gb, wSoundFadeDirection);
  CYC(0x413c, 0x413e); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(0x413e, 0x4140); updateSound__updateChannels_hook(gb); return; } // jr z
  CYC(0x413e, 0x4140);
  CYC(0x4140, 0x4143); A = mem_rd(gb, wSoundFadeSpeed);
  CYC(0x4143, 0x4144); B = A;
  CYC(0x4144, 0x4147); A = mem_rd(gb, wSoundFadeCounter);
  CYC(0x4147, 0x4148); A = alu_inc8(gb, A);
  CYC(0x4148, 0x414b); mem_wr(gb, wSoundFadeCounter, A);
  CYC(0x414b, 0x414c); alu_and(gb, B);
  CYC(0x414c, 0x414d); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(0x414d, 0x414f); updateSound__updateChannels_hook(gb); return; } // jr nz
  CYC(0x414d, 0x414f);
  CYC(0x414f, 0x4152); A = mem_rd(gb, wSoundFadeDirection);
  CYC(0x4152, 0x4154); alu_cp(gb, 0x0a);
  if (F & FZ) { CYCT(0x4154, 0x4156); updateSound__incVolume_hook(gb); return; } // jr z
  CYC(0x4154, 0x4156);
  updateSound__decVolume_hook(gb); return; // fallthrough
}

// 39:4156, @-local sub-label of updateSound.
void updateSound__decVolume_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4156, 0x4159); A = mem_rd(gb, wSoundVolume);
  CYC(0x4159, 0x415b); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(0x415b, 0x415d); updateSound__stopSound_hook(gb); return; } // jr z
  CYC(0x415b, 0x415d);
  CYC(0x415d, 0x415f); alu_sub(gb, 0x11);
  CYC(0x415f, 0x4162); mem_wr(gb, wSoundVolume, A);
  CYC(0x4162, 0x4165); updateSound__updateChannels_hook(gb); return; // jp
}

// 39:4165, @-local sub-label of updateSound.
void updateSound__incVolume_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4165, 0x4168); A = mem_rd(gb, wSoundVolume);
  CYC(0x4168, 0x416a); alu_cp(gb, 0x77);
  if (F & FZ) { CYCT(0x416a, 0x416c); updateSound__clearFadeVariables_hook(gb); return; } // jr z
  CYC(0x416a, 0x416c);
  CYC(0x416c, 0x416e); alu_add(gb, 0x11);
  CYC(0x416e, 0x4171); mem_wr(gb, wSoundVolume, A);
  CYC(0x4171, 0x4174); updateSound__updateChannels_hook(gb); return; // jp
}

// 39:4174, @-local sub-label of updateSound.
void updateSound__stopSound_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4174, stopSound_hook, 0x40a8, 0x4177);
  updateSound__clearFadeVariables_hook(gb); return; // fallthrough
}

// 39:4177, @-local sub-label of updateSound.
void updateSound__clearFadeVariables_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4177, 0x4179); A = 0x00;
  CYC(0x4179, 0x417c); mem_wr(gb, wSoundFadeCounter, A);
  CYC(0x417c, 0x417f); mem_wr(gb, wSoundFadeDirection, A);
  updateSound__updateChannels_hook(gb); return; // fallthrough
}

// 39:417f, @-local sub-label of updateSound.
void updateSound__updateChannels_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x417f, 0x4181); A = 0x00;
  updateSound__channelLoop_hook(gb); return; // fallthrough
}

// 39:4181, @-local sub-label of updateSound.
void updateSound__channelLoop_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4181, 0x4184); mem_wr(gb, wSoundChannel, A);
  CYC(0x4184, 0x4187); SET_HL(wChannelsEnabled);
  CYC(0x4187, 0x418a); A = mem_rd(gb, wSoundChannel);
  CYC(0x418a, 0x418b); E = A;
  CYC(0x418b, 0x418d); D = 0x00;
  CYC(0x418d, 0x418e); alu_add_hl(gb, DE);
  CYC(0x418e, 0x418f); A = mem_rd(gb, HL);
  CYC(0x418f, 0x4191); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(0x4191, 0x4193); updateSound__nextChannel_hook(gb); return; } // jr z
  CYC(0x4191, 0x4193);
  CYC(0x4193, 0x4196); SET_HL(wChannelWaitCounters);
  CYC(0x4196, 0x4199); A = mem_rd(gb, wSoundChannel);
  CYC(0x4199, 0x419a); E = A;
  CYC(0x419a, 0x419c); D = 0x00;
  CYC(0x419c, 0x419d); alu_add_hl(gb, DE);
  CYC(0x419d, 0x419e); A = mem_rd(gb, HL);
  CYC(0x419e, 0x41a0); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(0x41a0, 0x41a2); updateSound__continueSound_hook(gb); return; } // jr nz
  CYC(0x41a0, 0x41a2);
  CALL_C(0x41a2, doNextChannelCommand_hook, 0x4396, 0x41a5);
  CYC(0x41a5, 0x41a7); updateSound__nextChannel_hook(gb); return; // jr
}

// 39:41a7, @-local sub-label of updateSound.
void updateSound__continueSound_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x41a7, continuePlayingSound_hook, 0x41c2, 0x41aa);
  updateSound__nextChannel_hook(gb); return; // fallthrough
}

// 39:41aa, @-local sub-label of updateSound.
void updateSound__nextChannel_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x41aa, 0x41ad); A = mem_rd(gb, wSoundChannel);
  CYC(0x41ad, 0x41ae); A = alu_inc8(gb, A);
  CYC(0x41ae, 0x41b0); alu_cp(gb, 0x08);
  if (!(F & FZ)) { CYCT(0x41b0, 0x41b2); updateSound__channelLoop_hook(gb); return; } // jr nz
  CYC(0x41b0, 0x41b2);
  CYC(0x41b2, 0x41b5); A = mem_rd(gb, wMusicMuted);
  CYC(0x41b5, 0x41b7); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(0x41b7, 0x41b9); updateSound__ret_hook(gb); return; } // jr nz
  CYC(0x41b7, 0x41b9);
  CYC(0x41b9, 0x41bb); A = 0x02;
  CYC(0x41bb, 0x41be); mem_wr(gb, wMusicMuted, A);
  updateSound__ret_hook(gb); return; // fallthrough
}

// 39:41be, @-local sub-label of updateSound.
void updateSound__ret_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  SET_HL(POP(0x41be));
  SET_DE(POP(0x41bf));
  SET_BC(POP(0x41c0));
  RET(0x41c1); return; // ret
}

// 39:41c2, bare global. Keep playing the current sound.
void continuePlayingSound_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x41c2, 0x41c5); SET_HL(wChannelWaitCounters);
  CYC(0x41c5, 0x41c8); A = mem_rd(gb, wSoundChannel);
  CYC(0x41c8, 0x41c9); E = A;
  CYC(0x41c9, 0x41cb); D = 0x00;
  CYC(0x41cb, 0x41cc); alu_add_hl(gb, DE);
  CYC(0x41cc, 0x41cd); A = mem_rd(gb, HL);
  CYC(0x41cd, 0x41ce); A = alu_dec8(gb, A);
  CYC(0x41ce, 0x41cf); mem_wr(gb, HL, A);
  CYC(0x41cf, 0x41d2); A = mem_rd(gb, wSoundChannel);
  CYC(0x41d2, 0x41d4); alu_cp(gb, 0x06);
  if (!(F & FC)) { CYCT(0x41d4, 0x41d6); continuePlayingSound__ret_hook(gb); return; } // jr nc
  CYC(0x41d4, 0x41d6);
  CYC(0x41d6, 0x41d9); SET_HL(wChannelFrequencyModeAndLengthTimerEnabled);
  CYC(0x41d9, 0x41dc); A = mem_rd(gb, wSoundChannel);
  CYC(0x41dc, 0x41dd); E = A;
  CYC(0x41dd, 0x41df); D = 0x00;
  CYC(0x41df, 0x41e0); alu_add_hl(gb, DE);
  CYC(0x41e0, 0x41e1); A = mem_rd(gb, HL);
  CYC(0x41e1, 0x41e3); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(0x41e3, 0x41e5); continuePlayingSound__ret_hook(gb); return; } // jr nz
  CYC(0x41e3, 0x41e5);
  CYC(0x41e5, 0x41e8); A = mem_rd(gb, wSoundChannel);
  CYC(0x41e8, 0x41ea); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(0x41ea, 0x41ec); goto skipEnvelopes; } // jr nc
  CYC(0x41ea, 0x41ec);
  CALL_C(0x41ec, handleEnvelopes_hook, 0x464c, 0x41ef);

skipEnvelopes:
  CALL_C(0x41ef, updateSoundFrequencyAndPlay_hook, 0x41f3, 0x41f2);
  continuePlayingSound__ret_hook(gb); return; // fallthrough
}

// 39:41f2, @-local sub-label of continuePlayingSound.
void continuePlayingSound__ret_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  RET(0x41f2); return; // ret
}

// 39:41f3, bare global. Copies the channel's frequency value from hSoundData3 to
// wSoundFrequencyL,H after applying sweep and vibrato.
void updateSoundFrequencyAndPlay_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x41f3, 0x41f6); SET_HL(wChannelSweep);
  CYC(0x41f6, 0x41f9); A = mem_rd(gb, wSoundChannel);
  CYC(0x41f9, 0x41fa); E = A;
  CYC(0x41fa, 0x41fc); D = 0x00;
  CYC(0x41fc, 0x41fd); alu_add_hl(gb, DE);
  CYC(0x41fd, 0x41fe); A = mem_rd(gb, HL);
  CYC(0x41fe, 0x41ff); C = A;
  CYC(0x41ff, 0x4201); alu_and(gb, 0x7f);
  if (F & FZ) { CYCT(0x4201, 0x4203); updateSoundFrequencyAndPlay__handleVibrato_hook(gb); return; } // jr z
  CYC(0x4201, 0x4203);
  CYC(0x4203, 0x4204); A = C;
  CYC(0x4204, 0x4206); alu_and(gb, 0x80);
  if (!(F & FZ)) { CYCT(0x4206, 0x4208); goto negativeSweep; } // jr nz
  CYC(0x4206, 0x4208);
  CYC(0x4208, 0x420a); D = 0x00;
  CYC(0x420a, 0x420c); goto sweepDeReady; // jr

negativeSweep:
  CYC(0x420c, 0x420e); D = 0xff;

sweepDeReady:
  PUSH(0x420e, DE);
  CYC(0x420f, 0x4212); SET_HL(wChannelSweep);
  CYC(0x4212, 0x4215); A = mem_rd(gb, wSoundChannel);
  CYC(0x4215, 0x4216); E = A;
  CYC(0x4216, 0x4218); D = 0x00;
  CYC(0x4218, 0x4219); alu_add_hl(gb, DE);
  CYC(0x4219, 0x421a); A = mem_rd(gb, HL);
  SET_DE(POP(0x421a));
  CYC(0x421b, 0x421c); E = A;
  CYC(0x421c, 0x421f); A = mem_rd(gb, wSoundChannel);
  CYC(0x421f, 0x4221); A = alu_sla(gb, A);
  CYC(0x4221, 0x4222); B = A;
  CYC(0x4222, 0x4223); A = B;
  CYC(0x4223, 0x4225); alu_add(gb, hSoundData3 & 0xff);
  CYC(0x4225, 0x4226); C = A;
  CYC(0x4226, 0x4227); A = mem_rd(gb, 0xff00 | C);
  CYC(0x4227, 0x4228); C = alu_inc8(gb, C);
  CYC(0x4228, 0x4229); L = A;
  CYC(0x4229, 0x422a); A = mem_rd(gb, 0xff00 | C);
  CYC(0x422a, 0x422b); C = alu_inc8(gb, C);
  CYC(0x422b, 0x422c); H = A;
  CYC(0x422c, 0x422d); alu_add_hl(gb, DE);
  CYC(0x422d, 0x4230); A = mem_rd(gb, wSoundChannel);
  CYC(0x4230, 0x4232); A = alu_sla(gb, A);
  CYC(0x4232, 0x4233); B = A;
  CYC(0x4233, 0x4234); A = L;
  CYC(0x4234, 0x4236); C = hSoundData3 & 0xff;
  CALL_C(0x4236, writeIndexedHighRamAndIncrement_hook, 0x4d25, 0x4239);
  CYC(0x4239, 0x423a); A = H;
  CYC(0x423a, 0x423b); mem_wr(gb, 0xff00 | C, A);
  CYC(0x423b, 0x423c); C = alu_inc8(gb, C);
  updateSoundFrequencyAndPlay__handleVibrato_hook(gb); return; // fallthrough
}

// 39:423c, @-local sub-label of updateSoundFrequencyAndPlay.
void updateSoundFrequencyAndPlay__handleVibrato_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x423c, 0x423f); SET_HL(wChannelVibratoActive);
  CYC(0x423f, 0x4242); A = mem_rd(gb, wSoundChannel);
  CYC(0x4242, 0x4243); E = A;
  CYC(0x4243, 0x4245); D = 0x00;
  CYC(0x4245, 0x4246); alu_add_hl(gb, DE);
  CYC(0x4246, 0x4247); A = mem_rd(gb, HL);
  CYC(0x4247, 0x4249); alu_and(gb, 0x10);
  if (!(F & FZ)) { CYCT(0x4249, 0x424b); updateSoundFrequencyAndPlay__useVibrato_hook(gb); return; } // jr nz
  CYC(0x4249, 0x424b);
  CYC(0x424b, 0x424e); SET_HL(wChannelVibratoCounters);
  CYC(0x424e, 0x4251); A = mem_rd(gb, wSoundChannel);
  CYC(0x4251, 0x4252); E = A;
  CYC(0x4252, 0x4254); D = 0x00;
  CYC(0x4254, 0x4255); alu_add_hl(gb, DE);
  CYC(0x4255, 0x4256); A = mem_rd(gb, HL);
  CYC(0x4256, 0x4258); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(0x4258, 0x425a); updateSoundFrequencyAndPlay__endVibratoWait_hook(gb); return; } // jr z
  CYC(0x4258, 0x425a);
  CYC(0x425a, 0x425b); A = alu_dec8(gb, A);
  CYC(0x425b, 0x425e); SET_HL(wChannelVibratoCounters);
  PUSH(0x425e, AF);
  CYC(0x425f, 0x4262); A = mem_rd(gb, wSoundChannel);
  CYC(0x4262, 0x4263); E = A;
  CYC(0x4263, 0x4265); D = 0x00;
  CYC(0x4265, 0x4266); alu_add_hl(gb, DE);
  SET_AF(POP(0x4266));
  CYC(0x4267, 0x4268); mem_wr(gb, HL, A);
  CYC(0x4268, 0x426b); SET_HL(0x0000);
  CYC(0x426b, 0x426e); updateSoundFrequencyAndPlay__updateSoundFrequencyWithOffset_hook(gb); return; // jp
}

// 39:426e, @-local sub-label of updateSoundFrequencyAndPlay.
void updateSoundFrequencyAndPlay__endVibratoWait_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x426e, 0x4270); A = 0x10;
  CYC(0x4270, 0x4273); SET_HL(wChannelVibratoActive);
  PUSH(0x4273, AF);
  CYC(0x4274, 0x4277); A = mem_rd(gb, wSoundChannel);
  CYC(0x4277, 0x4278); E = A;
  CYC(0x4278, 0x427a); D = 0x00;
  CYC(0x427a, 0x427b); alu_add_hl(gb, DE);
  SET_AF(POP(0x427b));
  CYC(0x427c, 0x427d); mem_wr(gb, HL, A);
  CYC(0x427d, 0x427f); A = 0x00;
  CYC(0x427f, 0x4282); SET_HL(wChannelVibratoCounters);
  PUSH(0x4282, AF);
  CYC(0x4283, 0x4286); A = mem_rd(gb, wSoundChannel);
  CYC(0x4286, 0x4287); E = A;
  CYC(0x4287, 0x4289); D = 0x00;
  CYC(0x4289, 0x428a); alu_add_hl(gb, DE);
  SET_AF(POP(0x428a));
  CYC(0x428b, 0x428c); mem_wr(gb, HL, A);
  updateSoundFrequencyAndPlay__useVibrato_hook(gb); return; // fallthrough
}

// 39:428c, @-local sub-label of updateSoundFrequencyAndPlay.
void updateSoundFrequencyAndPlay__useVibrato_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x428c, 0x428f); SET_HL(wChannelVibratoCounters);
  CYC(0x428f, 0x4292); A = mem_rd(gb, wSoundChannel);
  CYC(0x4292, 0x4293); E = A;
  CYC(0x4293, 0x4295); D = 0x00;
  CYC(0x4295, 0x4296); alu_add_hl(gb, DE);
  CYC(0x4296, 0x4297); A = mem_rd(gb, HL);
  CYC(0x4297, 0x4299); alu_cp(gb, 0x08);
  if (!(F & FZ)) { CYCT(0x4299, 0x429b); updateSoundFrequencyAndPlay__determineFrequencyOffset_hook(gb); return; } // jr nz
  CYC(0x4299, 0x429b);
  CYC(0x429b, 0x429d); A = 0x00;
  CYC(0x429d, 0x42a0); SET_HL(wChannelVibratoCounters);
  PUSH(0x42a0, AF);
  CYC(0x42a1, 0x42a4); A = mem_rd(gb, wSoundChannel);
  CYC(0x42a4, 0x42a5); E = A;
  CYC(0x42a5, 0x42a7); D = 0x00;
  CYC(0x42a7, 0x42a8); alu_add_hl(gb, DE);
  SET_AF(POP(0x42a8));
  CYC(0x42a9, 0x42aa); mem_wr(gb, HL, A);
  CYC(0x42aa, 0x42ac); A = 0x00;
  updateSoundFrequencyAndPlay__determineFrequencyOffset_hook(gb); return; // fallthrough
}

// 39:42ac, @-local sub-label of updateSoundFrequencyAndPlay.
void updateSoundFrequencyAndPlay__determineFrequencyOffset_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x42ac, 0x42af); SET_HL(0x4b40); // vibratoOffsetTable
  CALL_C(0x42af, readWordFromTable_hook, 0x4d19, 0x42b2);
  PUSH(0x42b2, HL);
  CYC(0x42b3, 0x42b6); SET_HL(wChannelVibratoCounters);
  CYC(0x42b6, 0x42b9); A = mem_rd(gb, wSoundChannel);
  CYC(0x42b9, 0x42ba); E = A;
  CYC(0x42ba, 0x42bc); D = 0x00;
  CYC(0x42bc, 0x42bd); alu_add_hl(gb, DE);
  CYC(0x42bd, 0x42be); A = mem_rd(gb, HL);
  CYC(0x42be, 0x42bf); A = alu_inc8(gb, A);
  CYC(0x42bf, 0x42c0); mem_wr(gb, HL, A);
  CYC(0x42c0, 0x42c3); SET_HL(wChannelVibratos);
  CYC(0x42c3, 0x42c6); A = mem_rd(gb, wSoundChannel);
  CYC(0x42c6, 0x42c7); E = A;
  CYC(0x42c7, 0x42c9); D = 0x00;
  CYC(0x42c9, 0x42ca); alu_add_hl(gb, DE);
  CYC(0x42ca, 0x42cb); A = mem_rd(gb, HL);
  CYC(0x42cb, 0x42cd); alu_and(gb, 0x0f);
  SET_HL(POP(0x42cd));
  CALL_C(0x42ce, multiplyHlByA_hook, 0x4a10, 0x42d1);
  updateSoundFrequencyAndPlay__updateSoundFrequencyWithOffset_hook(gb); return; // fallthrough
}

// 39:42d1, @-local sub-label of updateSoundFrequencyAndPlay.
void updateSoundFrequencyAndPlay__updateSoundFrequencyWithOffset_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x42d1, 0x42d4); A = mem_rd(gb, wSoundChannel);
  CYC(0x42d4, 0x42d6); A = alu_sla(gb, A);
  CYC(0x42d6, 0x42d7); B = A;
  CYC(0x42d7, 0x42d8); A = B;
  CYC(0x42d8, 0x42da); alu_add(gb, hSoundData3 & 0xff);
  CYC(0x42da, 0x42db); C = A;
  CYC(0x42db, 0x42dc); A = mem_rd(gb, 0xff00 | C);
  CYC(0x42dc, 0x42dd); C = alu_inc8(gb, C);
  CYC(0x42dd, 0x42de); E = A;
  CYC(0x42de, 0x42df); A = mem_rd(gb, 0xff00 | C);
  CYC(0x42df, 0x42e0); C = alu_inc8(gb, C);
  CYC(0x42e0, 0x42e1); D = A;
  CYC(0x42e1, 0x42e2); alu_add_hl(gb, DE);
  CYC(0x42e2, 0x42e3); A = L;
  CYC(0x42e3, 0x42e6); mem_wr(gb, wSoundFrequencyL, A);
  CYC(0x42e6, 0x42e7); A = H;
  CYC(0x42e7, 0x42ea); mem_wr(gb, wSoundFrequencyH, A);
  updatePlayedFrequency_hook(gb); return; // fallthrough
}

// 39:42ea, bare global. When used for the wave channel, hl is expected to contain
// wSoundFrequencyL,H and is written to NR33 and NR34.
void updatePlayedFrequency_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x42ea, 0x42ed); A = mem_rd(gb, wSoundChannel);
  CYC(0x42ed, 0x42ee); alu_scf(gb);
  CYC(0x42ee, 0x42ef); alu_ccf(gb);
  CYC(0x42ef, 0x42f1); alu_cp(gb, 0x04);
  if (!(F & FC)) { CYCT(0x42f1, 0x42f3); updatePlayedFrequency__wave_hook(gb); return; } // jr nc
  CYC(0x42f1, 0x42f3);
  CYC(0x42f3, 0x42f5); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(0x42f5, 0x42f7); updatePlayedFrequency__square_hook(gb); return; } // jr nc
  CYC(0x42f5, 0x42f7);
  CYC(0x42f7, 0x42f8); A = alu_inc8(gb, A);
  CYC(0x42f8, 0x42f9); A = alu_inc8(gb, A);
  CYC(0x42f9, 0x42fa); E = A;
  CYC(0x42fa, 0x42fd); SET_HL(wChannelsEnabled);
  CYC(0x42fd, 0x42ff); D = 0x00;
  CYC(0x42ff, 0x4300); alu_add_hl(gb, DE);
  CYC(0x4300, 0x4301); A = mem_rd(gb, HL);
  CYC(0x4301, 0x4303); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(0x4303, 0x4305); updatePlayedFrequency__square_hook(gb); return; } // jr z
  CYC(0x4303, 0x4305);
  RET(0x4305); return; // ret
}

// 39:4306, @-local sub-label of updatePlayedFrequency.
void updatePlayedFrequency__square_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4306, 0x4309); A = mem_rd(gb, wSoundChannel);
  CYC(0x4309, 0x430b); alu_and(gb, 0x01);
  CYC(0x430b, 0x430c); B = A;
  CYC(0x430c, 0x430e); A = alu_sla(gb, A);
  CYC(0x430e, 0x4310); A = alu_sla(gb, A);
  CYC(0x4310, 0x4311); alu_add(gb, B);
  CYC(0x4311, 0x4312); B = A;
  PUSH(0x4312, BC);
  CYC(0x4313, 0x4316); A = mem_rd(gb, wSoundFrequencyL);
  CYC(0x4316, 0x4318); C = 0x13; // NR13
  CALL_C(0x4318, writeIndexedHighRamAndIncrement_hook, 0x4d25, 0x431b);
  CYC(0x431b, 0x431e); A = mem_rd(gb, wSoundCmdEnvelope);
  CYC(0x431e, 0x431f); E = A;
  CYC(0x431f, 0x4322); A = mem_rd(gb, wSoundFrequencyH);
  CYC(0x4322, 0x4323); alu_or(gb, E);
  CYC(0x4323, 0x4324); mem_wr(gb, 0xff00 | C, A); // NR14
  CYC(0x4324, 0x4325); C = alu_inc8(gb, C);
  SET_BC(POP(0x4325));
  PUSH(0x4326, BC);
  CYC(0x4327, 0x432a); SET_HL(wChannelDutyCycles);
  CYC(0x432a, 0x432d); A = mem_rd(gb, wSoundChannel);
  CYC(0x432d, 0x432e); E = A;
  CYC(0x432e, 0x4330); D = 0x00;
  CYC(0x4330, 0x4331); alu_add_hl(gb, DE);
  CYC(0x4331, 0x4332); A = mem_rd(gb, HL);
  SET_BC(POP(0x4332));
  CYC(0x4333, 0x4335); C = 0x11; // NR11
  CALL_C(0x4335, writeIndexedHighRamAndIncrement_hook, 0x4d25, 0x4338);
  RET(0x4338); return; // ret
}

// 39:4339, @-local sub-label of updatePlayedFrequency.
void updatePlayedFrequency__wave_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4339, isWaveChannelUnavailable_hook, 0x434b, 0x433c);
  CYC(0x433c, 0x433e); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(0x433e, 0x4340); updatePlayedFrequency__ret_hook(gb); return; } // jr nz
  CYC(0x433e, 0x4340);
  CYC(0x4340, 0x4341); A = L;
  CYC(0x4341, 0x4343); mem_wr(gb, 0xff1d, A); // NR33
  CYC(0x4343, 0x4344); A = H;
  CYC(0x4344, 0x4346); mem_wr(gb, 0xff1e, A); // NR34
  CYC(0x4346, 0x4348); A = 0x00;
  CYC(0x4348, 0x434a); mem_wr(gb, 0xff1b, A); // NR31
  updatePlayedFrequency__ret_hook(gb); return; // fallthrough
}

// 39:434a, @-local sub-label of updatePlayedFrequency.
void updatePlayedFrequency__ret_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  RET(0x434a); return; // ret
}

// 39:434b, bare global. Sounds can always play; music can only play if the wave channel
// is free and music has not been muted since the previous updateSound call.
// @param[out] a Whether wave channel registers may be written to (0 or 1)
void isWaveChannelUnavailable_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x434b, 0x434e); A = mem_rd(gb, wSoundChannel);
  CYC(0x434e, 0x4350); alu_cp(gb, 0x05);
  if (F & FZ) { CYCT(0x4350, 0x4352); isWaveChannelUnavailable__available_hook(gb); return; } // jr z
  CYC(0x4350, 0x4352);
  CYC(0x4352, 0x4355); A = mem_rd(gb, wChannelsEnabled + 5); // wave channel's slot
  CYC(0x4355, 0x4357); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(0x4357, 0x4359); isWaveChannelUnavailable__unavailable_hook(gb); return; } // jr nz
  CYC(0x4357, 0x4359);
  CYC(0x4359, 0x435c); A = mem_rd(gb, wMusicMuted);
  CYC(0x435c, 0x435e); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(0x435e, 0x4360); isWaveChannelUnavailable__unavailable_hook(gb); return; } // jr z
  CYC(0x435e, 0x4360);
  isWaveChannelUnavailable__available_hook(gb); return; // fallthrough
}

// 39:4360, @-local sub-label of isWaveChannelUnavailable.
void isWaveChannelUnavailable__available_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4360, 0x4362); A = 0x00;
  RET(0x4362); return; // ret
}

// 39:4363, @-local sub-label of isWaveChannelUnavailable.
void isWaveChannelUnavailable__unavailable_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4363, 0x4365); A = 0x01;
  RET(0x4365); return; // ret
}

// 39:4366, bare global.
void getNextChannelByte_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  PUSH(0x4366, BC);
  PUSH(0x4367, DE);
  PUSH(0x4368, HL);
  CYC(0x4369, 0x436c); A = mem_rd(gb, wSoundChannel);
  CYC(0x436c, 0x436e); A = alu_sla(gb, A);
  CYC(0x436e, 0x4370); alu_add(gb, hSoundChannelAddresses & 0xff);
  CYC(0x4370, 0x4371); C = A;
  CYC(0x4371, 0x4372); A = mem_rd(gb, 0xff00 | C);
  CYC(0x4372, 0x4373); C = alu_inc8(gb, C);
  CYC(0x4373, 0x4374); L = A;
  CYC(0x4374, 0x4375); A = mem_rd(gb, 0xff00 | C);
  CYC(0x4375, 0x4376); H = A;
  CYC(0x4376, 0x4379); A = mem_rd(gb, wSoundChannel);
  CYC(0x4379, 0x437b); alu_add(gb, hSoundChannelBanks & 0xff);
  CYC(0x437b, 0x437c); C = A;
  CYC(0x437c, 0x437d); A = mem_rd(gb, 0xff00 | C);
  CYC(0x437d, 0x437e); C = alu_inc8(gb, C);
  CALL_C(0x437e, wMusicReadFunction_hook, wMusicReadFunction, 0x4381);
  PUSH(0x4381, AF);
  CYC(0x4382, 0x4385); A = mem_rd(gb, wSoundChannel);
  CYC(0x4385, 0x4387); A = alu_sla(gb, A);
  CYC(0x4387, 0x4388); B = A;
  CYC(0x4388, 0x4389); A = L;
  CYC(0x4389, 0x438b); C = hSoundChannelAddresses & 0xff;
  CALL_C(0x438b, writeIndexedHighRamAndIncrement_hook, 0x4d25, 0x438e);
  CYC(0x438e, 0x438f); A = H;
  CYC(0x438f, 0x4390); mem_wr(gb, 0xff00 | C, A);
  CYC(0x4390, 0x4391); C = alu_inc8(gb, C);
  SET_AF(POP(0x4391));
  SET_HL(POP(0x4392));
  SET_DE(POP(0x4393));
  SET_BC(POP(0x4394));
  RET(0x4395); return; // ret
}

// 39:4396, bare global.
void doNextChannelCommand_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4396, getNextChannelByte_hook, 0x4366, 0x4399);
  CYC(0x4399, 0x439a); alu_scf(gb);
  CYC(0x439a, 0x439b); alu_ccf(gb);
  CYC(0x439b, 0x439d); alu_cp(gb, 0xf0);
  if (!(F & FC)) { CYCT(0x439d, 0x439f); doNextChannelCommand__cmdf0Toff_hook(gb); return; } // jr nc
  CYC(0x439d, 0x439f);
  CYC(0x439f, 0x43a0); alu_scf(gb);
  CYC(0x43a0, 0x43a1); alu_ccf(gb);
  CYC(0x43a1, 0x43a3); alu_cp(gb, 0xe0);
  if (F & FC) { CYCT(0x43a3, 0x43a5); goto lowRange; } // jr c
  CYC(0x43a3, 0x43a5);
  CYC(0x43a5, 0x43a8); cmde0Toef_hook(gb); return; // jp

lowRange:
  CYC(0x43a8, 0x43a9); alu_scf(gb);
  CYC(0x43a9, 0x43aa); alu_ccf(gb);
  CYC(0x43aa, 0x43ac); alu_cp(gb, 0xd0);
  if (F & FC) { CYCT(0x43ac, 0x43ae); goto standardRange; } // jr c
  CYC(0x43ac, 0x43ae);
  CYC(0x43ae, 0x43b1); cmdVolume_hook(gb); return; // jp

standardRange:
  CYC(0x43b1, 0x43b4); mem_wr(gb, wSoundCmd, A);
  CYC(0x43b4, 0x43b7); standardSoundCmd_hook(gb); return; // jp
}

// 39:43b7, @-local sub-label of doNextChannelCommand.
void doNextChannelCommand__cmdf0Toff_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x43b7, 0x43b8); E = A;
  CYC(0x43b8, 0x43ba); A = 0xff;
  CYC(0x43ba, 0x43bb); alu_sub(gb, E);
  CYC(0x43bb, 0x43be); SET_HL(0x43c2); // doNextChannelCommand@table
  CALL_C(0x43be, readWordFromTable_hook, 0x4d19, 0x43c1);
  CYC(0x43c1, 0x43c2);
  {
    uint16_t target = HL;
    if (target == 0x4923) { channelCmdff_hook(gb); return; }
    if (target == 0x49f6) { channelCmdfe_hook(gb); return; }
    if (target == 0x4423) { channelCmdfd_hook(gb); return; }
    if (target == 0x43eb) { channelCmdf9_hook(gb); return; }
    if (target == 0x4407) { channelCmdf8_hook(gb); return; }
    if (target == 0x44ea) { channelCmdf6_hook(gb); return; }
    if (target == 0x43e8) { channelCmdf3_hook(gb); return; }
    if (target == 0x43e5) { channelCmdf2_hook(gb); return; }
    if (target == 0x43e2) { channelCmdf1_hook(gb); return; }
    if (target == 0x4469) { channelCmdf0_hook(gb); return; }
    HANDOFF(target);
  }
}

// 39:43e2, bare global.
void channelCmdf1_hook(GB *gb) {
  CYC(0x43e2, 0x43e5); doNextChannelCommand_hook(gb); return; // jp
}

// 39:43e5, bare global.
void channelCmdf2_hook(GB *gb) {
  CYC(0x43e5, 0x43e8); doNextChannelCommand_hook(gb); return; // jp
}

// 39:43e8, bare global.
void channelCmdf3_hook(GB *gb) {
  CYC(0x43e8, 0x43eb); doNextChannelCommand_hook(gb); return; // jp
}

// 39:43eb, bare global. Vibrato: sets vibrato to the argument value, does nothing for
// noise channels.
void channelCmdf9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x43eb, 0x43ee); A = mem_rd(gb, wSoundChannel);
  CYC(0x43ee, 0x43ef); alu_scf(gb);
  CYC(0x43ef, 0x43f0); alu_ccf(gb);
  CYC(0x43f0, 0x43f2); alu_cp(gb, 0x06);
  if (!(F & FC)) { CYCT(0x43f2, 0x43f4); goto skip; } // jr nc
  CYC(0x43f2, 0x43f4);
  CALL_C(0x43f4, getNextChannelByte_hook, 0x4366, 0x43f7);
  CYC(0x43f7, 0x43fa); SET_HL(wChannelVibratos);
  PUSH(0x43fa, AF);
  CYC(0x43fb, 0x43fe); A = mem_rd(gb, wSoundChannel);
  CYC(0x43fe, 0x43ff); E = A;
  CYC(0x43ff, 0x4401); D = 0x00;
  CYC(0x4401, 0x4402); alu_add_hl(gb, DE);
  SET_AF(POP(0x4402));
  CYC(0x4403, 0x4404); mem_wr(gb, HL, A);
  CYC(0x4404, 0x4407); doNextChannelCommand_hook(gb); return; // jp

skip:
  CALL_C(0x443f, getNextChannelByte_hook, 0x4366, 0x4442);
  CYC(0x4442, 0x4445); doNextChannelCommand_hook(gb); return; // jp
}

// 39:4407, bare global. Sets sweep to the argument value, does nothing for noise channels.
void channelCmdf8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4407, 0x440a); A = mem_rd(gb, wSoundChannel);
  CYC(0x440a, 0x440b); alu_scf(gb);
  CYC(0x440b, 0x440c); alu_ccf(gb);
  CYC(0x440c, 0x440e); alu_cp(gb, 0x06);
  if (!(F & FC)) { CYCT(0x440e, 0x4410); goto skip; } // jr nc
  CYC(0x440e, 0x4410);
  CALL_C(0x4410, getNextChannelByte_hook, 0x4366, 0x4413);
  CYC(0x4413, 0x4416); SET_HL(wChannelSweep);
  PUSH(0x4416, AF);
  CYC(0x4417, 0x441a); A = mem_rd(gb, wSoundChannel);
  CYC(0x441a, 0x441b); E = A;
  CYC(0x441b, 0x441d); D = 0x00;
  CYC(0x441d, 0x441e); alu_add_hl(gb, DE);
  SET_AF(POP(0x441e));
  CYC(0x441f, 0x4420); mem_wr(gb, HL, A);
  CYC(0x4420, 0x4423); doNextChannelCommand_hook(gb); return; // jp

skip:
  CALL_C(0x443f, getNextChannelByte_hook, 0x4366, 0x4442);
  CYC(0x4442, 0x4445); doNextChannelCommand_hook(gb); return; // jp
}

// 39:4423, bare global. Sets pitch shift to the argument value, does nothing for noise
// channels.
void channelCmdfd_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4423, 0x4426); A = mem_rd(gb, wSoundChannel);
  CYC(0x4426, 0x4427); alu_scf(gb);
  CYC(0x4427, 0x4428); alu_ccf(gb);
  CYC(0x4428, 0x442a); alu_cp(gb, 0x06);
  if (!(F & FC)) { CYCT(0x442a, 0x442c); goto skip; } // jr nc
  CYC(0x442a, 0x442c);
  CALL_C(0x442c, getNextChannelByte_hook, 0x4366, 0x442f);
  CYC(0x442f, 0x4432); SET_HL(wChannelPitchShift);
  PUSH(0x4432, AF);
  CYC(0x4433, 0x4436); A = mem_rd(gb, wSoundChannel);
  CYC(0x4436, 0x4437); E = A;
  CYC(0x4437, 0x4439); D = 0x00;
  CYC(0x4439, 0x443a); alu_add_hl(gb, DE);
  SET_AF(POP(0x443a));
  CYC(0x443b, 0x443c); mem_wr(gb, HL, A);
  CYC(0x443c, 0x443f); doNextChannelCommand_hook(gb); return; // jp

skip:
  CALL_C(0x443f, getNextChannelByte_hook, 0x4366, 0x4442);
  CYC(0x4442, 0x4445); doNextChannelCommand_hook(gb); return; // jp
}

// 39:4445, bare global. Sets the channel envelopes to the lower 3 bits of the command
// value (note start) and the argument value (note end). Should not be used with wave or
// noise channels or else wChannelEnvelopes2 and wChannelsEnabled get messed up for square
// channels.
void cmde0Toef_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4445, 0x4447); alu_and(gb, 0x07);
  CYC(0x4447, 0x444a); SET_HL(wChannelEnvelopes);
  PUSH(0x444a, AF);
  CYC(0x444b, 0x444e); A = mem_rd(gb, wSoundChannel);
  CYC(0x444e, 0x444f); E = A;
  CYC(0x444f, 0x4451); D = 0x00;
  CYC(0x4451, 0x4452); alu_add_hl(gb, DE);
  SET_AF(POP(0x4452));
  CYC(0x4453, 0x4454); mem_wr(gb, HL, A);
  CALL_C(0x4454, getNextChannelByte_hook, 0x4366, 0x4457);
  CYC(0x4457, 0x4459); alu_and(gb, 0x07);
  CYC(0x4459, 0x445c); SET_HL(wChannelEnvelopes2);
  PUSH(0x445c, AF);
  CYC(0x445d, 0x4460); A = mem_rd(gb, wSoundChannel);
  CYC(0x4460, 0x4461); E = A;
  CYC(0x4461, 0x4463); D = 0x00;
  CYC(0x4463, 0x4464); alu_add_hl(gb, DE);
  SET_AF(POP(0x4464));
  CYC(0x4465, 0x4466); mem_wr(gb, HL, A);
  CYC(0x4466, 0x4469); doNextChannelCommand_hook(gb); return; // jp
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
  uint16_t sp0_ = gb->sp;
  CYC(0x4469, 0x446c); A = mem_rd(gb, wSoundChannel);
  CYC(0x446c, 0x446e); alu_cp(gb, 0x07);
  if (F & FZ) { CYCT(0x446e, 0x4470); channelCmdf0__channel7_hook(gb); return; } // jr z
  CYC(0x446e, 0x4470);
  CALL_C(0x4470, getNextChannelByte_hook, 0x4366, 0x4473);
  PUSH(0x4473, AF);
  CYC(0x4474, 0x4476); alu_and(gb, 0x3f);
  if (F & FZ) { CYCT(0x4476, 0x4478); channelCmdf0__disableLengthTimer_hook(gb); return; } // jr z
  CYC(0x4476, 0x4478);
  SET_AF(POP(0x4478));
  CYC(0x4479, 0x447c); SET_HL(wChannelDutyCycles);
  PUSH(0x447c, AF);
  CYC(0x447d, 0x4480); A = mem_rd(gb, wSoundChannel);
  CYC(0x4480, 0x4481); E = A;
  CYC(0x4481, 0x4483); D = 0x00;
  CYC(0x4483, 0x4484); alu_add_hl(gb, DE);
  SET_AF(POP(0x4484));
  CYC(0x4485, 0x4486); mem_wr(gb, HL, A);
  CYC(0x4486, 0x4488); A = 0x41;
  CYC(0x4488, 0x448b); SET_HL(wChannelFrequencyModeAndLengthTimerEnabled);
  PUSH(0x448b, AF);
  CYC(0x448c, 0x448f); A = mem_rd(gb, wSoundChannel);
  CYC(0x448f, 0x4490); E = A;
  CYC(0x4490, 0x4492); D = 0x00;
  CYC(0x4492, 0x4493); alu_add_hl(gb, DE);
  SET_AF(POP(0x4493));
  CYC(0x4494, 0x4495); mem_wr(gb, HL, A);
  CYC(0x4495, 0x4498); doNextChannelCommand_hook(gb); return; // jp
}

// 39:4498, @-local sub-label of channelCmdf0.
void channelCmdf0__disableLengthTimer_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  SET_AF(POP(0x4498));
  CYC(0x4499, 0x449b); alu_and(gb, 0xc0);
  CYC(0x449b, 0x449e); SET_HL(wChannelDutyCycles);
  PUSH(0x449e, AF);
  CYC(0x449f, 0x44a2); A = mem_rd(gb, wSoundChannel);
  CYC(0x44a2, 0x44a3); E = A;
  CYC(0x44a3, 0x44a5); D = 0x00;
  CYC(0x44a5, 0x44a6); alu_add_hl(gb, DE);
  SET_AF(POP(0x44a6));
  CYC(0x44a7, 0x44a8); mem_wr(gb, HL, A);
  CYC(0x44a8, 0x44aa); A = 0x01;
  CYC(0x44aa, 0x44ad); SET_HL(wChannelFrequencyModeAndLengthTimerEnabled);
  PUSH(0x44ad, AF);
  CYC(0x44ae, 0x44b1); A = mem_rd(gb, wSoundChannel);
  CYC(0x44b1, 0x44b2); E = A;
  CYC(0x44b2, 0x44b4); D = 0x00;
  CYC(0x44b4, 0x44b5); alu_add_hl(gb, DE);
  SET_AF(POP(0x44b5));
  CYC(0x44b6, 0x44b7); mem_wr(gb, HL, A);
  CYC(0x44b7, 0x44ba); doNextChannelCommand_hook(gb); return; // jp
}

// 39:44ba, @-local sub-label of channelCmdf0.
void channelCmdf0__channel7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x44ba, getNextChannelByte_hook, 0x4366, 0x44bd);
  CYC(0x44bd, 0x44bf); mem_wr(gb, 0xff21, A); // NR42
  CYC(0x44bf, 0x44c1); A = 0x00;
  CYC(0x44c1, 0x44c3); mem_wr(gb, 0xff20, A); // NR41
  CYC(0x44c3, 0x44c5); A = 0x80;
  CYC(0x44c5, 0x44c8); mem_wr(gb, wChannel7TriggerOnNextSound, A);
  CYC(0x44c8, 0x44cb); doNextChannelCommand_hook(gb); return; // jp
}

// 39:44cb, bare global. Command $d0-$df: sets volume to the lower 3 bits of the command
// value, does nothing for channel 4 (and is also useless for channel 5).
void cmdVolume_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  PUSH(0x44cb, AF);
  CYC(0x44cc, 0x44cf); A = mem_rd(gb, wSoundChannel);
  CYC(0x44cf, 0x44d1); alu_cp(gb, 0x04);
  if (F & FZ) { CYCT(0x44d1, 0x44d3); cmdVolume__next_hook(gb); return; } // jr z
  CYC(0x44d1, 0x44d3);
  SET_AF(POP(0x44d3));
  CYC(0x44d4, 0x44d6); alu_and(gb, 0x0f);
  CYC(0x44d6, 0x44d9); SET_HL(wChannelVolumes);
  PUSH(0x44d9, AF);
  CYC(0x44da, 0x44dd); A = mem_rd(gb, wSoundChannel);
  CYC(0x44dd, 0x44de); E = A;
  CYC(0x44de, 0x44e0); D = 0x00;
  CYC(0x44e0, 0x44e1); alu_add_hl(gb, DE);
  SET_AF(POP(0x44e1));
  CYC(0x44e2, 0x44e3); mem_wr(gb, HL, A);
  CYC(0x44e3, 0x44e6); doNextChannelCommand_hook(gb); return; // jp
}

// 39:44e6, @-local sub-label of cmdVolume.
void cmdVolume__next_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  SET_AF(POP(0x44e6));
  CYC(0x44e7, 0x44ea); doNextChannelCommand_hook(gb); return; // jp
}

// 39:44ea, bare global. For square channels, sets wChannelDutyCycles to the argument value
// shifted 6 bits to the left. For wave channels, sets wChannelDutyCycles to the argument
// value and updates the waveform based on that index. Should not be used with noise
// channels or else wChannelEnvelopeStates gets messed up for channel 0 or 1.
void channelCmdf6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x44ea, 0x44ed); A = mem_rd(gb, wSoundChannel);
  CYC(0x44ed, 0x44ef); alu_cp(gb, 0x04);
  if (F & FZ) { CYCT(0x44ef, 0x44f1); channelCmdf6__wave_hook(gb); return; } // jr z
  CYC(0x44ef, 0x44f1);
  CYC(0x44f1, 0x44f3); alu_cp(gb, 0x05);
  if (F & FZ) { CYCT(0x44f3, 0x44f5); channelCmdf6__wave_hook(gb); return; } // jr z
  CYC(0x44f3, 0x44f5);
  CALL_C(0x44f5, getNextChannelByte_hook, 0x4366, 0x44f8);
  CYC(0x44f8, 0x44fa); alu_and(gb, 0x03);
  CYC(0x44fa, 0x44fc); A = alu_swap(gb, A);
  CYC(0x44fc, 0x44fe); A = alu_sla(gb, A);
  CYC(0x44fe, 0x4500); A = alu_sla(gb, A);
  CYC(0x4500, 0x4503); SET_HL(wChannelDutyCycles);
  PUSH(0x4503, AF);
  CYC(0x4504, 0x4507); A = mem_rd(gb, wSoundChannel);
  CYC(0x4507, 0x4508); E = A;
  CYC(0x4508, 0x450a); D = 0x00;
  CYC(0x450a, 0x450b); alu_add_hl(gb, DE);
  SET_AF(POP(0x450b));
  CYC(0x450c, 0x450d); mem_wr(gb, HL, A);
  CYC(0x450d, 0x4510); doNextChannelCommand_hook(gb); return; // jp
}

// 39:4510, @-local sub-label of channelCmdf6.
void channelCmdf6__wave_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4510, getNextChannelByte_hook, 0x4366, 0x4513);
  CYC(0x4513, 0x4516); SET_HL(wChannelDutyCycles);
  PUSH(0x4516, AF);
  CYC(0x4517, 0x451a); A = mem_rd(gb, wSoundChannel);
  CYC(0x451a, 0x451b); E = A;
  CYC(0x451b, 0x451d); D = 0x00;
  CYC(0x451d, 0x451e); alu_add_hl(gb, DE);
  SET_AF(POP(0x451e));
  CYC(0x451f, 0x4520); mem_wr(gb, HL, A);
  CYC(0x4520, 0x4523); mem_wr(gb, wWaveformIndex, A);
  CALL_C(0x4523, setWaveform_hook, 0x49c1, 0x4526);
  CYC(0x4526, 0x4529); doNextChannelCommand_hook(gb); return; // jp
}

// 39:4529, bare global.
void standardSoundCmd_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4529, 0x452c); A = mem_rd(gb, wSoundChannel);
  CYC(0x452c, 0x452f); SET_HL(0x4533); // @table
  CALL_C(0x452f, readWordFromTable_hook, 0x4d19, 0x4532);
  CYC(0x4532, 0x4533);
  {
    uint16_t target = HL;
    if (target == 0x4543) { standardSoundCmd__channel0To3_hook(gb); return; }
    if (target == 0x47c8) { standardCmdChannels4To5_hook(gb); return; }
    if (target == 0x48cf) { standardCmdChannel6_hook(gb); return; }
    if (target == 0x4909) { standardCmdChannel7_hook(gb); return; }
    HANDOFF(target);
  }
}

// 39:4533, @-local sub-label of standardSoundCmd. This is the jump table's own raw bytes,
// decoded exactly as real hardware would if control ever reached here as code (it never
// does in practice: standardSoundCmd only ever indexes into this table, it doesn't jump to
// its start).
void standardSoundCmd__table_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4533, 0x4534); B = E; // ld b,e
  CYC(0x4534, 0x4535); B = L; // ld b,l
  CYC(0x4535, 0x4536); B = E;
  CYC(0x4536, 0x4537); B = L;
  CYC(0x4537, 0x4538); B = E;
  CYC(0x4538, 0x4539); B = L;
  CYC(0x4539, 0x453a); B = E;
  CYC(0x453a, 0x453b); B = L;
  if (F & FZ) { RET_TAKEN(0x453b); return; } // ret z
  CYC(0x453b, 0x453c);
  CYC(0x453c, 0x453d); B = A;
  if (F & FZ) { RET_TAKEN(0x453d); return; } // ret z
  CYC(0x453d, 0x453e);
  CYC(0x453e, 0x453f); B = A;
  CALL_ASM_RST(0x453f, 0x0008, 0x4540);
  CYC(0x4540, 0x4541); C = B;
  CYC(0x4541, 0x4542); alu_add_hl(gb, BC);
  CYC(0x4542, 0x4543); C = C;
  standardSoundCmd__channel0To3_hook(gb); return; // fallthrough
}

// 39:4543, @-local sub-label of standardSoundCmd.
void standardSoundCmd__channel0To3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4543, 0x4546); SET_HL(wChannelFrequencyModeAndLengthTimerEnabled);
  CYC(0x4546, 0x4549); A = mem_rd(gb, wSoundChannel);
  CYC(0x4549, 0x454a); E = A;
  CYC(0x454a, 0x454c); D = 0x00;
  CYC(0x454c, 0x454d); alu_add_hl(gb, DE);
  CYC(0x454d, 0x454e); A = mem_rd(gb, HL);
  CYC(0x454e, 0x4550); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(0x4550, 0x4552); goto notArbitraryMode; } // jr z
  CYC(0x4550, 0x4552);
  CALL_C(0x4552, getNextChannelByte_hook, 0x4366, 0x4555);
  CYC(0x4555, 0x4556); L = A;
  CYC(0x4556, 0x4559); A = mem_rd(gb, wSoundCmd);
  CYC(0x4559, 0x455a); H = A;
  CYC(0x455a, 0x455d); standardSoundCmd__arbitraryFrequency_hook(gb); return; // jp

notArbitraryMode:
  CYC(0x455d, 0x4560); A = mem_rd(gb, wSoundCmd);
  CYC(0x4560, 0x4562); alu_cp(gb, 0x60);
  if (F & FZ) { CYCT(0x4562, 0x4564); standardSoundCmd__cmd60_hook(gb); return; } // jr z
  CYC(0x4562, 0x4564);
  CYC(0x4564, 0x4566); alu_cp(gb, 0x61);
  if (F & FZ) { CYCT(0x4566, 0x4568); standardSoundCmd__cmd61_hook(gb); return; } // jr z
  CYC(0x4566, 0x4568);
  CYC(0x4568, 0x456b); standardSoundCmd__cmdFrequency_hook(gb); return; // jp
}

// 39:456b, @-local sub-label of standardSoundCmd.
void standardSoundCmd__cmd60_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x456b, 0x456e); SET_HL(wChannelEnvelopes2);
  CYC(0x456e, 0x4571); A = mem_rd(gb, wSoundChannel);
  CYC(0x4571, 0x4572); E = A;
  CYC(0x4572, 0x4574); D = 0x00;
  CYC(0x4574, 0x4575); alu_add_hl(gb, DE);
  CYC(0x4575, 0x4576); A = mem_rd(gb, HL);
  CYC(0x4576, 0x4578); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(0x4578, 0x457a); standardSoundCmd__cmd61_hook(gb); return; } // jr nz
  CYC(0x4578, 0x457a);
  CYC(0x457a, 0x457c); A = 0x02;
  CYC(0x457c, 0x457f); SET_HL(wChannelEnvelopeStates);
  PUSH(0x457f, AF);
  CYC(0x4580, 0x4583); A = mem_rd(gb, wSoundChannel);
  CYC(0x4583, 0x4584); E = A;
  CYC(0x4584, 0x4586); D = 0x00;
  CYC(0x4586, 0x4587); alu_add_hl(gb, DE);
  SET_AF(POP(0x4587));
  CYC(0x4588, 0x4589); mem_wr(gb, HL, A);
  CALL_C(0x4589, getChannelVolume_hook, 0x4783, 0x458c);
  CYC(0x458c, 0x458e); A = alu_sla(gb, A);
  CYC(0x458e, 0x4590); A = alu_sla(gb, A);
  CYC(0x4590, 0x4592); A = alu_sla(gb, A);
  CYC(0x4592, 0x4594); A = alu_sla(gb, A);
  CYC(0x4594, 0x4596); C = 0x01;
  CYC(0x4596, 0x4597); alu_or(gb, C);
  CYC(0x4597, 0x459a); mem_wr(gb, wSoundCmdEnvelope, A);
  CALL_C(0x459a, updateSquareChannelVolume_hook, 0x4714, 0x459d);
  CALL_C(0x459d, updateSoundFrequencyAndPlay_hook, 0x41f3, 0x45a0);
  standardSoundCmd__cmd61_hook(gb); return; // fallthrough
}

// 39:45a0, @-local sub-label of standardSoundCmd.
void standardSoundCmd__cmd61_hook(GB *gb) {
  CYC(0x45a0, 0x45a3); setChannelWaitCounter_hook(gb); return; // jp
}

// 39:45a3, @-local sub-label of standardSoundCmd.
void standardSoundCmd__cmdFrequency_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x45a3, 0x45a6); A = mem_rd(gb, wSoundCmd);
  CYC(0x45a6, 0x45a8); alu_sub(gb, 0x0c);
  CYC(0x45a8, 0x45ab); SET_HL(0x4a22); // soundFrequencyTable
  CALL_C(0x45ab, readWordFromTable_hook, 0x4d19, 0x45ae);
  standardSoundCmd__arbitraryFrequency_hook(gb); return; // fallthrough
}

// 39:45ae, @-local sub-label of standardSoundCmd.
void standardSoundCmd__arbitraryFrequency_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x45ae, setSoundFrequency_hook, 0x461a, 0x45b1);
  CYC(0x45b1, 0x45b3); A = 0x00;
  CYC(0x45b3, 0x45b6); SET_HL(wChannelEnvelopeStates);
  PUSH(0x45b6, AF);
  CYC(0x45b7, 0x45ba); A = mem_rd(gb, wSoundChannel);
  CYC(0x45ba, 0x45bb); E = A;
  CYC(0x45bb, 0x45bd); D = 0x00;
  CYC(0x45bd, 0x45be); alu_add_hl(gb, DE);
  SET_AF(POP(0x45be));
  CYC(0x45bf, 0x45c0); mem_wr(gb, HL, A);
  CALL_C(0x45c0, handleEnvelopes_hook, 0x464c, 0x45c3);
  CYC(0x45c3, 0x45c5); A = 0x00;
  CYC(0x45c5, 0x45c8); SET_HL(wChannelVibratoActive);
  PUSH(0x45c8, AF);
  CYC(0x45c9, 0x45cc); A = mem_rd(gb, wSoundChannel);
  CYC(0x45cc, 0x45cd); E = A;
  CYC(0x45cd, 0x45cf); D = 0x00;
  CYC(0x45cf, 0x45d0); alu_add_hl(gb, DE);
  SET_AF(POP(0x45d0));
  CYC(0x45d1, 0x45d2); mem_wr(gb, HL, A);
  CYC(0x45d2, 0x45d4); A = 0x00;
  CYC(0x45d4, 0x45d7); SET_HL(wChannelVibratos);
  CYC(0x45d7, 0x45da); A = mem_rd(gb, wSoundChannel);
  CYC(0x45da, 0x45db); E = A;
  CYC(0x45db, 0x45dd); D = 0x00;
  CYC(0x45dd, 0x45de); alu_add_hl(gb, DE);
  CYC(0x45de, 0x45df); A = mem_rd(gb, HL);
  CYC(0x45df, 0x45e1); alu_and(gb, 0xf0);
  CYC(0x45e1, 0x45e3); A = alu_srl(gb, A);
  CYC(0x45e3, 0x45e5); A = alu_srl(gb, A);
  CYC(0x45e5, 0x45e7); A = alu_srl(gb, A);
  CYC(0x45e7, 0x45ea); SET_HL(wChannelVibratoCounters);
  PUSH(0x45ea, AF);
  CYC(0x45eb, 0x45ee); A = mem_rd(gb, wSoundChannel);
  CYC(0x45ee, 0x45ef); E = A;
  CYC(0x45ef, 0x45f1); D = 0x00;
  CYC(0x45f1, 0x45f2); alu_add_hl(gb, DE);
  SET_AF(POP(0x45f2));
  CYC(0x45f3, 0x45f4); mem_wr(gb, HL, A);
  CALL_C(0x45f4, updatePlayedFrequency_hook, 0x42ea, 0x45f7);
  setChannelWaitCounter_hook(gb); return; // fallthrough
}
