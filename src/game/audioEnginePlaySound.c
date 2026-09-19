#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x39, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x39, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CALL_C(0x45f7, getNextChannelByte_hook, 0x4366, 0x45fa);
  CYC(0x45fa, 0x45fb); A = alu_dec8(gb, A);
  CYC(0x45fb, 0x45fe); SET_HL(wChannelWaitCounters);
  CYC(0x45fe, 0x45ff); push_effect(gb, AF);
  CYC(0x45ff, 0x4602); A = W8(wSoundChannel);
  CYC(0x4602, 0x4603); E = A;
  CYC(0x4603, 0x4605); D = 0x00;
  CYC(0x4605, 0x4606); alu_add_hl(gb, DE);
  CYC(0x4606, 0x4607); SET_AF(pop_effect(gb));
  CYC(0x4607, 0x4608); mem_wr(gb, HL, A);
  RET(0x4608);
}

// Determines the time to wait until the envelope with sweep pace c is
// expected to have reached the volume level in b
void getWaitTimeForEnvelope_hook(GB *gb) {
  CYC(0x4609, 0x460c); SET_HL(0x4ad0);
  CYC(0x460c, 0x460d); A = B;
  CYC(0x460d, 0x460f); A = alu_sla(gb, A);
  CYC(0x460f, 0x4611); A = alu_sla(gb, A);
  CYC(0x4611, 0x4613); A = alu_sla(gb, A);
  CYC(0x4613, 0x4614); alu_add(gb, C);
  CYC(0x4614, 0x4616); D = 0x00;
  CYC(0x4616, 0x4617); E = A;
  CYC(0x4617, 0x4618); alu_add_hl(gb, DE);
  CYC(0x4618, 0x4619); A = mem_rd(gb, HL);
  RET(0x4619);
}

// Sends wSoundFrequency to given value plus value in table at wChannelPitchShift.
void setSoundFrequency_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x461a, 0x461b); push_effect(gb, HL);
  CYC(0x461b, 0x461e); SET_HL(wChannelPitchShift);
  CYC(0x461e, 0x4621); A = W8(wSoundChannel);
  CYC(0x4621, 0x4622); E = A;
  CYC(0x4622, 0x4624); D = 0x00;
  CYC(0x4624, 0x4625); alu_add_hl(gb, DE);
  CYC(0x4625, 0x4626); A = mem_rd(gb, HL);
  CYC(0x4626, 0x4627); D = A;
  CYC(0x4627, 0x4629); D = alu_sla(gb, D);
  if (F & FC) { CYCT(0x4629, 0x462b); goto negOffset; } // jr c
  CYC(0x4629, 0x462b);
  CYC(0x462b, 0x462d); D = 0x00;
  CYC(0x462d, 0x462f); goto haveOffset; // jr
negOffset:
  CYC(0x462f, 0x4631); D = 0xff;
haveOffset:
  CYC(0x4631, 0x4632); E = A;
  CYC(0x4632, 0x4633); SET_HL(pop_effect(gb));
  CYC(0x4633, 0x4634); alu_add_hl(gb, DE);
  CYC(0x4634, 0x4637); A = W8(wSoundChannel);
  CYC(0x4637, 0x4639); A = alu_sla(gb, A);
  CYC(0x4639, 0x463a); B = A;
  CYC(0x463a, 0x463b); A = L;
  CYC(0x463b, 0x463d); C = 0xf2;
  CALL_C(0x463d, writeIndexedHighRamAndIncrement_hook, 0x4d25, 0x4640);
  CYC(0x4640, 0x4641); A = H;
  CYC(0x4641, 0x4642); mem_wr(gb, 0xff00 | C, A);
  CYC(0x4642, 0x4643); C = alu_inc8(gb, C);
  CYC(0x4643, 0x4644); A = L;
  CYC(0x4644, 0x4647); W8(wSoundFrequencyL) = A;
  CYC(0x4647, 0x4648); A = H;
  CYC(0x4648, 0x464b); W8(wSoundFrequencyH) = A;
  RET(0x464b);
}

// Handles envelopes for square channels, and redirects channel 4 to updateChannel4Volume
void handleEnvelopes_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x464c, 0x464f); A = W8(wSoundChannel);
  CYC(0x464f, 0x4651); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(0x4651, 0x4653); goto notChannel4; } // jr nz
  CYC(0x4651, 0x4653);
  CYC(0x4653, 0x4656); updateChannel4Volume_hook(gb); return; // jp
notChannel4:
  CYC(0x4656, 0x4659); SET_HL(wChannelEnvelopeStates);
  CYC(0x4659, 0x465c); A = W8(wSoundChannel);
  CYC(0x465c, 0x465d); E = A;
  CYC(0x465d, 0x465f); D = 0x00;
  CYC(0x465f, 0x4660); alu_add_hl(gb, DE);
  CYC(0x4660, 0x4661); A = mem_rd(gb, HL);
  CYC(0x4661, 0x4663); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(0x4663, 0x4665); goto checkEnvelopeRequested; } // jr z
  CYC(0x4663, 0x4665);
  CYC(0x4665, 0x4667); alu_cp(gb, 0x01);
  if (F & FZ) { CYCT(0x4667, 0x4669); goto waitForNoteStartEnvelope; } // jr z
  CYC(0x4667, 0x4669);
  CYC(0x4669, 0x466b); A = 0x00;
  CYC(0x466b, 0x466e); W8(wSoundCmdEnvelope) = A;
  RET(0x466e);
  return;
checkEnvelopeRequested:
  CYC(0x466f, 0x4672); SET_HL(wChannelEnvelopes);
  CYC(0x4672, 0x4675); A = W8(wSoundChannel);
  CYC(0x4675, 0x4676); E = A;
  CYC(0x4676, 0x4678); D = 0x00;
  CYC(0x4678, 0x4679); alu_add_hl(gb, DE);
  CYC(0x4679, 0x467a); A = mem_rd(gb, HL);
  CYC(0x467a, 0x467c); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(0x467c, 0x467e); goto checkAndStartNoteEndEnvelope; } // jr z
  CYC(0x467c, 0x467e);
  CYC(0x467e, 0x467f); C = A;
  CYC(0x467f, 0x4681); alu_or(gb, 0x18);
  CYC(0x4681, 0x4684); W8(wSoundCmdEnvelope) = A;
  CYC(0x4684, 0x4685); push_effect(gb, BC);
  CALL_C(0x4685, getChannelVolume_hook, 0x4783, 0x4688);
  CYC(0x4688, 0x4689); SET_BC(pop_effect(gb));
  CYC(0x4689, 0x468a); B = A;
  CALL_C(0x468a, getWaitTimeForEnvelope_hook, 0x4609, 0x468d);
  CYC(0x468d, 0x4690); SET_HL(wChannelEnvelopeWaitCounters);
  CYC(0x4690, 0x4691); push_effect(gb, AF);
  CYC(0x4691, 0x4694); A = W8(wSoundChannel);
  CYC(0x4694, 0x4695); E = A;
  CYC(0x4695, 0x4697); D = 0x00;
  CYC(0x4697, 0x4698); alu_add_hl(gb, DE);
  CYC(0x4698, 0x4699); SET_AF(pop_effect(gb));
  CYC(0x4699, 0x469a); mem_wr(gb, HL, A);
  CYC(0x469a, 0x469c); A = 0x01;
  CYC(0x469c, 0x469f); SET_HL(wChannelEnvelopeStates);
  CYC(0x469f, 0x46a0); push_effect(gb, AF);
  CYC(0x46a0, 0x46a3); A = W8(wSoundChannel);
  CYC(0x46a3, 0x46a4); E = A;
  CYC(0x46a4, 0x46a6); D = 0x00;
  CYC(0x46a6, 0x46a7); alu_add_hl(gb, DE);
  CYC(0x46a7, 0x46a8); SET_AF(pop_effect(gb));
  CYC(0x46a8, 0x46a9); mem_wr(gb, HL, A);
  CYC(0x46a9, 0x46ac); updateSquareChannelVolume_hook(gb); return; // jp
