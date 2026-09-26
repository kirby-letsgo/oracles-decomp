// libcurl, loaded when first needed so the app neither links it nor needs its headers: macOS
// always has it, most Linux systems do; without it sync is unavailable.
#include "sync/http.h"
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void http_add_header(HttpResponse *res, const char *line, size_t len);

enum {
  OPT_WRITEDATA = 10001, OPT_URL = 10002, OPT_POSTFIELDS = 10015, OPT_HTTPHEADER = 10023,
  OPT_HEADERDATA = 10029, OPT_CUSTOMREQUEST = 10036, OPT_WRITEFUNCTION = 20011,
  OPT_HEADERFUNCTION = 20079, OPT_POSTFIELDSIZE_LARGE = 30120, OPT_TIMEOUT = 13, OPT_NOSIGNAL = 99,
  OPT_FOLLOWLOCATION = 52, INFO_RESPONSE_CODE = 0x200002,
};

static struct {
  bool tried, ok;
  void *(*easy_init)(void);
  int (*easy_setopt)(void *, int, ...);
  int (*easy_perform)(void *);
  int (*easy_getinfo)(void *, int, ...);
  void (*easy_cleanup)(void *);
  const char *(*easy_strerror)(int);
  void *(*slist_append)(void *, const char *);
  void (*slist_free_all)(void *);
} curl;

static bool load(void) {
  if (curl.tried) return curl.ok;
  curl.tried = true;
  static const char *const names[] = {"libcurl.4.dylib", "libcurl.so.4", "libcurl.so", "libcurl-gnutls.so.4"};
  void *h = NULL;
  for (size_t i = 0; i < sizeof names / sizeof names[0] && !h; i++) h = dlopen(names[i], RTLD_NOW | RTLD_LOCAL);
  if (!h) return false;
  *(void **)&curl.easy_init = dlsym(h, "curl_easy_init");
  *(void **)&curl.easy_setopt = dlsym(h, "curl_easy_setopt");
  *(void **)&curl.easy_perform = dlsym(h, "curl_easy_perform");
  *(void **)&curl.easy_getinfo = dlsym(h, "curl_easy_getinfo");
  *(void **)&curl.easy_cleanup = dlsym(h, "curl_easy_cleanup");
  *(void **)&curl.easy_strerror = dlsym(h, "curl_easy_strerror");
  *(void **)&curl.slist_append = dlsym(h, "curl_slist_append");
  *(void **)&curl.slist_free_all = dlsym(h, "curl_slist_free_all");
  curl.ok = curl.easy_init && curl.easy_setopt && curl.easy_perform && curl.easy_getinfo && curl.easy_cleanup &&
            curl.easy_strerror && curl.slist_append && curl.slist_free_all;
  return curl.ok;
}

bool http_available(void) { return load(); }

static size_t on_body(char *data, size_t size, size_t n, void *ctx) {
  HttpResponse *res = ctx;
  size_t len = size * n;
  uint8_t *grown = realloc(res->body, res->size + len + 1);
  if (!grown) return 0;
  res->body = grown;
  memcpy(res->body + res->size, data, len);
  res->size += len;
  res->body[res->size] = 0;
  return len;
}

static size_t on_header(char *data, size_t size, size_t n, void *ctx) {
  http_add_header(ctx, data, size * n);
  return size * n;
}

bool http_request(const char *method, const char *url, const char *headers, const uint8_t *body, size_t size, HttpResponse *res) {
  memset(res, 0, sizeof *res);
  if (!load()) { snprintf(res->error, sizeof res->error, "libcurl is not installed"); return false; }
  void *c = curl.easy_init();
  if (!c) { snprintf(res->error, sizeof res->error, "curl_easy_init failed"); return false; }
  void *list = NULL;
  for (const char *p = headers; p && *p;) {
    const char *eol = strchr(p, '\n');
    char line[256];
    snprintf(line, sizeof line, "%.*s", (int)(eol ? eol - p : (long)strlen(p)), p);
    if (*line) list = curl.slist_append(list, line);
    p = eol ? eol + 1 : NULL;
  }
  curl.easy_setopt(c, OPT_URL, url);
  curl.easy_setopt(c, OPT_CUSTOMREQUEST, method);
  curl.easy_setopt(c, OPT_NOSIGNAL, 1L);
  curl.easy_setopt(c, OPT_TIMEOUT, 30L);
  curl.easy_setopt(c, OPT_FOLLOWLOCATION, 1L);
  if (list) curl.easy_setopt(c, OPT_HTTPHEADER, list);
  if (body) {
    curl.easy_setopt(c, OPT_POSTFIELDS, body ? (const char *)body : "");
    curl.easy_setopt(c, OPT_POSTFIELDSIZE_LARGE, (long long)size);
  }
  curl.easy_setopt(c, OPT_WRITEFUNCTION, on_body);
  curl.easy_setopt(c, OPT_WRITEDATA, res);
  curl.easy_setopt(c, OPT_HEADERFUNCTION, on_header);
  curl.easy_setopt(c, OPT_HEADERDATA, res);
  int rc = curl.easy_perform(c);
  long status = 0;
  if (rc == 0) curl.easy_getinfo(c, INFO_RESPONSE_CODE, &status);
  else snprintf(res->error, sizeof res->error, "%s", curl.easy_strerror(rc));
  res->status = (int)status;
  if (list) curl.slist_free_all(list);
  curl.easy_cleanup(c);
  return rc == 0;
}
