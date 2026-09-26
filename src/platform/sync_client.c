#include "platform/sync_client.h"
#include "platform/setup.h"
#include "sync/http.h"
#include "sync/sha256.h"
#include "ui/savefile.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// The synced files of a game; a state slot's thumbnail travels with its state.
static const char *const game_files[] = {"sram.sav", "item_buttons", "state_1", "state_2", "state_3", "state_4", "state_auto"};
#define SHARED "shared"

void sync_config_load(SyncConfig *c, const char *cache) {
  memset(c, 0, sizeof *c);
  snprintf(c->url, sizeof c->url, "%s", ORACLES_SYNC_URL);
  char path[1100];
  snprintf(path, sizeof path, "%ssync.ini", cache);
  size_t n;
  char *text = (char *)oracles_read_file(path, &n);
  if (!text) return;
  char *z = realloc(text, n + 1);
  if (!z) { free(text); return; }
  z[n] = 0;
  for (char *line = strtok(z, "\n"); line; line = strtok(NULL, "\n")) {
    char value[256];
    if (sscanf(line, "url=%255s", value) == 1) snprintf(c->url, sizeof c->url, "%s", value);
    else if (sscanf(line, "code=%255s", value) == 1 && !sync_normalize_code(value, c->code)) c->code[0] = 0;
  }
  free(z);
}

void sync_config_store(const SyncConfig *c, const char *cache) {
  char path[1100], text[400];
  snprintf(path, sizeof path, "%ssync.ini", cache);
  int n = snprintf(text, sizeof text, "url=%s\ncode=%s\n", c->url, c->code);
  SDL_SaveFile(path, text, (size_t)n);
}

static void local_path(char *out, size_t n, const char *cache, const char *game, const char *name) {
  if (!strcmp(game, SHARED)) snprintf(out, n, "%s%s", cache, name);
  else snprintf(out, n, "%s%s/%s", cache, game, name);
}

// Writes next to the file, then renames over it, so a reader never sees half a save.
static bool write_atomic(const char *path, const void *data, size_t n) {
  char tmp[1200];
  snprintf(tmp, sizeof tmp, "%s.sync", path);
  if (!SDL_SaveFile(tmp, data, n)) return false;
  SDL_RemovePath(path);
  return SDL_RenamePath(tmp, path);
}

typedef struct {
  bool exists;
  char sha[65];
  int64_t mtime;
} LocalFile;

// A save with no valid file in it (the game writes one on first start) is no save to keep.
static bool blank_save(const char *path, const uint8_t *data, size_t n) {
  size_t len = strlen(path);
  if (len < 8 || strcmp(path + len - 8, "sram.sav")) return false;
  UiFileInfo files[UI_FILES];
  ui_read_files(data, n, strstr(path, "/ages/sram.sav") != NULL, files);
  for (int i = 0; i < UI_FILES; i++)
    if (files[i].valid) return false;
  return true;
}

static LocalFile read_local(const char *path) {
  LocalFile f = {0};
  size_t n;
  uint8_t *data = oracles_read_file(path, &n);
  if (!data) return f;
  f.exists = !blank_save(path, data, n);
  sha256_hex(data, n, f.sha);
  free(data);
  SDL_PathInfo info;
  if (SDL_GetPathInfo(path, &info)) f.mtime = info.modify_time / 1000000000;
  return f;
}

static const char *device_name(void) {
  static char name[64];
  if (!*name) snprintf(name, sizeof name, "%s", SDL_GetPlatform());
  return name;
}

// The sync record: "account CODE" then "game name version sha" per file.
typedef struct {
  SyncRecord rec[SYNC_MAX_FILES];
  int n;
} Records;

static void records_load(Records *r, const char *cache, const char *code) {
  char path[1100];
  snprintf(path, sizeof path, "%ssync_state", cache);
  r->n = 0;
  size_t n;
  char *text = (char *)oracles_read_file(path, &n);
  if (!text) return;
  char *z = realloc(text, n + 1);
  if (!z) { free(text); return; }
  z[n] = 0;
  char account[32] = "";
  if (sscanf(z, "account %31s", account) == 1 && !strcmp(account, code)) {
    char *body = strchr(z, '\n');
    if (body) r->n = sync_parse_records(body + 1, r->rec, SYNC_MAX_FILES);
  }
  free(z);
}

