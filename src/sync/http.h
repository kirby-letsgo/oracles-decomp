#pragma once
// A blocking HTTP(S) request with the platform's own TLS: libcurl loaded at run time (macOS, Linux),
// WinHTTP (Windows), HttpURLConnection through JNI (Android). Run it off the game's thread.
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
  int status;                   // 0 when the request did not complete
  uint8_t *body;                // malloc'd, NUL-terminated beyond size
  size_t size;
  char headers[1024];           // "name: value\n" lines, names lower-case
  char error[128];
} HttpResponse;

// headers: "Name: value\n" lines (or NULL); body may be NULL.
bool http_request(const char *method, const char *url, const char *headers, const uint8_t *body, size_t size, HttpResponse *res);
void http_response_free(HttpResponse *res);
// A response header's value (name lower-case), or NULL.
const char *http_header(const HttpResponse *res, const char *name, char *out, size_t size);
bool http_available(void);
