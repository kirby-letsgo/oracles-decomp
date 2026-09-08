#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "core/gb.h"
#include "platform/png.h"
#include "platform/render.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint8_t live_joy;
static uint8_t live_input(void *ctx, uint64_t frame) { (void)ctx; (void)frame; return live_joy; }

#define SCALE 4
#define AUDIO_TARGET_BYTES (APU_SAMPLE_RATE / 10 * 4)

static uint8_t *read_all(const char *path, size_t *size) {
  FILE *f = fopen(path, "rb");
  if (!f) return NULL;
  fseek(f, 0, SEEK_END);
  long n = ftell(f);
  fseek(f, 0, SEEK_SET);
  uint8_t *buf = malloc(n);
  if (fread(buf, 1, n, f) != (size_t)n) { free(buf); fclose(f); return NULL; }
  fclose(f);
  *size = n;
  return buf;
}

static void sav_path(const char *rom_path, char *out, size_t n) {
  snprintf(out, n, "%s", rom_path);
  char *dot = strrchr(out, '.');
  if (dot) *dot = 0;
  strncat(out, ".sav", n - strlen(out) - 1);
}

static void load_sram(GB *gb, const char *path) {
  FILE *f = fopen(path, "rb");
  if (!f) return;
  size_t n = fread(gb->eram, 1, gb->eram_size, f);
  (void)n;
  fclose(f);
}

static void save_sram(GB *gb, const char *path) {
  if (gb->eram_size == 0) return;
  FILE *f = fopen(path, "wb");
  if (!f) return;
  fwrite(gb->eram, 1, gb->eram_size, f);
  fclose(f);
}

static uint8_t key_bit(SDL_Scancode sc) {
  switch (sc) {
  case SDL_SCANCODE_UP: return JOY_UP;
  case SDL_SCANCODE_DOWN: return JOY_DOWN;
  case SDL_SCANCODE_LEFT: return JOY_LEFT;
  case SDL_SCANCODE_RIGHT: return JOY_RIGHT;
  case SDL_SCANCODE_X: return JOY_A;
  case SDL_SCANCODE_Z: return JOY_B;
  case SDL_SCANCODE_RETURN: return JOY_START;
  case SDL_SCANCODE_RSHIFT: return JOY_SELECT;
  default: return 0;
  }
}

static uint8_t pad_bit(int button) {
  switch (button) {
  case SDL_GAMEPAD_BUTTON_DPAD_UP: return JOY_UP;
  case SDL_GAMEPAD_BUTTON_DPAD_DOWN: return JOY_DOWN;
  case SDL_GAMEPAD_BUTTON_DPAD_LEFT: return JOY_LEFT;
  case SDL_GAMEPAD_BUTTON_DPAD_RIGHT: return JOY_RIGHT;
  case SDL_GAMEPAD_BUTTON_SOUTH: return JOY_A;
  case SDL_GAMEPAD_BUTTON_WEST: return JOY_B;
  case SDL_GAMEPAD_BUTTON_START: return JOY_START;
  case SDL_GAMEPAD_BUTTON_BACK: return JOY_SELECT;
  default: return 0;
  }
}

int main(int argc, char **argv) {
  if (argc < 2) { fprintf(stderr, "usage: oracles ROM [BOOTROM]\n"); return 2; }
  size_t rom_size, boot_size;
  uint8_t *rom = read_all(argv[1], &rom_size);
  if (!rom) { fprintf(stderr, "cannot read %s\n", argv[1]); return 2; }
  GB *gb = calloc(1, sizeof *gb);
  gb_init(gb);
  if (!gb_load_rom(gb, rom, rom_size)) { fprintf(stderr, "unsupported ROM\n"); return 2; }
  if (argc > 2) {
    uint8_t *boot = read_all(argv[2], &boot_size);
    if (boot) gb_set_boot_rom(gb, boot, boot_size);
  }
  gb_reset(gb);
  char sav[1024];
  sav_path(argv[1], sav, sizeof sav);
  load_sram(gb, sav);

  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) { fprintf(stderr, "%s\n", SDL_GetError()); return 2; }
  SDL_Window *win;
  SDL_Renderer *ren;
  if (!SDL_CreateWindowAndRenderer("Oracles", FB_W * SCALE, FB_H * SCALE, 0, &win, &ren)) return 2;
  SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, FB_W, FB_H);
  SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
  SDL_AudioSpec spec = {SDL_AUDIO_S16, 2, APU_SAMPLE_RATE};
  SDL_AudioStream *audio = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
  if (audio) SDL_ResumeAudioStreamDevice(audio);

  static uint8_t rgb[FB_W * FB_H * 3];
  static int16_t samples[APU_RING * 2];
  bool running = true;
  gb->input_at = live_input;
  uint64_t frames = 0;
  while (running) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
      switch (ev.type) {
      case SDL_EVENT_QUIT: running = false; break;
      case SDL_EVENT_KEY_DOWN:
        if (ev.key.scancode == SDL_SCANCODE_ESCAPE) running = false;
        else if (ev.key.scancode == SDL_SCANCODE_F12) {
          char path[256];
          snprintf(path, sizeof path, "out/screenshot_%llu.png", (unsigned long long)frames);
          framebuffer_to_rgb(gb->sample->framebuffer, rgb);
          png_write_rgb(path, rgb, FB_W, FB_H);
        } else live_joy |= key_bit(ev.key.scancode);
        break;
      case SDL_EVENT_KEY_UP: live_joy &= ~key_bit(ev.key.scancode); break;
      case SDL_EVENT_GAMEPAD_ADDED: SDL_OpenGamepad(ev.gdevice.which); break;
      case SDL_EVENT_GAMEPAD_BUTTON_DOWN: live_joy |= pad_bit(ev.gbutton.button); break;
      case SDL_EVENT_GAMEPAD_BUTTON_UP: live_joy &= ~pad_bit(ev.gbutton.button); break;
      }
    }
    if (audio && SDL_GetAudioStreamQueued(audio) > AUDIO_TARGET_BYTES) { SDL_Delay(1); continue; }
    gb_run_frame(gb);
    frames++;
    uint32_t n = apu_read_samples(&gb->apu, samples, APU_RING);
    if (audio) SDL_PutAudioStreamData(audio, samples, n * 4);
    framebuffer_to_rgb(gb->sample->framebuffer, rgb);
    SDL_UpdateTexture(tex, NULL, rgb, FB_W * 3);
    SDL_RenderClear(ren);
    SDL_RenderTexture(ren, tex, NULL, NULL);
    SDL_RenderPresent(ren);
    if (frames % 600 == 0) save_sram(gb, sav);
  }
  save_sram(gb, sav);
  SDL_Quit();
  return 0;
}