waitForNoteStartEnvelope:
  CYC(0x46ac, 0x46af); SET_HL(wChannelEnvelopeWaitCounters);
  CYC(0x46af, 0x46b2); A = W8(wSoundChannel);
  CYC(0x46b2, 0x46b3); E = A;
  CYC(0x46b3, 0x46b5); D = 0x00;
  CYC(0x46b5, 0x46b6); alu_add_hl(gb, DE);
  CYC(0x46b6, 0x46b7); A = mem_rd(gb, HL);
  CYC(0x46b7, 0x46b9); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(0x46b9, 0x46bb); goto checkAndStartNoteEndEnvelope; } // jr z
  CYC(0x46b9, 0x46bb);
  CYC(0x46bb, 0x46be); SET_HL(wChannelEnvelopeWaitCounters);
  CYC(0x46be, 0x46c1); A = W8(wSoundChannel);
  CYC(0x46c1, 0x46c2); E = A;
  CYC(0x46c2, 0x46c4); D = 0x00;
  CYC(0x46c4, 0x46c5); alu_add_hl(gb, DE);
  CYC(0x46c5, 0x46c6); A = mem_rd(gb, HL);
  CYC(0x46c6, 0x46c7); A = alu_dec8(gb, A);
  CYC(0x46c7, 0x46c8); mem_wr(gb, HL, A);
  CYC(0x46c8, 0x46ca); A = 0x00;
  CYC(0x46ca, 0x46cd); W8(wSoundCmdEnvelope) = A;
  RET(0x46cd);
  return;
checkAndStartNoteEndEnvelope:
  CYC(0x46ce, 0x46d1); SET_HL(wChannelEnvelopes2);
  CYC(0x46d1, 0x46d4); A = W8(wSoundChannel);
  CYC(0x46d4, 0x46d5); E = A;
  CYC(0x46d5, 0x46d7); D = 0x00;
  CYC(0x46d7, 0x46d8); alu_add_hl(gb, DE);
  CYC(0x46d8, 0x46d9); A = mem_rd(gb, HL);
  CYC(0x46d9, 0x46db); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(0x46db, 0x46dd); goto notZeroEnvelope2; } // jr nz
  CYC(0x46db, 0x46dd);
  CYC(0x46dd, 0x46df); A = 0x02;
  CYC(0x46df, 0x46e1); goto haveEnvelopeState; // jr
notZeroEnvelope2:
  CYC(0x46e1, 0x46e3); A = 0x03;
haveEnvelopeState:
  CYC(0x46e3, 0x46e6); SET_HL(wChannelEnvelopeStates);
  CYC(0x46e6, 0x46e7); push_effect(gb, AF);
  CYC(0x46e7, 0x46ea); A = W8(wSoundChannel);
  CYC(0x46ea, 0x46eb); E = A;
  CYC(0x46eb, 0x46ed); D = 0x00;
  CYC(0x46ed, 0x46ee); alu_add_hl(gb, DE);
  CYC(0x46ee, 0x46ef); SET_AF(pop_effect(gb));
  CYC(0x46ef, 0x46f0); mem_wr(gb, HL, A);
  CALL_C(0x46f0, getChannelVolume_hook, 0x4783, 0x46f3);
  CYC(0x46f3, 0x46f5); A = alu_sla(gb, A);
  CYC(0x46f5, 0x46f7); A = alu_sla(gb, A);
  CYC(0x46f7, 0x46f9); A = alu_sla(gb, A);
  CYC(0x46f9, 0x46fb); A = alu_sla(gb, A);
  CYC(0x46fb, 0x46fe); W8(wSoundCmdEnvelope) = A;
  CYC(0x46fe, 0x4701); SET_HL(wChannelEnvelopes2);
  CYC(0x4701, 0x4704); A = W8(wSoundChannel);
  CYC(0x4704, 0x4705); E = A;
  CYC(0x4705, 0x4707); D = 0x00;
  CYC(0x4707, 0x4708); alu_add_hl(gb, DE);
  CYC(0x4708, 0x4709); A = mem_rd(gb, HL);
  CYC(0x4709, 0x470a); C = A;
  CYC(0x470a, 0x470d); A = W8(wSoundCmdEnvelope);
  CYC(0x470d, 0x470e); alu_or(gb, C);
  CYC(0x470e, 0x4711); W8(wSoundCmdEnvelope) = A;
  CYC(0x4711, 0x4714); updateSquareChannelVolume_hook(gb); return; // jp
}

void updateSquareChannelVolume_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4714, 0x4717); A = W8(wSoundChannel);
  CYC(0x4717, 0x4719); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(0x4719, 0x471b); goto setVolume; } // jr nc
  CYC(0x4719, 0x471b);
  CYC(0x471b, 0x471e); A = W8(wMusicVolume);
  CYC(0x471e, 0x4720); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(0x4720, 0x4722); goto doRet; } // jr z
  CYC(0x4720, 0x4722);
  CYC(0x4722, 0x4725); A = W8(wSoundChannel);
  CYC(0x4725, 0x4726); A = alu_inc8(gb, A);
  CYC(0x4726, 0x4727); A = alu_inc8(gb, A);
  CYC(0x4727, 0x4728); E = A;
  CYC(0x4728, 0x472b); SET_HL(wChannelsEnabled);
  CYC(0x472b, 0x472d); D = 0x00;
  CYC(0x472d, 0x472e); alu_add_hl(gb, DE);
  CYC(0x472e, 0x472f); A = mem_rd(gb, HL);
  CYC(0x472f, 0x4731); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(0x4731, 0x4733); goto setVolume; } // jr z
  CYC(0x4731, 0x4733);
doRet:
  RET(0x4733);
  return;
setVolume:
  CYC(0x4734, 0x4737); A = W8(wSoundChannel);
  CYC(0x4737, 0x4739); alu_and(gb, 0x01);
  if (!(F & FZ)) { CYCT(0x4739, 0x473b); goto sweepDone; } // jr nz
  CYC(0x4739, 0x473b);
  CYC(0x473b, 0x473d); A = 0x08;
  CYC(0x473d, 0x473f); mem_wr(gb, 0xff10, A);
sweepDone:
  CYC(0x473f, 0x4742); A = W8(wSoundChannel);
  CYC(0x4742, 0x4744); alu_and(gb, 0x01);
  CYC(0x4744, 0x4745); B = A;
  CYC(0x4745, 0x4747); A = alu_sla(gb, A);
  CYC(0x4747, 0x4749); A = alu_sla(gb, A);
  CYC(0x4749, 0x474a); alu_add(gb, B);
  CYC(0x474a, 0x474b); B = A;
  CYC(0x474b, 0x474e); A = W8(wSoundCmdEnvelope);
  CYC(0x474e, 0x4750); C = 0x12;
  CALL_C(0x4750, writeIndexedHighRamAndIncrement_hook, 0x4d25, 0x4753);
  CYC(0x4753, 0x4756); SET_HL(wChannelFrequencyModeAndLengthTimerEnabled);
  CYC(0x4756, 0x4759); A = W8(wSoundChannel);
  CYC(0x4759, 0x475a); E = A;
  CYC(0x475a, 0x475c); D = 0x00;
  CYC(0x475c, 0x475d); alu_add_hl(gb, DE);
  CYC(0x475d, 0x475e); A = mem_rd(gb, HL);
  CYC(0x475e, 0x4760); alu_and(gb, 0x40);
  CYC(0x4760, 0x4762); alu_or(gb, 0x80);
  CYC(0x4762, 0x4765); W8(wSoundCmdEnvelope) = A;
  RET(0x4765);
}

// Updates wWaveChannelVolume+4 and, if changed, writes to R_NR32 if possible
void updateChannel4Volume_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4766, getWaveChannelVolume_hook, 0x489e, 0x4769);
  CYC(0x4769, 0x476a); B = A;
  CYC(0x476a, 0x476d); A = mem_rd(gb, wWaveChannelVolume + 4);
  CYC(0x476d, 0x476e); alu_cp(gb, B);
  if (F & FZ) { CYCT(0x476e, 0x4770); goto ret4; } // jr z
  CYC(0x476e, 0x4770);
  CALL_C(0x4770, getWaveChannelVolume_hook, 0x489e, 0x4773);
  CYC(0x4773, 0x4776); mem_wr(gb, wWaveChannelVolume + 4, A);
  CALL_C(0x4776, isWaveChannelUnavailable_hook, 0x434b, 0x4779);
  CYC(0x4779, 0x477b); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(0x477b, 0x477d); goto ret4; } // jr nz
  CYC(0x477b, 0x477d);
  CYC(0x477d, 0x4780); A = mem_rd(gb, wWaveChannelVolume + 4);
  CYC(0x4780, 0x4782); mem_wr(gb, 0xff1c, A);
