#ifndef APU_H
#define APU_H
#include <stdbool.h>
#include <stdint.h>

#define APU_SAMPLE_RATE 48000
#define APU_RING 16384

typedef struct {
  bool enabled, dac, length_enable, env_add, sweep_enabled, sweep_neg;
  int length, freq, timer, duty, duty_pos, volume, env_period, env_timer;
  int sweep_period, sweep_timer, sweep_shift, sweep_shadow;
} Square;

typedef struct {
  bool enabled, dac, length_enable;
  int length, freq, timer, pos, volume_code;
  uint8_t sample;
} Wave;

typedef struct {
  bool enabled, dac, length_enable, env_add, width7;
  int length, timer, volume, env_period, env_timer, clock_shift, divisor_code;
  uint16_t lfsr;
} Noise;

typedef struct Apu {
  uint8_t regs[0x30];
  bool power;
  Square sq[2];
  Wave wave;
  Noise noise;
  int fs_counter, fs_step;
  uint32_t sample_acc;
  int16_t ring[APU_RING * 2];
  uint32_t ring_w, ring_r;
} Apu;

void apu_reset(Apu *apu);
void apu_tick(Apu *apu, int dots);
uint8_t apu_read(Apu *apu, uint8_t r);
void apu_write(Apu *apu, uint8_t r, uint8_t v);
uint32_t apu_samples_available(const Apu *apu);
uint32_t apu_read_samples(Apu *apu, int16_t *out, uint32_t max_frames);
#endif
