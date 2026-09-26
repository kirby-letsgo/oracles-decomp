#include "unit.h"
#include "sync/sha256.h"
#include "sync/sync.h"

static void sha256_matches_the_standard_vectors(void) {
  char h[65];
  sha256_hex((const uint8_t *)"abc", 3, h);
  ASSERT(!strcmp(h, "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"));
  const char *m = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";   // 56 bytes: two padding blocks
  sha256_hex((const uint8_t *)m, strlen(m), h);
  ASSERT(!strcmp(h, "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1"));
}

static SyncRemote remote(int version, const char *sha, int format, int64_t updated) {
  SyncRemote r = {"seasons", "sram.sav", version, "", format, updated, "phone"};
  snprintf(r.sha, sizeof r.sha, "%s", sha);
  return r;
}

static void decide_covers_every_case(void) {
  SyncRemote r = remote(3, "bbb", -1, 1000);
  SyncRecord last = {"seasons", "sram.sav", 3, "aaa"};
  SyncInput in = {.local = true, .local_sha = "aaa", .local_mtime = 500, .ask_on_conflict = true};
  ASSERT_EQ(sync_decide(&in), SYNC_UPLOAD);                       // only here
  in.local = false; in.remote = &r;
  ASSERT_EQ(sync_decide(&in), SYNC_DOWNLOAD);                     // only on the server
  in.local = true; snprintf(in.local_sha, 65, "bbb");
  ASSERT_EQ(sync_decide(&in), SYNC_KEEP);                         // the same bytes on both
  snprintf(in.local_sha, 65, "ccc");
  ASSERT_EQ(sync_decide(&in), SYNC_ASK);                          // a new device with its own save
  in.last = &last;
  snprintf(in.local_sha, 65, "aaa"); r.version = 3;
  ASSERT_EQ(sync_decide(&in), SYNC_KEEP);                         // nothing changed since the last sync
  snprintf(in.local_sha, 65, "ddd");
  ASSERT_EQ(sync_decide(&in), SYNC_UPLOAD);                       // played here
  snprintf(in.local_sha, 65, "aaa"); r.version = 4;
  ASSERT_EQ(sync_decide(&in), SYNC_DOWNLOAD);                     // played elsewhere
  snprintf(in.local_sha, 65, "ddd");
  ASSERT_EQ(sync_decide(&in), SYNC_ASK);                          // played in both places
  in.ask_on_conflict = false;
  ASSERT_EQ(sync_decide(&in), SYNC_DOWNLOAD);                     // settings: the server's is newer
  in.local_mtime = 2000;
  ASSERT_EQ(sync_decide(&in), SYNC_UPLOAD);
}

static void states_need_the_same_format(void) {
  SyncRemote r = remote(2, "bbb", 5, 1000);
  SyncInput in = {.local = false, .remote = &r, .is_state = true, .local_format = 6, .ask_on_conflict = true};
  ASSERT_EQ(sync_decide(&in), SYNC_KEEP);                         // another build's state is not fetched
  in.local_format = 5;
  ASSERT_EQ(sync_decide(&in), SYNC_DOWNLOAD);
  in.local = true; in.local_format = 6; snprintf(in.local_sha, 65, "ccc");
  ASSERT_EQ(sync_decide(&in), SYNC_UPLOAD);                       // ours replaces it
}

static void manifest_and_records_parse(void) {
  SyncRemote r[4];
  const char *text = "seasons sram.sav 2 aaaa - 1790443843 Sara's Pixel 8\nages state_1 1 bbbb 7 1790443900 \n\n";
  ASSERT_EQ(sync_parse_manifest(text, r, 4), 2);
  ASSERT(!strcmp(r[0].device, "Sara's Pixel 8"));
  ASSERT_EQ(r[0].format, -1);
  ASSERT_EQ(r[1].format, 7);
  ASSERT_EQ(r[1].updated, 1790443900);
  ASSERT(sync_find_remote(r, 2, "ages", "state_1") == &r[1]);
  ASSERT(sync_find_remote(r, 2, "ages", "sram.sav") == NULL);

  SyncRecord rec[4];
  int n = sync_set_record(rec, 0, 4, "seasons", "sram.sav", 2, "aaaa");
  n = sync_set_record(rec, n, 4, "ages", "state_1", 1, "bbbb");
  n = sync_set_record(rec, n, 4, "seasons", "sram.sav", 3, "cccc");
  ASSERT_EQ(n, 2);
  char buf[512];
  sync_format_records(rec, n, buf, sizeof buf);
  SyncRecord back[4];
  ASSERT_EQ(sync_parse_records(buf, back, 4), 2);
  ASSERT_EQ(sync_find_record(back, 2, "seasons", "sram.sav")->version, 3);
  ASSERT(!strcmp(sync_find_record(back, 2, "seasons", "sram.sav")->sha, "cccc"));
}

static void codes_normalize(void) {
  char c[17], f[20];
  ASSERT(sync_normalize_code("4827-1930 5561.0284", c));
  ASSERT(!strcmp(c, "4827193055610284"));
  ASSERT(!sync_normalize_code("4827-1930-5561-028", c));
  ASSERT(!sync_normalize_code("4827-1930-5561-02845", c));
  ASSERT(!sync_normalize_code("4827-1930-5561-028x", c));
  sync_format_code("4827193055610284", f);
  ASSERT(!strcmp(f, "4827-1930-5561-0284"));
}

int main(void) {
  RUN(sha256_matches_the_standard_vectors);
  RUN(decide_covers_every_case);
  RUN(states_need_the_same_format);
  RUN(manifest_and_records_parse);
  RUN(codes_normalize);
  return 0;
}