ret4:
  RET(0x4782);
}

// Shared tail of getChannelVolume, reached directly by a real ROM `call $478c`
// from standardCmdChannel6 (NOT independently hooked).
static void getChannelVolume_fullVolume(GB *gb) {
  CYC(0x479b, 0x479e); SET_HL(wChannelVolumes);
  CYC(0x479e, 0x47a1); A = W8(wSoundChannel);
  CYC(0x47a1, 0x47a2); E = A;
  CYC(0x47a2, 0x47a4); D = 0x00;
  CYC(0x47a4, 0x47a5); alu_add_hl(gb, DE);
  CYC(0x47a5, 0x47a6); A = mem_rd(gb, HL);
  RET(0x47a6);
}

static void getChannelVolume_affectedByMusicVolume(GB *gb) {
  CYC(0x478c, 0x478f); A = W8(wMusicVolume);
  CYC(0x478f, 0x4791); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(0x4791, 0x4793); goto muted; } // jr z
  CYC(0x4791, 0x4793);
  CYC(0x4793, 0x4795); alu_cp(gb, 0x01);
  if (F & FZ) { CYCT(0x4795, 0x4797); goto quarterVolume; } // jr z
  CYC(0x4795, 0x4797);
  CYC(0x4797, 0x4799); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(0x4799, 0x479b); goto halfVolume; } // jr z
  CYC(0x4799, 0x479b);
  getChannelVolume_fullVolume(gb);
  return;
halfVolume:
  CYC(0x47a7, 0x47aa); SET_HL(wChannelVolumes);
  CYC(0x47aa, 0x47ad); A = W8(wSoundChannel);
  CYC(0x47ad, 0x47ae); E = A;
  CYC(0x47ae, 0x47b0); D = 0x00;
  CYC(0x47b0, 0x47b1); alu_add_hl(gb, DE);
  CYC(0x47b1, 0x47b2); A = mem_rd(gb, HL);
  CYC(0x47b2, 0x47b4); A = alu_srl(gb, A);
  RET(0x47b4);
  return;
quarterVolume:
  CYC(0x47b5, 0x47b8); SET_HL(wChannelVolumes);
  CYC(0x47b8, 0x47bb); A = W8(wSoundChannel);
  CYC(0x47bb, 0x47bc); E = A;
  CYC(0x47bc, 0x47be); D = 0x00;
  CYC(0x47be, 0x47bf); alu_add_hl(gb, DE);
  CYC(0x47bf, 0x47c0); A = mem_rd(gb, HL);
  CYC(0x47c0, 0x47c2); A = alu_srl(gb, A);
  CYC(0x47c2, 0x47c4); A = alu_srl(gb, A);
  RET(0x47c4);
  return;
muted:
  CYC(0x47c5, 0x47c7); A = 0x00;
  RET(0x47c7);
}

// Intended for use with square and noise channels, but not used by channel 7.
// Channel 6 uses @affectedByMusicVolume as entry point.
void getChannelVolume_hook(GB *gb) {
  CYC(0x4783, 0x4786); A = W8(wSoundChannel);
  CYC(0x4786, 0x4787); alu_scf(gb);
  CYC(0x4787, 0x4788); alu_ccf(gb);
  CYC(0x4788, 0x478a); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(0x478a, 0x478c); getChannelVolume_fullVolume(gb); return; } // jr nc
  CYC(0x478a, 0x478c);
  getChannelVolume_affectedByMusicVolume(gb);
}

void standardCmdChannels4To5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x47c8, 0x47cb); SET_HL(wChannelFrequencyModeAndLengthTimerEnabled);
  CYC(0x47cb, 0x47ce); A = W8(wSoundChannel);
  CYC(0x47ce, 0x47cf); E = A;
  CYC(0x47cf, 0x47d1); D = 0x00;
  CYC(0x47d1, 0x47d2); alu_add_hl(gb, DE);
  CYC(0x47d2, 0x47d3); A = mem_rd(gb, HL);
  CYC(0x47d3, 0x47d5); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(0x47d5, 0x47d7); goto freqOrCmd; } // jr z
  CYC(0x47d5, 0x47d7);
  CALL_C(0x47d7, getNextChannelByte_hook, 0x4366, 0x47da);
  CYC(0x47da, 0x47db); L = A;
  CYC(0x47db, 0x47de); A = W8(wSoundCmd);
  CYC(0x47de, 0x47df); H = A;
  CYC(0x47df, 0x47e2); goto arbitraryFrequency; // jp
freqOrCmd:
  CYC(0x47e2, 0x47e5); A = W8(wSoundCmd);
  CYC(0x47e5, 0x47e6); alu_scf(gb);
  CYC(0x47e6, 0x47e7); alu_ccf(gb);
  CYC(0x47e7, 0x47e9); alu_cp(gb, 0x60);
  if (!(F & FZ)) { CYCT(0x47e9, 0x47eb); goto freqCommand; } // jr nz
  CYC(0x47e9, 0x47eb);
  CYC(0x47eb, 0x47ed); A = 0x01;
  CYC(0x47ed, 0x47f0); SET_HL(wChannelIsPlayingRest);
  CYC(0x47f0, 0x47f1); push_effect(gb, AF);
  CYC(0x47f1, 0x47f4); A = W8(wSoundChannel);
  CYC(0x47f4, 0x47f5); E = A;
  CYC(0x47f5, 0x47f7); D = 0x00;
  CYC(0x47f7, 0x47f8); alu_add_hl(gb, DE);
  CYC(0x47f8, 0x47f9); SET_AF(pop_effect(gb));
  CYC(0x47f9, 0x47fa); mem_wr(gb, HL, A);
  CALL_C(0x47fa, getWaveChannelVolume_hook, 0x489e, 0x47fd);
  CYC(0x47fd, 0x4800); SET_HL(wWaveChannelVolume);
  CYC(0x4800, 0x4801); push_effect(gb, AF);
  CYC(0x4801, 0x4804); A = W8(wSoundChannel);
  CYC(0x4804, 0x4805); E = A;
  CYC(0x4805, 0x4807); D = 0x00;
  CYC(0x4807, 0x4808); alu_add_hl(gb, DE);
  CYC(0x4808, 0x4809); SET_AF(pop_effect(gb));
  CYC(0x4809, 0x480a); mem_wr(gb, HL, A);
  CALL_C(0x480a, isWaveChannelUnavailable_hook, 0x434b, 0x480d);
  CYC(0x480d, 0x480f); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(0x480f, 0x4811); goto waitCmd60; } // jr nz
  CYC(0x480f, 0x4811);
  CYC(0x4811, 0x4814); SET_HL(wWaveChannelVolume);
  CYC(0x4814, 0x4817); A = W8(wSoundChannel);
  CYC(0x4817, 0x4818); E = A;
  CYC(0x4818, 0x481a); D = 0x00;
  CYC(0x481a, 0x481b); alu_add_hl(gb, DE);
  CYC(0x481b, 0x481c); A = mem_rd(gb, HL);
  CYC(0x481c, 0x481e); mem_wr(gb, 0xff1c, A);
waitCmd60:
  CYC(0x481e, 0x4821); setChannelWaitCounter_hook(gb); return; // jp
freqCommand:
  CYC(0x4821, 0x4823); A = 0x00;
  CYC(0x4823, 0x4826); SET_HL(wChannelIsPlayingRest);
  CYC(0x4826, 0x4827); push_effect(gb, AF);
  CYC(0x4827, 0x482a); A = W8(wSoundChannel);
  CYC(0x482a, 0x482b); E = A;
  CYC(0x482b, 0x482d); D = 0x00;
  CYC(0x482d, 0x482e); alu_add_hl(gb, DE);
  CYC(0x482e, 0x482f); SET_AF(pop_effect(gb));
  CYC(0x482f, 0x4830); mem_wr(gb, HL, A);
  CYC(0x4830, 0x4833); A = W8(wSoundCmd);
  CYC(0x4833, 0x4836); SET_HL(0x4a22);
  CALL_C(0x4836, readWordFromTable_hook, 0x4d19, 0x4839);
