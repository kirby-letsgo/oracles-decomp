#pragma once
// What one sync does with each file, decided from three views of it: the file on this device, the
// server's current version (its manifest), and what this device last synced (its sync record).
// Pure logic: the app does the network and the files.
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define SYNC_MAX_FILES 32

typedef struct {
  char game[8], name[24];
  int version;
  char sha[65];
  int format;                   // save states: the state format; -1 when the server has none
  int64_t updated;              // unix seconds
  char device[64];
} SyncRemote;

typedef struct {
  char game[8], name[24];
  int version;
  char sha[65];
} SyncRecord;

typedef enum { SYNC_KEEP, SYNC_UPLOAD, SYNC_DOWNLOAD, SYNC_ASK } SyncAction;

typedef struct {
  bool local;                   // the file exists on this device
  char local_sha[65];
  int64_t local_mtime;          // unix seconds
  const SyncRemote *remote;     // NULL: the server has no version
  const SyncRecord *last;       // NULL: never synced on this device
  int local_format;             // save states: this build's state format
  bool is_state;                // a save state: a download needs the same format
  bool ask_on_conflict;         // saves and states ask the player; settings take the newer side
} SyncInput;

SyncAction sync_decide(const SyncInput *in);

// The server's plain-text manifest ("game name version sha format|- updated device")
int sync_parse_manifest(const char *text, SyncRemote *out, int max);
const SyncRemote *sync_find_remote(const SyncRemote *r, int n, const char *game, const char *name);

// The sync record ("game name version sha" per line)
int sync_parse_records(const char *text, SyncRecord *out, int max);
int sync_format_records(const SyncRecord *r, int n, char *out, size_t size);
const SyncRecord *sync_find_record(const SyncRecord *r, int n, const char *game, const char *name);
// Records that game/name is now at version/sha on both sides; returns the new count.
int sync_set_record(SyncRecord *r, int n, int max, const char *game, const char *name, int version, const char *sha);

// "4827-1930-5561-0284", "4827 1930 5561 0284" -> "4827193055610284"; false unless 16 digits
bool sync_normalize_code(const char *in, char out[17]);
void sync_format_code(const char *code, char out[20]);