static void records_store(const Records *r, const char *cache, const char *code) {
  char path[1100], text[SYNC_MAX_FILES * 120 + 64];
  snprintf(path, sizeof path, "%ssync_state", cache);
  int used = snprintf(text, sizeof text, "account %s\n", code);
  used += sync_format_records(r->rec, r->n, text + used, sizeof text - (size_t)used);
  SDL_SaveFile(path, text, (size_t)used);
}

static void file_url(char *out, size_t n, const SyncConfig *c, const char *game, const char *name) {
  snprintf(out, n, "%s/accounts/%s/files/%s/%s", c->url, c->code, game, name);
}

static bool is_state(const char *name) { return !strncmp(name, "state_", 6); }

// Uploads the local file as the next version after base; returns the version the server keeps.
static int upload(const SyncConfig *c, const char *cache, const char *game, const char *name, int base, char *error, size_t esize) {
  char path[1200], url[600], headers[256];
  local_path(path, sizeof path, cache, game, name);
  size_t n;
  uint8_t *data = oracles_read_file(path, &n);
  if (!data) return -1;
  file_url(url, sizeof url, c, game, name);
  int len = snprintf(headers, sizeof headers, "Content-Type: application/octet-stream\nX-Device: %s\nX-Base-Version: %d\n", device_name(), base);
  if (is_state(name) && !strstr(name, ".thumb")) snprintf(headers + len, sizeof headers - (size_t)len, "X-Format-Version: %d\n", oracles_state_format());
  HttpResponse res;
  bool ok = http_request("PUT", url, headers, data, n, &res);
  free(data);
  int version = -1;
  if (ok && (res.status == 200 || res.status == 201)) {
    const char *v = res.body ? strstr((const char *)res.body, "\"version\":") : NULL;
    if (v) version = atoi(v + 10);
  } else snprintf(error, esize, "%s %s: %s", ok ? "upload refused" : "upload failed", name, ok ? (res.status == 409 ? "changed elsewhere" : "server error") : res.error);
  http_response_free(&res);
  return version;
}

// Downloads the server's current file over the local one; fills version and sha.
static bool download(const SyncConfig *c, const char *cache, const char *game, const char *name, int *version, char sha[65], char *error, size_t esize) {
  char path[1200], url[600], h[80];
  local_path(path, sizeof path, cache, game, name);
  file_url(url, sizeof url, c, game, name);
  HttpResponse res;
  bool ok = http_request("GET", url, NULL, NULL, 0, &res) && res.status == 200;
  if (ok) {
    sha256_hex(res.body ? res.body : (const uint8_t *)"", res.size, sha);
    *version = http_header(&res, "x-version", h, sizeof h) ? atoi(h) : 0;
    ok = write_atomic(path, res.body ? res.body : (const uint8_t *)"", res.size);
    if (!ok) snprintf(error, esize, "could not write %s", name);
  } else snprintf(error, esize, "download failed %s: %s", name, res.error[0] ? res.error : "server error");
  http_response_free(&res);
  return ok;
}

// Moves a state slot's thumbnail the same way as its state (thumbnails are never asked about).
static void follow_thumb(const SyncConfig *c, const char *cache, const char *game, const char *state, bool up,
                         const SyncRemote *remote, int nremote, Records *rec, SyncResult *r) {
  char thumb[32], path[1200];
  snprintf(thumb, sizeof thumb, "%s.thumb", state);
  const SyncRemote *tr = sync_find_remote(remote, nremote, game, thumb);
  if (up) {
    local_path(path, sizeof path, cache, game, thumb);
    LocalFile lf = read_local(path);
    if (!lf.exists || (tr && !strcmp(tr->sha, lf.sha))) return;
    int v = upload(c, cache, game, thumb, tr ? tr->version : 0, r->error, sizeof r->error);
    if (v > 0) rec->n = sync_set_record(rec->rec, rec->n, SYNC_MAX_FILES, game, thumb, v, lf.sha);
  } else if (tr) {
    int v;
    char sha[65];
    if (download(c, cache, game, thumb, &v, sha, r->error, sizeof r->error)) rec->n = sync_set_record(rec->rec, rec->n, SYNC_MAX_FILES, game, thumb, v, sha);
  }
}

