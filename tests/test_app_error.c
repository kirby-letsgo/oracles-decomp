// Exercise the real app's failure/launcher/settings path without a game ROM. Only the
// engine's frame runner is replaced; clipboard access goes through SDL's dummy driver.
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <stdio.h>
#include "core/gb.h"
static void failed_frame(GB *gb) { gb_stop(gb, "test engine failure"); }
static void defer_quit(void) {}
static bool open_url(const char *url) { printf("OPEN URL: %s\n", url); return true; }
#define gb_run_frame failed_frame
#define SDL_Quit defer_quit
#define SDL_OpenURL open_url
#define main app_main
#include "../src/platform/app.c"
#undef main
#undef SDL_Quit
#undef gb_run_frame
#undef SDL_OpenURL

int main(int argc, char **argv) {
  SDL_SetMainReady();
  int result = app_main(argc, argv);
  char *text = SDL_GetClipboardText();
  printf("CLIPBOARD:\n%s\n", text ? text : "");
  SDL_free(text);
  SDL_Quit();
  return result;
}
