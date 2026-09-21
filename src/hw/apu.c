#include "apu.h"
#include <string.h>

static const uint8_t duty_table[4] = {0x01, 0x81, 0x87, 0x7e};
static const int noise_divisor[8] = {8, 16, 32, 48, 64, 80, 96, 112};
static const uint8_t read_mask[0x20] = {
  0x80, 0x3f, 0x00, 0xff, 0xbf, 0xff, 0x3f, 0x00, 0xff, 0xbf, 0x7f, 0xff, 0x9f, 0xff, 0xbf, 0xff,
  0xff, 0x00, 0x00, 0xbf, 0x00, 0x00, 0x70, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};

void apu_reset(Apu *apu) {
  memset(apu, 0, sizeof *apu);
  apu->noise.lfsr = 0x7fff;
  apu->power = true;
  static const uint8_t init[] = {0x80, 0xbf, 0xf3, 0xff, 0xbf, 0xff, 0x3f, 0x00, 0xff, 0xbf, 0x7f,
                                 0xff, 0x9f, 0xff, 0xbf, 0xff, 0xff, 0x00, 0x00, 0xbf, 0x77, 0xf3, 0xf1};
  for (int i = 0; i < 0x17; i++) apu_write(apu, 0x10 + i, init[i]);
}

static void sq_reg(Square *s, const uint8_t *r, int base) {
  s->duty = r[base + 1] >> 6;
  s->freq = r[base + 3] | ((r[base + 4] & 7) << 8);
  s->length_enable = r[base + 4] & 0x40;
  s->dac = (r[base + 2] & 0xf8) != 0;
  if (!s->dac) s->enabled = false;
}

static int sweep_calc(Square *s) {
  int d = s->sweep_shadow >> s->sweep_shift;
  return s->sweep_neg ? s->sweep_shadow - d : s->sweep_shadow + d;
}

static void sq_trigger(Square *s, const uint8_t *r, int base, bool has_sweep) {
  s->enabled = s->dac;
  if (s->length == 0) s->length = 64;
  s->timer = (2048 - s->freq) * 4;
  s->env_period = r[base + 2] & 7;
  s->env_timer = s->env_period ? s->env_period : 8;
  s->env_add = r[base + 2] & 8;
  s->volume = r[base + 2] >> 4;
  if (has_sweep) {
    s->sweep_period = (r[base] >> 4) & 7;
    s->sweep_neg = r[base] & 8;
    s->sweep_shift = r[base] & 7;
    s->sweep_shadow = s->freq;
    s->sweep_timer = s->sweep_period ? s->sweep_period : 8;
    s->sweep_enabled = s->sweep_period || s->sweep_shift;
    if (s->sweep_shift && sweep_calc(s) > 2047) s->enabled = false;
  }
}

static void length_clock(bool *enabled, int *length, bool length_enable) {
  if (length_enable && *length > 0 && --*length == 0) *enabled = false;
}

static void env_clock(int *volume, int *timer, int period, bool add) {
  if (period == 0) return;
  if (--*timer == 0) {
    *timer = period;
    if (add && *volume < 15) (*volume)++;
    if (!add && *volume > 0) (*volume)--;
  }
}

static void sweep_clock(Apu *apu) {
  Square *s = &apu->sq[0];
  if (!s->sweep_enabled || s->sweep_period == 0) return;
  if (--s->sweep_timer == 0) {
    s->sweep_timer = s->sweep_period;
    int nf = sweep_calc(s);
    if (nf > 2047) { s->enabled = false; return; }
    if (s->sweep_shift) {
      s->freq = s->sweep_shadow = nf;
      apu->regs[0x03] = nf & 0xff;
      apu->regs[0x04] = (apu->regs[0x04] & 0xf8) | (nf >> 8);
      if (sweep_calc(s) > 2047) s->enabled = false;
    }
  }
}

