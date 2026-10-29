#include "unit.h"
#include "platform/error_log.h"
#ifdef _WIN32
#include <direct.h>
#define make_dir(p) _mkdir(p)
#define remove_dir(p) _rmdir(p)
#else
#include <sys/stat.h>
#include <unistd.h>
#define make_dir(p) mkdir(p, 0700)
#define remove_dir(p) rmdir(p)
#endif

int main(void) {
  const char *root = "test-error-log-data";
  char out[ERROR_LOG_SIZE];
  ASSERT_EQ(make_dir(root), 0);
  ASSERT(!error_log_read(root, out));
  ASSERT_EQ(out[0], 0);
  ASSERT_EQ(make_dir("test-error-log-data/seasons-eu"), 0);
  FILE *f = fopen("test-error-log-data/seasons-eu/crash.log", "wb");
  ASSERT(f);
  fclose(f);
  ASSERT(!error_log_read(root, out));
  f = fopen("test-error-log-data/seasons-eu/crash.log", "wb");
  ASSERT(f);
  fputs("Maple stopped\nroom 12\n", f);
  fclose(f);
  ASSERT(error_log_read(root, out));
  ASSERT(strstr(out, "--- seasons-eu ---\nMaple stopped\nroom 12\n"));
  ASSERT_EQ(make_dir("test-error-log-data/ages"), 0);
  f = fopen("test-error-log-data/ages/crash.log", "wb");
  ASSERT(f);
  fputs("old report\n", f);
  for (int i = 0; i < ERROR_LOG_TAIL; i++) fputs("old line\n", f);
  fputs("latest report\n", f);
  fclose(f);
  ASSERT(error_log_read(root, out));
  ASSERT(strstr(out, "[older log text omitted]"));
  ASSERT(!strstr(out, "old report"));
  ASSERT(strstr(out, "latest report"));
  ASSERT(strstr(out, "Maple stopped"));
  ASSERT(strlen(out) < ERROR_LOG_SIZE);
  remove("test-error-log-data/ages/crash.log");
  remove("test-error-log-data/seasons-eu/crash.log");
  remove_dir("test-error-log-data/ages");
  remove_dir("test-error-log-data/seasons-eu");
  remove_dir(root);
  puts("error log: missing, empty, EU, multiple games and bounded tails passed");
  return 0;
}
