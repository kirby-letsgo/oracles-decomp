#include "sync/sync.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SyncAction sync_decide(const SyncInput *in) {
  const SyncRemote *r = in->remote;
  const SyncRecord *last = in->last;
  bool usable = r && (!in->is_state || r->format == in->local_format);
  if (in->local && r && !strcmp(in->local_sha, r->sha)) return SYNC_KEEP;
  bool local_changed = in->local && (!last || strcmp(in->local_sha, last->sha));
  bool remote_changed = usable && (!last || r->version != last->version);
  if (local_changed && remote_changed) {
    if (in->ask_on_conflict) return SYNC_ASK;
    return in->local_mtime >= r->updated ? SYNC_UPLOAD : SYNC_DOWNLOAD;
  }
  if (local_changed) return SYNC_UPLOAD;
  if (remote_changed) return SYNC_DOWNLOAD;
  return SYNC_KEEP;
}

static bool known_line(const char *p, const char **eol) {
  *eol = strchr(p, '\n');
  if (!*eol) *eol = p + strlen(p);
  return *eol > p;
}

int sync_parse_manifest(const char *text, SyncRemote *out, int max) {
  int n = 0;
  for (const char *p = text, *eol; p && *p && n < max; p = *eol ? eol + 1 : NULL) {
    if (!known_line(p, &eol)) continue;
    char line[512], format[16];
    snprintf(line, sizeof line, "%.*s", (int)(eol - p), p);
    SyncRemote *r = &out[n];
    long long updated = 0;
    int used = 0;
    memset(r, 0, sizeof *r);
    if (sscanf(line, "%7s %23s %d %64s %15s %lld %n", r->game, r->name, &r->version, r->sha, format, &updated, &used) < 6) continue;
    r->format = strcmp(format, "-") ? atoi(format) : -1;
    r->updated = updated;
    if (used) snprintf(r->device, sizeof r->device, "%s", line + used);
    n++;
  }
  return n;
}

const SyncRemote *sync_find_remote(const SyncRemote *r, int n, const char *game, const char *name) {
  for (int i = 0; i < n; i++)
    if (!strcmp(r[i].game, game) && !strcmp(r[i].name, name)) return &r[i];
  return NULL;
}

int sync_parse_records(const char *text, SyncRecord *out, int max) {
  int n = 0;
  for (const char *p = text, *eol; p && *p && n < max; p = *eol ? eol + 1 : NULL) {
    if (!known_line(p, &eol)) continue;
    char line[256];
    snprintf(line, sizeof line, "%.*s", (int)(eol - p), p);
    SyncRecord *r = &out[n];
    if (sscanf(line, "%7s %23s %d %64s", r->game, r->name, &r->version, r->sha) == 4) n++;
  }
  return n;
}

int sync_format_records(const SyncRecord *r, int n, char *out, size_t size) {
  size_t used = 0;
  out[0] = 0;
  for (int i = 0; i < n && used < size; i++)
    used += (size_t)snprintf(out + used, size - used, "%s %s %d %s\n", r[i].game, r[i].name, r[i].version, r[i].sha);
  return (int)(used < size ? used : size - 1);
}

const SyncRecord *sync_find_record(const SyncRecord *r, int n, const char *game, const char *name) {
  for (int i = 0; i < n; i++)
    if (!strcmp(r[i].game, game) && !strcmp(r[i].name, name)) return &r[i];
  return NULL;
}

int sync_set_record(SyncRecord *r, int n, int max, const char *game, const char *name, int version, const char *sha) {
  SyncRecord *rec = (SyncRecord *)sync_find_record(r, n, game, name);
  if (!rec) {
    if (n >= max) return n;
    rec = &r[n++];
    snprintf(rec->game, sizeof rec->game, "%s", game);
    snprintf(rec->name, sizeof rec->name, "%s", name);
  }
  rec->version = version;
  snprintf(rec->sha, sizeof rec->sha, "%s", sha);
  return n;
}

bool sync_normalize_code(const char *in, char out[17]) {
  int n = 0;
  for (; *in; in++) {
    if (*in >= '0' && *in <= '9') { if (n == 16) return false; out[n++] = *in; }
    else if (*in != '-' && *in != ' ' && *in != '.') return false;
  }
  out[n] = 0;
  return n == 16;
}

void sync_format_code(const char *code, char out[20]) {
  snprintf(out, 20, "%.4s-%.4s-%.4s-%.4s", code, code + 4, code + 8, code + 12);
}