static void sync_file(const SyncConfig *c, const char *cache, const char *game, const char *name,
                      const SyncRemote *remote, int nremote, Records *rec, SyncResult *r) {
  char path[1200];
  local_path(path, sizeof path, cache, game, name);
  LocalFile lf = read_local(path);
  const SyncRemote *rm = sync_find_remote(remote, nremote, game, name);
  const SyncRecord *last = sync_find_record(rec->rec, rec->n, game, name);
  SyncInput in = {lf.exists, "", lf.mtime, rm, last, oracles_state_format(), is_state(name),
                  strcmp(name, "settings.ini") && strcmp(name, "item_buttons")};
  snprintf(in.local_sha, sizeof in.local_sha, "%s", lf.sha);
  switch (sync_decide(&in)) {
  case SYNC_KEEP:
    if (lf.exists && rm && !strcmp(lf.sha, rm->sha)) rec->n = sync_set_record(rec->rec, rec->n, SYNC_MAX_FILES, game, name, rm->version, rm->sha);
    break;
  case SYNC_UPLOAD: {
    int v = upload(c, cache, game, name, rm ? rm->version : 0, r->error, sizeof r->error);
    if (v > 0) {
      rec->n = sync_set_record(rec->rec, rec->n, SYNC_MAX_FILES, game, name, v, lf.sha);
      r->uploaded++;
      if (is_state(name)) follow_thumb(c, cache, game, name, true, remote, nremote, rec, r);
    }
    break;
  }
  case SYNC_DOWNLOAD: {
    int v;
    char sha[65];
    if (download(c, cache, game, name, &v, sha, r->error, sizeof r->error)) {
      rec->n = sync_set_record(rec->rec, rec->n, SYNC_MAX_FILES, game, name, v, sha);
      r->downloaded++;
      if (!strcmp(name, "settings.ini")) r->settings_changed = true;
      if (is_state(name)) follow_thumb(c, cache, game, name, false, remote, nremote, rec, r);
    }
    break;
  }
  case SYNC_ASK:
    if (r->choices < SYNC_MAX_CHOICES) {
      SyncChoice *ch = &r->choice[r->choices++];
      snprintf(ch->game, sizeof ch->game, "%s", game);
      snprintf(ch->name, sizeof ch->name, "%s", name);
      ch->remote = *rm;
      ch->local = lf.exists;
      ch->local_mtime = lf.mtime;
    }
    break;
  }
}

static int fetch_manifest(const SyncConfig *c, SyncRemote *remote, int max, char *error, size_t esize) {
  char url[600];
  snprintf(url, sizeof url, "%s/accounts/%s/manifest", c->url, c->code);
  HttpResponse res;
  int n = -1;
  if (http_request("GET", url, NULL, NULL, 0, &res) && res.status == 200) n = sync_parse_manifest(res.body ? (const char *)res.body : "", remote, max);
  else if (res.status == 404) snprintf(error, esize, "unknown sync code");
  else snprintf(error, esize, "%s", res.error[0] ? res.error : "server error");
  http_response_free(&res);
  return n;
}

void sync_run(const SyncConfig *c, const char *cache, const char *game, SyncResult *r) {
  memset(r, 0, sizeof *r);
  if (!c->code[0]) { snprintf(r->error, sizeof r->error, "sync is off"); return; }
  static SyncRemote remote[SYNC_MAX_FILES];
  int nremote = fetch_manifest(c, remote, SYNC_MAX_FILES, r->error, sizeof r->error);
  if (nremote < 0) return;
  r->ok = true;
  Records rec;
  records_load(&rec, cache, c->code);
  sync_file(c, cache, SHARED, "settings.ini", remote, nremote, &rec, r);
  static const char *const all[] = {"ages", "seasons"};
  for (int g = 0; g < 2; g++) {
    const char *name = game && !strcmp(game, SYNC_ALL_GAMES) ? all[g] : g == 0 ? game : NULL;
    char dir[1100];
    snprintf(dir, sizeof dir, "%s%s", cache, name ? name : "");
    if (!name || !SDL_GetPathInfo(dir, NULL)) continue;           // not installed on this device
    for (size_t i = 0; i < sizeof game_files / sizeof game_files[0]; i++) sync_file(c, cache, name, game_files[i], remote, nremote, &rec, r);
  }
  records_store(&rec, cache, c->code);
}

