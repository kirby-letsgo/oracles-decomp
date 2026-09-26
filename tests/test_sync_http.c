// Against a running sync server: ORACLES_SYNC_URL=http://host:port test_sync_http (skipped without it).
#include "unit.h"
#include "sync/http.h"
#include "sync/sha256.h"
#include "sync/sync.h"

static const char *base;

static void round_trip_through_the_server(void) {
  char url[512], code[17], h[80];
  HttpResponse res;
  snprintf(url, sizeof url, "%s/accounts", base);
  ASSERT(http_request("POST", url, NULL, NULL, 0, &res));
  ASSERT_EQ(res.status, 201);
  const char *q = strstr((const char *)res.body, "\"code\":\"");
  ASSERT(q && sync_normalize_code(strtok((char *)q + 8, "\""), code));
  http_response_free(&res);

  uint8_t save[8192];
  for (size_t i = 0; i < sizeof save; i++) save[i] = (uint8_t)(i * 7);
  char sha[65];
  sha256_hex(save, sizeof save, sha);
  snprintf(url, sizeof url, "%s/accounts/%s/files/seasons/sram.sav", base, code);
  const char *hdr = "Content-Type: application/octet-stream\nX-Device: test_sync_http\nX-Base-Version: 0\n";
  ASSERT(http_request("PUT", url, hdr, save, sizeof save, &res));
  ASSERT_EQ(res.status, 201);
  http_response_free(&res);
  ASSERT(http_request("PUT", url, hdr, save, sizeof save, &res));
  ASSERT_EQ(res.status, 200);                                   // the same bytes: no new version
  http_response_free(&res);
  save[0] ^= 1;
  ASSERT(http_request("PUT", url, hdr, save, sizeof save, &res));
  ASSERT_EQ(res.status, 409);                                   // based on version 0, the server has 1
  http_response_free(&res);
  save[0] ^= 1;

  snprintf(url, sizeof url, "%s/accounts/%s/manifest", base, code);
  ASSERT(http_request("GET", url, NULL, NULL, 0, &res));
  ASSERT_EQ(res.status, 200);
  SyncRemote r[4];
  ASSERT_EQ(sync_parse_manifest((const char *)res.body, r, 4), 1);
  ASSERT_EQ(r[0].version, 1);
  ASSERT(!strcmp(r[0].sha, sha));
  ASSERT(!strcmp(r[0].device, "test_sync_http"));
  http_response_free(&res);

  snprintf(url, sizeof url, "%s/accounts/%s/files/seasons/sram.sav", base, code);
  ASSERT(http_request("GET", url, NULL, NULL, 0, &res));
  ASSERT_EQ(res.status, 200);
  ASSERT_EQ(res.size, sizeof save);
  ASSERT(!memcmp(res.body, save, sizeof save));
  ASSERT(http_header(&res, "x-version", h, sizeof h) && !strcmp(h, "1"));
  ASSERT(http_header(&res, "x-sha256", h, sizeof h) && !strcmp(h, sha));
  http_response_free(&res);
}

static void reports_an_unreachable_server(void) {
  HttpResponse res;
  ASSERT(!http_request("GET", "http://127.0.0.1:9/health", NULL, NULL, 0, &res));
  ASSERT_EQ(res.status, 0);
  ASSERT(res.error[0]);
}

int main(void) {
  base = getenv("ORACLES_SYNC_URL");
  if (!base || !*base) SKIP("ORACLES_SYNC_URL not set");
  if (!http_available()) SKIP("no HTTP backend on this system");
  RUN(round_trip_through_the_server);
  RUN(reports_an_unreachable_server);
  return 0;
}