static void frame_sequencer(Apu *apu) {
  int step = apu->fs_step;
  apu->fs_step = (step + 1) & 7;
  if ((step & 1) == 0) {
    for (int i = 0; i < 2; i++) length_clock(&apu->sq[i].enabled, &apu->sq[i].length, apu->sq[i].length_enable);
    length_clock(&apu->wave.enabled, &apu->wave.length, apu->wave.length_enable);
    length_clock(&apu->noise.enabled, &apu->noise.length, apu->noise.length_enable);
  }
  if (step == 2 || step == 6) sweep_clock(apu);
  if (step == 7) {
    for (int i = 0; i < 2; i++) env_clock(&apu->sq[i].volume, &apu->sq[i].env_timer, apu->sq[i].env_period, apu->sq[i].env_add);
    env_clock(&apu->noise.volume, &apu->noise.env_timer, apu->noise.env_period, apu->noise.env_add);
  }
}

static void emit_sample(Apu *apu) {
  int out[4];
  for (int i = 0; i < 2; i++) {
    Square *s = &apu->sq[i];
    out[i] = (s->enabled && s->dac && ((duty_table[s->duty] >> s->duty_pos) & 1)) ? s->volume : 0;
  }
  Wave *w = &apu->wave;
  if (w->enabled && w->dac && w->volume_code) out[2] = w->sample >> (w->volume_code - 1);
  else out[2] = 0;
  Noise *n = &apu->noise;
  out[3] = (n->enabled && n->dac && !(n->lfsr & 1)) ? n->volume : 0;

  uint8_t nr50 = apu->regs[0x14], nr51 = apu->regs[0x15];
  int left = 0, right = 0;
  for (int i = 0; i < 4; i++) {
    if (nr51 & (0x10 << i)) left += out[i];
    if (nr51 & (0x01 << i)) right += out[i];
  }
  left = left * (((nr50 >> 4) & 7) + 1) * 32;
  right = right * ((nr50 & 7) + 1) * 32;
  if (!apu->power) left = right = 0;
  if (apu->ring_w - apu->ring_r >= APU_RING) return;
  uint32_t i = (apu->ring_w % APU_RING) * 2;
  apu->ring[i] = (int16_t)left;
  apu->ring[i + 1] = (int16_t)right;
  apu->ring_w++;
}

void apu_tick(Apu *apu, int dots) {
  for (int d = 0; d < dots; d++) {
    if (apu->power) {
      for (int i = 0; i < 2; i++) {
        Square *s = &apu->sq[i];
        if (--s->timer <= 0) { s->timer = (2048 - s->freq) * 4; s->duty_pos = (s->duty_pos + 1) & 7; }
      }
      Wave *w = &apu->wave;
      if (--w->timer <= 0) {
        w->timer = (2048 - w->freq) * 2;
        w->pos = (w->pos + 1) & 31;
        uint8_t b = apu->regs[0x20 + (w->pos >> 1)];
        w->sample = (w->pos & 1) ? (b & 0x0f) : (b >> 4);
      }
      Noise *n = &apu->noise;
      if (--n->timer <= 0) {
        n->timer = noise_divisor[n->divisor_code] << n->clock_shift;
        uint16_t x = (n->lfsr ^ (n->lfsr >> 1)) & 1;
        n->lfsr = (n->lfsr >> 1) | (x << 14);
        if (n->width7) n->lfsr = (n->lfsr & ~(1 << 6)) | (x << 6);
      }
      if (++apu->fs_counter == 8192) { apu->fs_counter = 0; frame_sequencer(apu); }
    }
    apu->sample_acc += APU_SAMPLE_RATE;
    if (apu->sample_acc >= 4194304) { apu->sample_acc -= 4194304; emit_sample(apu); }
  }
}

uint8_t apu_read(Apu *apu, uint8_t r) {
  int i = r - 0x10;
  if (i >= 0x20) return apu->regs[i];
  if (i == 0x16) {
    return 0x70 | (apu->power ? 0x80 : 0) | (apu->sq[0].enabled ? 1 : 0) | (apu->sq[1].enabled ? 2 : 0) |
           (apu->wave.enabled ? 4 : 0) | (apu->noise.enabled ? 8 : 0);
  }
  return apu->regs[i] | read_mask[i];
}