arbitraryFrequency:
  CALL_C(0x4839, setSoundFrequency_hook, 0x461a, 0x483c);
  CYC(0x483c, 0x483e); A = 0x00;
  CYC(0x483e, 0x4841); SET_HL(wChannelVibratoActive);
  CYC(0x4841, 0x4842); push_effect(gb, AF);
  CYC(0x4842, 0x4845); A = W8(wSoundChannel);
  CYC(0x4845, 0x4846); E = A;
  CYC(0x4846, 0x4848); D = 0x00;
  CYC(0x4848, 0x4849); alu_add_hl(gb, DE);
  CYC(0x4849, 0x484a); SET_AF(pop_effect(gb));
  CYC(0x484a, 0x484b); mem_wr(gb, HL, A);
  CYC(0x484b, 0x484d); A = 0x00;
  CYC(0x484d, 0x4850); SET_HL(wChannelVibratos);
  CYC(0x4850, 0x4853); A = W8(wSoundChannel);
  CYC(0x4853, 0x4854); E = A;
  CYC(0x4854, 0x4856); D = 0x00;
  CYC(0x4856, 0x4857); alu_add_hl(gb, DE);
  CYC(0x4857, 0x4858); A = mem_rd(gb, HL);
  CYC(0x4858, 0x485a); alu_and(gb, 0xf0);
  CYC(0x485a, 0x485c); A = alu_srl(gb, A);
  CYC(0x485c, 0x485e); A = alu_srl(gb, A);
  CYC(0x485e, 0x4860); A = alu_srl(gb, A);
  CYC(0x4860, 0x4863); SET_HL(wChannelVibratoCounters);
  CYC(0x4863, 0x4864); push_effect(gb, AF);
  CYC(0x4864, 0x4867); A = W8(wSoundChannel);
  CYC(0x4867, 0x4868); E = A;
  CYC(0x4868, 0x486a); D = 0x00;
  CYC(0x486a, 0x486b); alu_add_hl(gb, DE);
  CYC(0x486b, 0x486c); SET_AF(pop_effect(gb));
  CYC(0x486c, 0x486d); mem_wr(gb, HL, A);
  CALL_C(0x486d, getWaveChannelVolume_hook, 0x489e, 0x4870);
  CYC(0x4870, 0x4873); SET_HL(wWaveChannelVolume);
  CYC(0x4873, 0x4874); push_effect(gb, AF);
  CYC(0x4874, 0x4877); A = W8(wSoundChannel);
  CYC(0x4877, 0x4878); E = A;
  CYC(0x4878, 0x487a); D = 0x00;
  CYC(0x487a, 0x487b); alu_add_hl(gb, DE);
  CYC(0x487b, 0x487c); SET_AF(pop_effect(gb));
  CYC(0x487c, 0x487d); mem_wr(gb, HL, A);
  CALL_C(0x487d, isWaveChannelUnavailable_hook, 0x434b, 0x4880);
  CYC(0x4880, 0x4882); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(0x4882, 0x4884); goto waitFreq; } // jr nz
  CYC(0x4882, 0x4884);
  CYC(0x4884, 0x4887); SET_HL(wWaveChannelVolume);
  CYC(0x4887, 0x488a); A = W8(wSoundChannel);
  CYC(0x488a, 0x488b); E = A;
  CYC(0x488b, 0x488d); D = 0x00;
  CYC(0x488d, 0x488e); alu_add_hl(gb, DE);
  CYC(0x488e, 0x488f); A = mem_rd(gb, HL);
  CYC(0x488f, 0x4891); mem_wr(gb, 0xff1c, A);
  CYC(0x4891, 0x4894); A = W8(wSoundFrequencyL);
  CYC(0x4894, 0x4896); mem_wr(gb, 0xff1d, A);
  CYC(0x4896, 0x4899); A = W8(wSoundFrequencyH);
  CYC(0x4899, 0x489b); mem_wr(gb, 0xff1e, A);
waitFreq:
  CYC(0x489b, 0x489e); setChannelWaitCounter_hook(gb); return; // jp
}

// @param[out] a Volume of channel wSoundChannel dependent of wMusicVolume,
// in a form that can be written to NR32.
void getWaveChannelVolume_hook(GB *gb) {
  CYC(0x489e, 0x48a1); SET_HL(wChannelIsPlayingRest);
  CYC(0x48a1, 0x48a4); A = W8(wSoundChannel);
  CYC(0x48a4, 0x48a5); E = A;
  CYC(0x48a5, 0x48a7); D = 0x00;
  CYC(0x48a7, 0x48a8); alu_add_hl(gb, DE);
  CYC(0x48a8, 0x48a9); A = mem_rd(gb, HL);
  CYC(0x48a9, 0x48ab); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(0x48ab, 0x48ad); goto mute; } // jr nz
  CYC(0x48ab, 0x48ad);
  CYC(0x48ad, 0x48b0); A = W8(wSoundChannel);
  CYC(0x48b0, 0x48b2); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(0x48b2, 0x48b4); goto fullVolume; } // jr nc
  CYC(0x48b2, 0x48b4);
  CYC(0x48b4, 0x48b7); A = W8(wMusicVolume);
  CYC(0x48b7, 0x48b9); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(0x48b9, 0x48bb); goto mute; } // jr z
  CYC(0x48b9, 0x48bb);
  CYC(0x48bb, 0x48bd); alu_cp(gb, 0x01);
  if (F & FZ) { CYCT(0x48bd, 0x48bf); goto quarterVolume; } // jr z
  CYC(0x48bd, 0x48bf);
  CYC(0x48bf, 0x48c1); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(0x48c1, 0x48c3); goto halfVolume; } // jr z
  CYC(0x48c1, 0x48c3);
fullVolume:
  CYC(0x48c3, 0x48c5); A = 0x20;
  RET(0x48c5);
  return;
halfVolume:
  CYC(0x48c6, 0x48c8); A = 0x40;
  RET(0x48c8);
  return;
quarterVolume:
  CYC(0x48c9, 0x48cb); A = 0x60;
  RET(0x48cb);
  return;
mute:
  CYC(0x48cc, 0x48ce); A = 0x00;
  RET(0x48ce);
}

void standardCmdChannel6_hook(GB *gb) {
  CYC(0x48cf, 0x48d2); A = W8(wSoundCmd);
  CYC(0x48d2, 0x48d3); C = A;
  CYC(0x48d3, 0x48d6); SET_DE(0x4d38);
loop:
  CYC(0x48d6, 0x48d7); A = mem_rd(gb, DE);
  CYC(0x48d7, 0x48d8); SET_DE(DE + 1);
  CYC(0x48d8, 0x48da); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(0x48da, 0x48dc); goto wait; } // jr z
  CYC(0x48da, 0x48dc);
  CYC(0x48dc, 0x48dd); alu_cp(gb, C);
  if (F & FZ) { CYCT(0x48dd, 0x48df); goto found; } // jr z
  CYC(0x48dd, 0x48df);
  CYC(0x48df, 0x48e0); SET_DE(DE + 1);
  CYC(0x48e0, 0x48e1); SET_DE(DE + 1);
  CYC(0x48e1, 0x48e3); goto loop; // jr
found:
  CYC(0x48e3, 0x48e4); A = mem_rd(gb, DE);
  CYC(0x48e4, 0x48e5); L = A;
  CYC(0x48e5, 0x48e6); SET_DE(DE + 1);
  CYC(0x48e6, 0x48e7); A = mem_rd(gb, DE);
  CYC(0x48e7, 0x48e8); H = A;
  CYC(0x48e8, 0x48eb); A = mem_rd(gb, wChannelsEnabled + 7);
  CYC(0x48eb, 0x48ed); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(0x48ed, 0x48ef); goto wait; } // jr nz
  CYC(0x48ed, 0x48ef);
  CYC(0x48ef, 0x48f0); push_effect(gb, HL);
  CYC(0x48f0, 0x48f3);
  push_effect(gb, 0x48f3);
  getChannelVolume_affectedByMusicVolume(gb);
  CYC(0x48f3, 0x48f4); SET_HL(pop_effect(gb));
  CYC(0x48f4, 0x48f6); A = alu_sla(gb, A);
  CYC(0x48f6, 0x48f8); A = alu_sla(gb, A);
  CYC(0x48f8, 0x48fa); A = alu_sla(gb, A);
  CYC(0x48fa, 0x48fc); A = alu_sla(gb, A);
  CYC(0x48fc, 0x48fd); alu_or(gb, L);
  CYC(0x48fd, 0x48ff); mem_wr(gb, 0xff21, A);
  CYC(0x48ff, 0x4900); A = H;
  CYC(0x4900, 0x4902); mem_wr(gb, 0xff22, A);
  CYC(0x4902, 0x4904); A = 0x80;
  CYC(0x4904, 0x4906); mem_wr(gb, 0xff23, A);