uint8_t *sync_fetch(const SyncConfig *c, const char *game, const char *name, size_t *size) {
  char url[600];
  file_url(url, sizeof url, c, game, name);
  HttpResponse res;
  uint8_t *data = NULL;
  if (http_request("GET", url, NULL, NULL, 0, &res) && res.status == 200) {
    data = res.body;
    *size = res.size;
    res.body = NULL;
  }
  http_response_free(&res);
  return data;
}

bool sync_resolve(const SyncConfig *c, const char *cache, const SyncChoice *ch, bool keep_local) {
  Records rec;
  records_load(&rec, cache, c->code);
  SyncResult r = {0};
  bool ok;
  if (keep_local) {
    char path[1200];
    local_path(path, sizeof path, cache, ch->game, ch->name);
    LocalFile lf = read_local(path);
    int v = upload(c, cache, ch->game, ch->name, ch->remote.version, r.error, sizeof r.error);
    ok = v > 0;
    if (ok) rec.n = sync_set_record(rec.rec, rec.n, SYNC_MAX_FILES, ch->game, ch->name, v, lf.sha);
  } else {
    int v;
    char sha[65];
    ok = download(c, cache, ch->game, ch->name, &v, sha, r.error, sizeof r.error);
    if (ok) rec.n = sync_set_record(rec.rec, rec.n, SYNC_MAX_FILES, ch->game, ch->name, v, sha);
  }
  if (ok && is_state(ch->name)) {
    static SyncRemote remote[SYNC_MAX_FILES];
    int n = fetch_manifest(c, remote, SYNC_MAX_FILES, r.error, sizeof r.error);
    if (n >= 0) follow_thumb(c, cache, ch->game, ch->name, keep_local, remote, n, &rec, &r);
  }
  records_store(&rec, cache, c->code);
  return ok;
}

bool sync_create_account(const char *url, char code[17], char *error, size_t size) {
  char full[400];
  snprintf(full, sizeof full, "%s/accounts", url);
  HttpResponse res;
  bool ok = http_request("POST", full, NULL, NULL, 0, &res) && res.status == 201;
  const char *q = ok && res.body ? strstr((const char *)res.body, "\"code\":\"") : NULL;
  char raw[32] = "";
  if (q) sscanf(q + 8, "%31[0-9-]", raw);
  ok = q && sync_normalize_code(raw, code);
  if (!ok) snprintf(error, size, "%s", res.status == 429 ? "too many new accounts, try later" : res.error[0] ? res.error : "server error");
  http_response_free(&res);
  return ok;
}

static struct {
  SDL_AtomicInt state;          // 0 idle, 1 running, 2 finished
  SDL_Thread *thread;
  SyncConfig config;
  char cache[1024], game[8];
  SyncResult result;
} bg;

static int SDLCALL sync_thread(void *data) {
  (void)data;
  sync_run(&bg.config, bg.cache, bg.game[0] ? bg.game : NULL, &bg.result);
  SDL_SetAtomicInt(&bg.state, 2);
  return 0;
}

bool sync_busy(void) { return SDL_GetAtomicInt(&bg.state) == 1; }

bool sync_start(const SyncConfig *c, const char *cache, const char *game) {
  if (!c->code[0] || SDL_GetAtomicInt(&bg.state) != 0) return false;
  bg.config = *c;
  snprintf(bg.cache, sizeof bg.cache, "%s", cache);
  snprintf(bg.game, sizeof bg.game, "%s", game ? game : "");
  SDL_SetAtomicInt(&bg.state, 1);
  bg.thread = SDL_CreateThread(sync_thread, "sync", NULL);
  if (!bg.thread) { SDL_SetAtomicInt(&bg.state, 0); return false; }
  return true;
}

bool sync_poll(SyncResult *out) {
  if (SDL_GetAtomicInt(&bg.state) != 2) return false;
  SDL_WaitThread(bg.thread, NULL);
  bg.thread = NULL;
  *out = bg.result;
  SDL_SetAtomicInt(&bg.state, 0);
  return true;
}

bool sync_wait(int timeout_ms) {
  for (int t = 0; t < timeout_ms && SDL_GetAtomicInt(&bg.state) == 1; t += 10) SDL_Delay(10);
  return SDL_GetAtomicInt(&bg.state) != 1;
}
