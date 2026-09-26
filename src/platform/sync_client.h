#pragma once
// Save sync with the sync server (sync-server/): the files of the app folder that a player would
// miss on another device, per game (sram.sav, save-state slots with their thumbnails,
// item_buttons) plus the shared settings.ini. Files only: the game never runs while its files sync.
#include "sync/sync.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct {
  char url[256];
  char code[17];                // 16 digits; "" when sync is off
} SyncConfig;

// sync.ini in the app folder: this device's server and code (not synced itself).
void sync_config_load(SyncConfig *c, const char *cache);
void sync_config_store(const SyncConfig *c, const char *cache);

// A file changed both here and on the server since this device last synced: the player picks.
typedef struct {
  char game[8], name[24];
  SyncRemote remote;
  bool local;
  int64_t local_mtime;
} SyncChoice;

#define SYNC_MAX_CHOICES 8

typedef struct {
  bool ok;                      // the server answered
  int uploaded, downloaded;
  int choices;
  SyncChoice choice[SYNC_MAX_CHOICES];
  bool settings_changed;        // settings.ini came from the server
  char error[128];
} SyncResult;

// Blocking. game: "ages", "seasons" or SYNC_ALL_GAMES; NULL skips it (the running game) and syncs
// only the shared settings, which always come along.
#define SYNC_ALL_GAMES "*"
void sync_run(const SyncConfig *c, const char *cache, const char *game, SyncResult *r);
// The server's current bytes of a file, for showing a choice (malloc'd; NULL when missing).
uint8_t *sync_fetch(const SyncConfig *c, const char *game, const char *name, size_t *size);
// keep_local uploads this device's file over the server's, else the server's replaces it.
bool sync_resolve(const SyncConfig *c, const char *cache, const SyncChoice *ch, bool keep_local);
bool sync_create_account(const char *url, char code[17], char *error, size_t size);

// One sync at a time on a thread; sync_poll hands its result over once.
bool sync_start(const SyncConfig *c, const char *cache, const char *game);
bool sync_busy(void);
bool sync_poll(SyncResult *out);
// Waits up to timeout_ms for a running sync; false if it is still going.
bool sync_wait(int timeout_ms);