wait:
  CYC(0x4906, 0x4909); setChannelWaitCounter_hook(gb); return; // jp
}

void standardCmdChannel7_hook(GB *gb) {
  CYC(0x4909, 0x490c); A = W8(wSoundCmd);
  CYC(0x490c, 0x490e); mem_wr(gb, 0xff22, A);
  CYC(0x490e, 0x4910); A = 0x00;
  CYC(0x4910, 0x4912); mem_wr(gb, 0xff20, A);
  CYC(0x4912, 0x4915); A = W8(wChannel7TriggerOnNextSound);
  CYC(0x4915, 0x4917); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(0x4917, 0x4919); goto skipTrigger; } // jr z
  CYC(0x4917, 0x4919);
  CYC(0x4919, 0x491b); mem_wr(gb, 0xff23, A);
skipTrigger:
  CYC(0x491b, 0x491d); A = 0x00;
  CYC(0x491d, 0x4920); W8(wChannel7TriggerOnNextSound) = A;
  CYC(0x4920, 0x4923); setChannelWaitCounter_hook(gb); return; // jp
}

// Disables and silences the current channel
void channelCmdff_hook(GB *gb) {
  CYC(0x4923, 0x4925); A = 0x00;
  CYC(0x4925, 0x4928); SET_HL(wChannelsEnabled);
  CYC(0x4928, 0x4929); push_effect(gb, AF);
  CYC(0x4929, 0x492c); A = W8(wSoundChannel);
  CYC(0x492c, 0x492d); E = A;
  CYC(0x492d, 0x492f); D = 0x00;
  CYC(0x492f, 0x4930); alu_add_hl(gb, DE);
  CYC(0x4930, 0x4931); SET_AF(pop_effect(gb));
  CYC(0x4931, 0x4932); mem_wr(gb, HL, A);
  silencePlayedSound_hook(gb);
}

// Ensures no sound is audible on the current channel by setting the volume
// to $0 or turning off the wave channel DAC.
void silencePlayedSound_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4932, 0x4935); A = W8(wSoundChannel);
  CYC(0x4935, 0x4938); SET_HL(0x493c);
  CALL_C(0x4938, readWordFromTable_hook, 0x4d19, 0x493b);
  CYC(0x493b, 0x493c);
  switch (HL) {
    case 0x494c: goto musicSquareChannel;
    case 0x495e: goto sfxSquareChannel;
    case 0x498a: goto musicWaveChannel;
    case 0x4996: goto sfxWaveChannel;
    case 0x49b8: goto noiseChannel;
    default: HANDOFF(HL);
  }
musicSquareChannel:
  // Only update if the corresponding sfx channel is not enabled
  CYC(0x494c, 0x494f); A = W8(wSoundChannel);
  CYC(0x494f, 0x4950); A = alu_inc8(gb, A);
  CYC(0x4950, 0x4951); A = alu_inc8(gb, A);
  CYC(0x4951, 0x4952); E = A;
  CYC(0x4952, 0x4955); SET_HL(wChannelsEnabled);
  CYC(0x4955, 0x4957); D = 0x00;
  CYC(0x4957, 0x4958); alu_add_hl(gb, DE);
  CYC(0x4958, 0x4959); A = mem_rd(gb, HL);
  CYC(0x4959, 0x495b); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(0x495b, 0x495d); goto silenceSharedTail; } // jr z
  CYC(0x495b, 0x495d);
  RET(0x495d);
  return;
sfxSquareChannel:
  CYC(0x495e, 0x4961); A = W8(wSoundChannel);
  CYC(0x4961, 0x4962); A = alu_dec8(gb, A);
  CYC(0x4962, 0x4963); A = alu_dec8(gb, A);
  CYC(0x4963, 0x4964); E = A;
  CYC(0x4964, 0x4967); SET_HL(wChannelsEnabled);
  CYC(0x4967, 0x4969); D = 0x00;
  CYC(0x4969, 0x496a); alu_add_hl(gb, DE);
  CYC(0x496a, 0x496b); A = mem_rd(gb, HL);
  CYC(0x496b, 0x496d); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(0x496d, 0x496f); } else { CYC(0x496d, 0x496f); } // jr z
  goto silenceSharedTail;
silenceSharedTail:
  // If an envelope is active that decreases volume over time, allow the
  // sound to keep playing.
  CYC(0x496f, 0x4972); SET_HL(wChannelEnvelopeStates);
  CYC(0x4972, 0x4975); A = W8(wSoundChannel);
  CYC(0x4975, 0x4976); E = A;
  CYC(0x4976, 0x4978); D = 0x00;
  CYC(0x4978, 0x4979); alu_add_hl(gb, DE);
  CYC(0x4979, 0x497a); A = mem_rd(gb, HL);
  CYC(0x497a, 0x497c); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x497c, 0x497e); goto silenceAndUpdate; } // jr nz
  CYC(0x497c, 0x497e);
  RET(0x497e);
  return;
silenceAndUpdate:
  CYC(0x497f, 0x4981); A = 0x08;
  CYC(0x4981, 0x4984); W8(wSoundCmdEnvelope) = A;
  CALL_C(0x4984, updateSquareChannelVolume_hook, 0x4714, 0x4987);
  CYC(0x4987, 0x498a); updatePlayedFrequency_hook(gb); return; // jp
musicWaveChannel:
  CALL_C(0x498a, isWaveChannelUnavailable_hook, 0x434b, 0x498d);
  CYC(0x498d, 0x498f); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(0x498f, 0x4991); goto musicWaveChannelRet; } // jr nz
  CYC(0x498f, 0x4991);
  CYC(0x4991, 0x4993); A = 0x00;
  CYC(0x4993, 0x4995); mem_wr(gb, 0xff1a, A);
musicWaveChannelRet:
  RET(0x4995);
  return;
sfxWaveChannel:
  CYC(0x4996, 0x4999); A = mem_rd(gb, wChannelsEnabled + 4);
  CYC(0x4999, 0x499b); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(0x499b, 0x499d); goto sfxWaveChannelDisable; } // jr z
  CYC(0x499b, 0x499d);
  CYC(0x499d, 0x499f); A = 0x04;
  CYC(0x499f, 0x49a0); E = A;
  CYC(0x49a0, 0x49a3); SET_HL(wChannelDutyCycles);
  CYC(0x49a3, 0x49a5); D = 0x00;
  CYC(0x49a5, 0x49a6); alu_add_hl(gb, DE);
  CYC(0x49a6, 0x49a7); A = mem_rd(gb, HL);
  CYC(0x49a7, 0x49aa); W8(wWaveformIndex) = A;
  CALL_C(0x49aa, setWaveform_hook, 0x49c1, 0x49ad);
  CYC(0x49ad, 0x49b0); A = mem_rd(gb, wWaveChannelVolume + 4);
  CYC(0x49b0, 0x49b2); mem_wr(gb, 0xff1c, A);
  RET(0x49b2);
  return;
sfxWaveChannelDisable:
  CYC(0x49b3, 0x49b5); A = 0x00;
  CYC(0x49b5, 0x49b7); mem_wr(gb, 0xff1a, A);
  RET(0x49b7);
  return;
noiseChannel:
  CYC(0x49b8, 0x49ba); A = 0x08;
  CYC(0x49ba, 0x49bc); mem_wr(gb, 0xff21, A);
  CYC(0x49bc, 0x49be); A = 0x80;
  CYC(0x49be, 0x49c0); mem_wr(gb, 0xff23, A);
  RET(0x49c0);
}

void setWaveform_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x49c1, isWaveChannelUnavailable_hook, 0x434b, 0x49c4);
  CYC(0x49c4, 0x49c6); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(0x49c6, 0x49c8); goto waitLoop; } // jr z
  CYC(0x49c6, 0x49c8);
  RET(0x49c8);
  return;
waitLoop:
  CYC(0x49c9, 0x49cb); A = 0x00;
  CYC(0x49cb, 0x49cd); mem_wr(gb, 0xff1a, A);
  CYC(0x49cd, 0x49cf); A = mem_rd(gb, 0xff26);
  CYC(0x49cf, 0x49d1); alu_and(gb, 0x04);
  if (!(F & FZ)) { CYCT(0x49d1, 0x49d3); goto waitLoop; } // jr nz
  CYC(0x49d1, 0x49d3);
  CYC(0x49d3, 0x49d6); A = W8(wWaveformIndex);
  CYC(0x49d6, 0x49d9); SET_HL(0x4d60);
  CALL_C(0x49d9, readWordFromTable_hook, 0x4d19, 0x49dc);
  CYC(0x49dc, 0x49de); C = 0x10;
  CYC(0x49de, 0x49e1); SET_DE(0xff30);
