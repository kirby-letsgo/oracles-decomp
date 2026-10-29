#include "platform/error_log.h"
#include <stdio.h>
#include <string.h>

bool error_log_read(const char *cache, char out[ERROR_LOG_SIZE]) {
  static const char *const games[] = {"ages", "ages-eu", "seasons", "seasons-eu"};
  size_t used = 0;
  out[0] = 0;
  for (unsigned i = 0; i < sizeof games / sizeof games[0]; i++) {
    char path[1300], tail[ERROR_LOG_TAIL + 1];
    int len = snprintf(path, sizeof path, "%s/%s/crash.log", cache, games[i]);
    if (len < 0 || (size_t)len >= sizeof path) continue;
    FILE *f = fopen(path, "rb");
    if (!f) continue;
    if (fseek(f, 0, SEEK_END)) { fclose(f); continue; }
    long end = ftell(f);
    if (end <= 0) { fclose(f); continue; }
    bool clipped = end > ERROR_LOG_TAIL;
    if (fseek(f, clipped ? end - ERROR_LOG_TAIL : 0, SEEK_SET)) { fclose(f); continue; }
    size_t n = fread(tail, 1, ERROR_LOG_TAIL, f);
    bool failed = ferror(f);
    fclose(f);
    if (failed || !n) continue;
    tail[n] = 0;
    // A clipped tail can start halfway through a line; keep complete lines when possible.
    const char *text = tail;
    if (clipped) {
      const char *nl = strchr(tail, '\n');
      if (nl && nl[1]) text = nl + 1;
    }
    int written = snprintf(out + used, ERROR_LOG_SIZE - used, "--- %s ---\n%s%s\n",
                           games[i], clipped ? "[older log text omitted]\n" : "", text);
    if (written > 0) used += (size_t)written;
  }
  return used != 0;
}
