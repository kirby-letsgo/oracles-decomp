#include "sync/http.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void http_response_free(HttpResponse *res) {
  free(res->body);
  res->body = NULL;
  res->size = 0;
}

const char *http_header(const HttpResponse *res, const char *name, char *out, size_t size) {
  size_t n = strlen(name);
  for (const char *p = res->headers; *p;) {
    const char *eol = strchr(p, '\n');
    if (!eol) eol = p + strlen(p);
    if ((size_t)(eol - p) > n + 1 && !strncmp(p, name, n) && p[n] == ':') {
      const char *v = p + n + 1;
      while (*v == ' ') v++;
      size_t len = (size_t)(eol - v);
      while (len && (v[len - 1] == '\r' || v[len - 1] == ' ')) len--;
      snprintf(out, size, "%.*s", (int)len, v);
      return out;
    }
    p = *eol ? eol + 1 : eol;
  }
  return NULL;
}

// Appends one "Name: value" header line to res->headers with the name lower-cased (backends call it).
void http_add_header(HttpResponse *res, const char *line, size_t len) {
  while (len && (line[len - 1] == '\r' || line[len - 1] == '\n')) len--;
  const char *colon = memchr(line, ':', len);
  if (!colon) return;
  size_t used = strlen(res->headers);
  if (used + len + 2 >= sizeof res->headers) return;
  for (size_t i = 0; i < len; i++) res->headers[used + i] = line + i < colon ? (char)tolower((unsigned char)line[i]) : line[i];
  res->headers[used + len] = '\n';
  res->headers[used + len + 1] = 0;
}