void apu_write(Apu *apu, uint8_t r, uint8_t v) {
  int i = r - 0x10;
  if (i >= 0x20) { apu->regs[i] = v; return; }
  if (i == 0x16) {
    bool on = v & 0x80;
    if (!on && apu->power) {
      for (int j = 0; j < 0x16; j++) apu_write(apu, 0x10 + j, 0);
      apu->sq[0].enabled = apu->sq[1].enabled = apu->wave.enabled = apu->noise.enabled = false;
      apu->sq[0].length = apu->sq[1].length = apu->wave.length = apu->noise.length = 0;
    }
    if (on && !apu->power) { apu->fs_step = 0; apu->fs_counter = 0; }
    apu->power = on;
    return;
  }
  if (!apu->power) return;
  apu->regs[i] = v;
  uint8_t *R = apu->regs;
  switch (i) {
  case 0x00: apu->sq[0].sweep_neg = v & 8; return;
  case 0x01: apu->sq[0].length = 64 - (v & 63); sq_reg(&apu->sq[0], R, 0x00); return;
  case 0x02: sq_reg(&apu->sq[0], R, 0x00); return;
  case 0x03: sq_reg(&apu->sq[0], R, 0x00); return;
  case 0x04: sq_reg(&apu->sq[0], R, 0x00); if (v & 0x80) sq_trigger(&apu->sq[0], R, 0x00, true); return;
  case 0x06: apu->sq[1].length = 64 - (v & 63); sq_reg(&apu->sq[1], R, 0x05); return;
  case 0x07: sq_reg(&apu->sq[1], R, 0x05); return;
  case 0x08: sq_reg(&apu->sq[1], R, 0x05); return;
  case 0x09: sq_reg(&apu->sq[1], R, 0x05); if (v & 0x80) sq_trigger(&apu->sq[1], R, 0x05, false); return;
  case 0x0a: apu->wave.dac = v & 0x80; if (!apu->wave.dac) apu->wave.enabled = false; return;
  case 0x0b: apu->wave.length = 256 - v; return;
  case 0x0c: apu->wave.volume_code = (v >> 5) & 3; return;
  case 0x0d: apu->wave.freq = (apu->wave.freq & 0x700) | v; return;
  case 0x0e:
    apu->wave.freq = (apu->wave.freq & 0xff) | ((v & 7) << 8);
    apu->wave.length_enable = v & 0x40;
    if (v & 0x80) {
      apu->wave.enabled = apu->wave.dac;
      if (apu->wave.length == 0) apu->wave.length = 256;
      apu->wave.timer = (2048 - apu->wave.freq) * 2;
      apu->wave.pos = 0;
    }
    return;
  case 0x10: apu->noise.length = 64 - (v & 63); return;
  case 0x11: apu->noise.dac = (v & 0xf8) != 0; if (!apu->noise.dac) apu->noise.enabled = false; return;
  case 0x12: apu->noise.clock_shift = v >> 4; apu->noise.width7 = v & 8; apu->noise.divisor_code = v & 7; return;
  case 0x13:
    apu->noise.length_enable = v & 0x40;
    if (v & 0x80) {
      apu->noise.enabled = apu->noise.dac;
      if (apu->noise.length == 0) apu->noise.length = 64;
      apu->noise.timer = noise_divisor[apu->noise.divisor_code] << apu->noise.clock_shift;
      apu->noise.env_period = R[0x11] & 7;
      apu->noise.env_timer = apu->noise.env_period ? apu->noise.env_period : 8;
      apu->noise.env_add = R[0x11] & 8;
      apu->noise.volume = R[0x11] >> 4;
      apu->noise.lfsr = 0x7fff;
    }
    return;
  default: return;
  }
}

uint32_t apu_samples_available(const Apu *apu) { return apu->ring_w - apu->ring_r; }

uint32_t apu_read_samples(Apu *apu, int16_t *out, uint32_t max_frames) {
  uint32_t n = 0;
  while (n < max_frames && apu->ring_r != apu->ring_w) {
    uint32_t i = (apu->ring_r % APU_RING) * 2;
    out[n * 2] = apu->ring[i];
    out[n * 2 + 1] = apu->ring[i + 1];
    apu->ring_r++;
    n++;
  }
  return n;
}