copyLoop:
  CYC(0x49e1, 0x49e2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x49e2, 0x49e3); mem_wr(gb, DE, A);
  CYC(0x49e3, 0x49e4); SET_DE(DE + 1);
  CYC(0x49e4, 0x49e5); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(0x49e5, 0x49e7); goto copyLoop; } // jr nz
  CYC(0x49e5, 0x49e7);
triggerWait:
  CYC(0x49e7, 0x49e9); A = 0x80;
  CYC(0x49e9, 0x49eb); mem_wr(gb, 0xff1a, A);
  CYC(0x49eb, 0x49ed); A = mem_rd(gb, 0xff1a);
  CYC(0x49ed, 0x49ef); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(0x49ef, 0x49f1); goto triggerWait; } // jr z
  CYC(0x49ef, 0x49f1);
  CYC(0x49f1, 0x49f3); A = 0x80;
  CYC(0x49f3, 0x49f5); mem_wr(gb, 0xff1e, A);
  RET(0x49f5);
}

// Jump to word from argument
void channelCmdfe_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x49f6, getNextChannelByte_hook, 0x4366, 0x49f9);
  CYC(0x49f9, 0x49fa); L = A;
  CALL_C(0x49fa, getNextChannelByte_hook, 0x4366, 0x49fd);
  CYC(0x49fd, 0x49fe); H = A;
  CYC(0x49fe, 0x4a01); A = W8(wSoundChannel);
  CYC(0x4a01, 0x4a03); A = alu_sla(gb, A);
  CYC(0x4a03, 0x4a04); B = A;
  CYC(0x4a04, 0x4a05); A = L;
  CYC(0x4a05, 0x4a07); C = 0xe2;
  CALL_C(0x4a07, writeIndexedHighRamAndIncrement_hook, 0x4d25, 0x4a0a);
  CYC(0x4a0a, 0x4a0b); A = H;
  CYC(0x4a0b, 0x4a0c); mem_wr(gb, 0xff00 | C, A);
  CYC(0x4a0c, 0x4a0d); C = alu_inc8(gb, C);
  CYC(0x4a0d, 0x4a10); doNextChannelCommand_hook(gb); return; // jp
}

void multiplyHlByA_hook(GB *gb) {
  CYC(0x4a10, 0x4a12); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(0x4a12, 0x4a14); goto nonzero; } // jr nz
  CYC(0x4a12, 0x4a14);
  CYC(0x4a14, 0x4a17); SET_HL(0x0000);
  RET(0x4a17);
  return;
nonzero:
  CYC(0x4a18, 0x4a19); E = L;
  CYC(0x4a19, 0x4a1a); D = H;
loop:
  CYC(0x4a1a, 0x4a1b); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x4a1b, 0x4a1d); goto done; } // jr z
  CYC(0x4a1b, 0x4a1d);
  CYC(0x4a1d, 0x4a1e); alu_add_hl(gb, DE);
  CYC(0x4a1e, 0x4a21); goto loop; // jp
done:
  RET(0x4a21);
}

// @param a The sound to play.
void playSound_b39_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4b50, 0x4b51); push_effect(gb, BC);
  CYC(0x4b51, 0x4b52); push_effect(gb, DE);
  CYC(0x4b52, 0x4b53); push_effect(gb, HL);
  CYC(0x4b53, 0x4b56); W8(wSoundTmp) = A;
  CYC(0x4b56, 0x4b58); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(0x4b58, 0x4b5a); goto dispatch; } // jr nz
  CYC(0x4b58, 0x4b5a);
  CYC(0x4b5a, 0x4b5d); goto playSoundEnd; // jp
dispatch:
  CYC(0x4b5d, 0x4b5f); alu_cp(gb, 0xf0);
  if (F & FZ) { CYCT(0x4b5f, 0x4b61); goto sndf0; } // jr z
  CYC(0x4b5f, 0x4b61);
  CYC(0x4b61, 0x4b63); alu_cp(gb, 0xf1);
  if (F & FZ) { CYCT(0x4b63, 0x4b65); goto sndf1; } // jr z
  CYC(0x4b63, 0x4b65);
  CYC(0x4b65, 0x4b67); alu_cp(gb, 0xf5);
  if (F & FZ) { CYCT(0x4b67, 0x4b69); goto sndf5; } // jr z
  CYC(0x4b67, 0x4b69);
  CYC(0x4b69, 0x4b6b); alu_cp(gb, 0xf6);
  if (F & FZ) { CYCT(0x4b6b, 0x4b6d); goto sndf6; } // jr z
  CYC(0x4b6b, 0x4b6d);
  CYC(0x4b6d, 0x4b6f); alu_cp(gb, 0xf7);
  if (F & FZ) { CYCT(0x4b6f, 0x4b71); goto sndf7; } // jr z
  CYC(0x4b6f, 0x4b71);
  CYC(0x4b71, 0x4b73); alu_cp(gb, 0xf8);
  if (F & FZ) { CYCT(0x4b73, 0x4b75); goto sndf8; } // jr z
  CYC(0x4b73, 0x4b75);
  CYC(0x4b75, 0x4b77); alu_cp(gb, 0xf9);
  if (F & FZ) { CYCT(0x4b77, 0x4b79); goto sndf9; } // jr z
  CYC(0x4b77, 0x4b79);
  CYC(0x4b79, 0x4b7b); alu_cp(gb, 0xfa);
  if (F & FZ) { CYCT(0x4b7b, 0x4b7d); goto sndfa; } // jr z
  CYC(0x4b7b, 0x4b7d);
  CYC(0x4b7d, 0x4b7f); alu_cp(gb, 0xfb);
  if (F & FZ) { CYCT(0x4b7f, 0x4b81); goto sndfb; } // jr z
  CYC(0x4b7f, 0x4b81);
  CYC(0x4b81, 0x4b83); alu_cp(gb, 0xfc);
  if (F & FZ) { CYCT(0x4b83, 0x4b85); goto sndfc; } // jr z
  CYC(0x4b83, 0x4b85);
  CYC(0x4b85, 0x4b87); goto normalSound; // jr
sndf0:
  CYC(0x4b87, 0x4b89); A = 0xde; // stop music
  CYC(0x4b89, 0x4b8c); W8(wSoundTmp) = A;
  CYC(0x4b8c, 0x4b8e); goto normalSound; // jr
sndf1:
  CALL_C(0x4b8e, stopSfx_hook, 0x40ca, 0x4b91);
  CYC(0x4b91, 0x4b94); goto playSoundEnd; // jp
sndf5:
  CALL_C(0x4b94, silenceAllChannels_hook, 0x40b9, 0x4b97);
  CYC(0x4b97, 0x4b99); A = 0x01;
  CYC(0x4b99, 0x4b9c); W8(wSoundDisabled) = A;
  CYC(0x4b9c, 0x4b9f); goto setVolumeAndEnd; // jp
sndf6:
  CYC(0x4b9f, 0x4ba1); A = 0x00;
  CYC(0x4ba1, 0x4ba4); W8(wSoundDisabled) = A;
  CYC(0x4ba4, 0x4ba7); goto setVolumeAndEnd; // jp
sndfa:
  CYC(0x4ba7, 0x4ba9); A = 0x07;
  CYC(0x4ba9, 0x4bab); goto fadeSpeedSet; // jr
sndfb:
  CYC(0x4bab, 0x4bad); A = 0x0f;
  CYC(0x4bad, 0x4baf); goto fadeSpeedSet; // jr
sndfc:
  CYC(0x4baf, 0x4bb1); A = 0x1f;
fadeSpeedSet:
  CYC(0x4bb1, 0x4bb4); W8(wSoundFadeSpeed) = A;
  CYC(0x4bb4, 0x4bb6); A = 0x00;
  CYC(0x4bb6, 0x4bb9); W8(wSoundFadeCounter) = A;
  CYC(0x4bb9, 0x4bbb); A = 0x01;
  CYC(0x4bbb, 0x4bbe); W8(wSoundFadeDirection) = A;
  CYC(0x4bbe, 0x4bc0); A = 0x77;
  CYC(0x4bc0, 0x4bc3); W8(wSoundVolume) = A;
  CYC(0x4bc3, 0x4bc6); goto playSoundEnd; // jp
