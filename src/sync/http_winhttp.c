// WinHTTP: part of every Windows since XP, with the system's TLS and proxy settings.
#include "sync/http.h"
#include <windows.h>
#include <winhttp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void http_add_header(HttpResponse *res, const char *line, size_t len);

bool http_available(void) { return true; }

static bool fail(HttpResponse *res, const char *what) {
  snprintf(res->error, sizeof res->error, "%s failed (%lu)", what, (unsigned long)GetLastError());
  return false;
}

static void keep_headers(HttpResponse *res, HINTERNET r) {
  DWORD bytes = 0;
  WinHttpQueryHeaders(r, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, NULL, &bytes, WINHTTP_NO_HEADER_INDEX);
  if (!bytes) return;
  wchar_t *w = malloc(bytes + sizeof(wchar_t));
  if (!w) return;
  if (WinHttpQueryHeaders(r, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, w, &bytes, WINHTTP_NO_HEADER_INDEX)) {
    int n = WideCharToMultiByte(CP_UTF8, 0, w, -1, NULL, 0, NULL, NULL);
    char *text = malloc((size_t)n);
    if (text && WideCharToMultiByte(CP_UTF8, 0, w, -1, text, n, NULL, NULL)) {
      for (char *p = text; *p;) {
        char *eol = strchr(p, '\n');
        size_t len = eol ? (size_t)(eol - p) : strlen(p);
        http_add_header(res, p, len);
        p += len + (eol ? 1 : 0);
      }
    }
    free(text);
  }
  free(w);
}

bool http_request(const char *method, const char *url, const char *headers, const uint8_t *body, size_t size, HttpResponse *res) {
  memset(res, 0, sizeof *res);
  wchar_t wurl[1024], host[256], path[1024], extra[1024], wmethod[16], *wheaders = NULL;
  MultiByteToWideChar(CP_UTF8, 0, url, -1, wurl, 1024);
  MultiByteToWideChar(CP_UTF8, 0, method, -1, wmethod, 16);
  URL_COMPONENTS uc = {0};
  uc.dwStructSize = sizeof uc;
  uc.lpszHostName = host; uc.dwHostNameLength = 256;
  uc.lpszUrlPath = path; uc.dwUrlPathLength = 1024;
  uc.lpszExtraInfo = extra; uc.dwExtraInfoLength = 1024;
  if (!WinHttpCrackUrl(wurl, 0, 0, &uc)) return fail(res, "WinHttpCrackUrl");
  wcsncat(path, extra, 1024 - wcslen(path) - 1);
  if (headers && *headers) {
    size_t n = strlen(headers);
    char *crlf = malloc(n * 2 + 1), *q = crlf;
    for (const char *p = headers; *p; p++) { if (*p == '\n') *q++ = '\r'; *q++ = *p; }
    *q = 0;
    int wn = MultiByteToWideChar(CP_UTF8, 0, crlf, -1, NULL, 0);
    wheaders = malloc((size_t)wn * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, crlf, -1, wheaders, wn);
    free(crlf);
  }
  bool ok = false;
  HINTERNET s = WinHttpOpen(L"Oracles", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
  HINTERNET c = s ? WinHttpConnect(s, host, uc.nPort, 0) : NULL;
  HINTERNET r = c ? WinHttpOpenRequest(c, wmethod, path, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
                                       uc.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0) : NULL;
  if (!r) { fail(res, "WinHttpOpenRequest"); goto done; }
  WinHttpSetTimeouts(r, 10000, 10000, 30000, 30000);
  if (!WinHttpSendRequest(r, wheaders ? wheaders : WINHTTP_NO_ADDITIONAL_HEADERS, wheaders ? (DWORD)-1L : 0,
                          (void *)body, (DWORD)size, (DWORD)size, 0)) { fail(res, "WinHttpSendRequest"); goto done; }
  if (!WinHttpReceiveResponse(r, NULL)) { fail(res, "WinHttpReceiveResponse"); goto done; }
  DWORD status = 0, len = sizeof status;
  WinHttpQueryHeaders(r, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &status, &len, WINHTTP_NO_HEADER_INDEX);
  keep_headers(res, r);
  for (;;) {
    DWORD avail = 0, got = 0;
    if (!WinHttpQueryDataAvailable(r, &avail)) { fail(res, "WinHttpQueryDataAvailable"); goto done; }
    if (!avail) break;
    uint8_t *grown = realloc(res->body, res->size + avail + 1);
    if (!grown) goto done;
    res->body = grown;
    if (!WinHttpReadData(r, res->body + res->size, avail, &got)) { fail(res, "WinHttpReadData"); goto done; }
    res->size += got;
    res->body[res->size] = 0;
  }
  res->status = (int)status;
  ok = true;
done:
  if (r) WinHttpCloseHandle(r);
  if (c) WinHttpCloseHandle(c);
  if (s) WinHttpCloseHandle(s);
  free(wheaders);
  return ok;
}