sndf7:
  CYC(0x4bc6, 0x4bc8); A = 0x03;
  CYC(0x4bc8, 0x4bca); goto fadeInSet; // jr
sndf8:
  CYC(0x4bca, 0x4bcc); A = 0x07;
  CYC(0x4bcc, 0x4bce); goto fadeInSet; // jr
sndf9:
  CYC(0x4bce, 0x4bd0); A = 0x0f;
fadeInSet:
  CYC(0x4bd0, 0x4bd3); W8(wSoundFadeSpeed) = A;
  CYC(0x4bd3, 0x4bd5); A = 0x00;
  CYC(0x4bd5, 0x4bd8); W8(wSoundFadeCounter) = A;
  CYC(0x4bd8, 0x4bda); A = 0x0a;
  CYC(0x4bda, 0x4bdd); W8(wSoundFadeDirection) = A;
  CYC(0x4bdd, 0x4bdf); A = 0x00;
  CYC(0x4bdf, 0x4be2); W8(wSoundVolume) = A;
  CYC(0x4be2, 0x4be5); goto playSoundEnd; // jp
normalSound:
  CYC(0x4be5, 0x4be7); A = 0x00;
  CYC(0x4be7, 0x4bea); W8(wSoundFadeDirection) = A;
  CYC(0x4bea, 0x4bed); A = W8(wSoundTmp);
  CYC(0x4bed, 0x4bef); D = 0x00;
  CYC(0x4bef, 0x4bf0); E = A;
  CYC(0x4bf0, 0x4bf2); H = 0x00;
  CYC(0x4bf2, 0x4bf3); L = A;
  CYC(0x4bf3, 0x4bf5); L = alu_sla(gb, L);
  CYC(0x4bf5, 0x4bf7); H = alu_rl(gb, H);
  CYC(0x4bf7, 0x4bf8); alu_add_hl(gb, DE);
  CYC(0x4bf8, 0x4bf9); D = H;
  CYC(0x4bf9, 0x4bfa); E = L;
  CYC(0x4bfa, 0x4bfd); SET_HL(0x5748);
  CYC(0x4bfd, 0x4bfe); alu_add_hl(gb, DE);
  CYC(0x4bfe, 0x4bff); A = mem_rd(gb, HL);
  CYC(0x4bff, 0x4c01); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(0x4c01, 0x4c03); goto skipWeirdCall; } // jr z
  CYC(0x4c01, 0x4c03);
  CALL_ROM(0x4c03, 0x4d38);
  CYC(0x4c06, 0x4c09); goto setVolumeAndEnd; // jp
skipWeirdCall:
  CYC(0x4c09, 0x4c0a); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4c0a, 0x4c0b); C = A;
  CYC(0x4c0b, 0x4c0d); A = H8(hSoundDataBaseBank);
  CYC(0x4c0d, 0x4c0e); alu_add(gb, C);
  CYC(0x4c0e, 0x4c11); W8(wLoadingSoundBank) = A;
  CYC(0x4c11, 0x4c12); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4c12, 0x4c13); C = A;
  CYC(0x4c13, 0x4c14); A = mem_rd(gb, HL);
  CYC(0x4c14, 0x4c15); B = A;
  CYC(0x4c15, 0x4c16); L = C;
  CYC(0x4c16, 0x4c17); H = B;
nextSoundChannel:
  CYC(0x4c17, 0x4c19); A = H8(hSoundDataBaseBank);
  CALL_C(0x4c19, wMusicReadFunction_hook, 0xc000, 0x4c1c);
  CYC(0x4c1c, 0x4c1e); alu_cp(gb, 0xff);
  if (!(F & FZ)) { CYCT(0x4c1e, 0x4c20); goto haveChannelByte; } // jr nz
  CYC(0x4c1e, 0x4c20);
  CYC(0x4c20, 0x4c23); goto setVolumeAndEnd; // jp
haveChannelByte:
  CYC(0x4c23, 0x4c26); W8(wSoundTmp) = A;
  CYC(0x4c26, 0x4c28); alu_and(gb, 0xf0);
  CYC(0x4c28, 0x4c2a); A = alu_swap(gb, A);
  CYC(0x4c2a, 0x4c2b); A = alu_inc8(gb, A);
  CYC(0x4c2b, 0x4c2e); W8(wSoundChannelValue) = A;
  CYC(0x4c2e, 0x4c31); A = W8(wSoundTmp);
  CYC(0x4c31, 0x4c33); alu_and(gb, 0x0f);
  CYC(0x4c33, 0x4c36); W8(wSoundTmp) = A;
  CYC(0x4c36, 0x4c37); E = A;
  CYC(0x4c37, 0x4c38); push_effect(gb, HL);
  CYC(0x4c38, 0x4c3b); SET_HL(wChannelsEnabled);
  CYC(0x4c3b, 0x4c3d); D = 0x00;
  CYC(0x4c3d, 0x4c3e); alu_add_hl(gb, DE);
  CYC(0x4c3e, 0x4c3f); A = mem_rd(gb, HL);
  CYC(0x4c3f, 0x4c40); SET_HL(pop_effect(gb));
  CYC(0x4c40, 0x4c41); C = A;
  CYC(0x4c41, 0x4c44); A = W8(wSoundChannelValue);
  CYC(0x4c44, 0x4c45); alu_cp(gb, C);
  if (!(F & FC)) { CYCT(0x4c45, 0x4c47); goto channelSlotFound; } // jr nc
  CYC(0x4c45, 0x4c47);
  CYC(0x4c47, 0x4c48); SET_HL(HL + 1);
  CYC(0x4c48, 0x4c49); SET_HL(HL + 1);
  CYC(0x4c49, 0x4c4c); goto nextSoundChannel; // jp
channelSlotFound:
  CYC(0x4c4c, 0x4c4d); push_effect(gb, HL);
  CYC(0x4c4d, 0x4c50); A = W8(wSoundTmp);
  CYC(0x4c50, 0x4c51); E = A;
  CYC(0x4c51, 0x4c54); A = W8(wSoundChannelValue);
  CYC(0x4c54, 0x4c57); SET_HL(wChannelsEnabled);
  CYC(0x4c57, 0x4c59); D = 0x00;
  CYC(0x4c59, 0x4c5a); alu_add_hl(gb, DE);
  CYC(0x4c5a, 0x4c5b); mem_wr(gb, HL, A);
  CYC(0x4c5b, 0x4c5d); A = 0x08;
  CYC(0x4c5d, 0x4c60); SET_HL(wChannelVolumes);
  CYC(0x4c60, 0x4c62); D = 0x00;
  CYC(0x4c62, 0x4c63); alu_add_hl(gb, DE);
  CYC(0x4c63, 0x4c64); mem_wr(gb, HL, A);
  CYC(0x4c64, 0x4c66); A = 0x00;
  CYC(0x4c66, 0x4c69); SET_HL(wChannelWaitCounters);
  CYC(0x4c69, 0x4c6b); D = 0x00;
  CYC(0x4c6b, 0x4c6c); alu_add_hl(gb, DE);
  CYC(0x4c6c, 0x4c6d); mem_wr(gb, HL, A);
  CYC(0x4c6d, 0x4c70); A = W8(wSoundTmp);
  CYC(0x4c70, 0x4c72); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(0x4c72, 0x4c74); goto squareChannel; } // jr z
  CYC(0x4c72, 0x4c74);
  CYC(0x4c74, 0x4c76); alu_cp(gb, 0x01);
  if (F & FZ) { CYCT(0x4c76, 0x4c78); goto squareChannel; } // jr z
  CYC(0x4c76, 0x4c78);
  CYC(0x4c78, 0x4c7a); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(0x4c7a, 0x4c7c); goto squareChannel; } // jr z
  CYC(0x4c7a, 0x4c7c);
  CYC(0x4c7c, 0x4c7e); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(0x4c7e, 0x4c80); goto squareChannel; } // jr z
  CYC(0x4c7e, 0x4c80);
  CYC(0x4c80, 0x4c82); alu_cp(gb, 0x04);
  if (F & FZ) { CYCT(0x4c82, 0x4c84); goto waveChannel; } // jr z
  CYC(0x4c82, 0x4c84);
  CYC(0x4c84, 0x4c86); alu_cp(gb, 0x05);
  if (F & FZ) { CYCT(0x4c86, 0x4c88); goto waveChannel; } // jr z
  CYC(0x4c86, 0x4c88);
  CYC(0x4c88, 0x4c8a); goto writeChannelPtrs; // jr
waveChannel:
  CYC(0x4c8a, 0x4c8d); A = W8(wSoundTmp);
  CYC(0x4c8d, 0x4c8e); E = A;
  CYC(0x4c8e, 0x4c90); A = 0x00;
  CYC(0x4c90, 0x4c93); SET_HL(wChannelVibratos);
  CYC(0x4c93, 0x4c95); D = 0x00;
  CYC(0x4c95, 0x4c96); alu_add_hl(gb, DE);
  CYC(0x4c96, 0x4c97); mem_wr(gb, HL, A);
  CYC(0x4c97, 0x4c9a); SET_HL(wChannelSweep);
  CYC(0x4c9a, 0x4c9c); D = 0x00;
  CYC(0x4c9c, 0x4c9d); alu_add_hl(gb, DE);
  CYC(0x4c9d, 0x4c9e); mem_wr(gb, HL, A);
  CYC(0x4c9e, 0x4ca1); SET_HL(wChannelPitchShift);
  CYC(0x4ca1, 0x4ca3); D = 0x00;
  CYC(0x4ca3, 0x4ca4); alu_add_hl(gb, DE);
  CYC(0x4ca4, 0x4ca5); mem_wr(gb, HL, A);
  CYC(0x4ca5, 0x4ca8); SET_HL(wChannelFrequencyModeAndLengthTimerEnabled);
  CYC(0x4ca8, 0x4caa); D = 0x00;
  CYC(0x4caa, 0x4cab); alu_add_hl(gb, DE);
  CYC(0x4cab, 0x4cac); mem_wr(gb, HL, A);
  CYC(0x4cac, 0x4cae); goto writeChannelPtrs; // jr
squareChannel:
  CYC(0x4cae, 0x4cb1); A = W8(wSoundTmp);
  CYC(0x4cb1, 0x4cb2); E = A;
  CYC(0x4cb2, 0x4cb4); A = 0x00;
  CYC(0x4cb4, 0x4cb7); SET_HL(wChannelEnvelopes);
  CYC(0x4cb7, 0x4cb9); D = 0x00;
  CYC(0x4cb9, 0x4cba); alu_add_hl(gb, DE);
  CYC(0x4cba, 0x4cbb); mem_wr(gb, HL, A);
  CYC(0x4cbb, 0x4cbe); SET_HL(wChannelEnvelopes2);
  CYC(0x4cbe, 0x4cc0); D = 0x00;
  CYC(0x4cc0, 0x4cc1); alu_add_hl(gb, DE);
  CYC(0x4cc1, 0x4cc2); mem_wr(gb, HL, A);
  CYC(0x4cc2, 0x4cc5); SET_HL(wChannelDutyCycles);
  CYC(0x4cc5, 0x4cc7); D = 0x00;
  CYC(0x4cc7, 0x4cc8); alu_add_hl(gb, DE);
  CYC(0x4cc8, 0x4cc9); mem_wr(gb, HL, A);
  CYC(0x4cc9, 0x4ccc); SET_HL(wChannelVibratos);
  CYC(0x4ccc, 0x4cce); D = 0x00;
  CYC(0x4cce, 0x4ccf); alu_add_hl(gb, DE);
  CYC(0x4ccf, 0x4cd0); mem_wr(gb, HL, A);
  CYC(0x4cd0, 0x4cd3); SET_HL(wChannelSweep);
  CYC(0x4cd3, 0x4cd5); D = 0x00;
  CYC(0x4cd5, 0x4cd6); alu_add_hl(gb, DE);
  CYC(0x4cd6, 0x4cd7); mem_wr(gb, HL, A);
  CYC(0x4cd7, 0x4cda); SET_HL(wChannelPitchShift);
  CYC(0x4cda, 0x4cdc); D = 0x00;
  CYC(0x4cdc, 0x4cdd); alu_add_hl(gb, DE);
  CYC(0x4cdd, 0x4cde); mem_wr(gb, HL, A);
  CYC(0x4cde, 0x4ce1); SET_HL(wChannelFrequencyModeAndLengthTimerEnabled);
  CYC(0x4ce1, 0x4ce3); D = 0x00;
  CYC(0x4ce3, 0x4ce4); alu_add_hl(gb, DE);
  CYC(0x4ce4, 0x4ce5); mem_wr(gb, HL, A);
writeChannelPtrs:
  CYC(0x4ce5, 0x4ce6); SET_HL(pop_effect(gb));
  CYC(0x4ce6, 0x4ce9); A = W8(wSoundTmp);
  CYC(0x4ce9, 0x4cea); B = A;
  CYC(0x4cea, 0x4ced); A = W8(wLoadingSoundBank);
  CYC(0x4ced, 0x4cef); C = 0xda;
  CALL_C(0x4cef, writeIndexedHighRamAndIncrement_hook, 0x4d25, 0x4cf2);
  CYC(0x4cf2, 0x4cf5); A = W8(wSoundTmp);
  CYC(0x4cf5, 0x4cf7); A = alu_sla(gb, A);
  CYC(0x4cf7, 0x4cf8); B = A;
  CYC(0x4cf8, 0x4cf9); push_effect(gb, BC);
  CYC(0x4cf9, 0x4cfb); A = H8(hSoundDataBaseBank);
  CALL_C(0x4cfb, wMusicReadFunction_hook, 0xc000, 0x4cfe);
  CYC(0x4cfe, 0x4cff); SET_BC(pop_effect(gb));
  CYC(0x4cff, 0x4d01); C = 0xe2;
  CALL_C(0x4d01, writeIndexedHighRamAndIncrement_hook, 0x4d25, 0x4d04);
  CYC(0x4d04, 0x4d05); push_effect(gb, BC);
  CYC(0x4d05, 0x4d07); A = H8(hSoundDataBaseBank);
  CALL_C(0x4d07, wMusicReadFunction_hook, 0xc000, 0x4d0a);
  CYC(0x4d0a, 0x4d0b); SET_BC(pop_effect(gb));
  CYC(0x4d0b, 0x4d0c); mem_wr(gb, 0xff00 | C, A);
  CYC(0x4d0c, 0x4d0d); C = alu_inc8(gb, C);
  CYC(0x4d0d, 0x4d10); goto nextSoundChannel; // jp
setVolumeAndEnd:
  CYC(0x4d10, 0x4d12); A = 0x77;
  CYC(0x4d12, 0x4d15); W8(wSoundVolume) = A;
playSoundEnd:
  CYC(0x4d15, 0x4d16); SET_HL(pop_effect(gb));
  CYC(0x4d16, 0x4d17); SET_DE(pop_effect(gb));
  CYC(0x4d17, 0x4d18); SET_BC(pop_effect(gb));
  RET(0x4d18);
}

// Reads a word at hl+a*2 into de and hl. Index can't be higher than $7f.
void readWordFromTable_hook(GB *gb) {
  CYC(0x4d19, 0x4d1b); A = alu_sla(gb, A);
  CYC(0x4d1b, 0x4d1d); D = 0x00;
  CYC(0x4d1d, 0x4d1e); E = A;
  CYC(0x4d1e, 0x4d1f); alu_add_hl(gb, DE);
  CYC(0x4d1f, 0x4d20); E = mem_rd(gb, HL);
  CYC(0x4d20, 0x4d21); SET_HL(HL + 1);
  CYC(0x4d21, 0x4d22); D = mem_rd(gb, HL);
  CYC(0x4d22, 0x4d23); H = D;
  CYC(0x4d23, 0x4d24); L = E;
  RET(0x4d24);
}

// Adds b to c, writes a to ($ff00+c), increments c.
void writeIndexedHighRamAndIncrement_hook(GB *gb) {
  CYC(0x4d25, 0x4d26); push_effect(gb, AF);
  CYC(0x4d26, 0x4d27); A = B;
  CYC(0x4d27, 0x4d28); alu_add(gb, C);
  CYC(0x4d28, 0x4d29); C = A;
  CYC(0x4d29, 0x4d2a); SET_AF(pop_effect(gb));
  CYC(0x4d2a, 0x4d2b); mem_wr(gb, 0xff00 | C, A);
  CYC(0x4d2b, 0x4d2c); C = alu_inc8(gb, C);
  RET(0x4d2c);
}
