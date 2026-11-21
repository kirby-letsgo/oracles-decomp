#include "platform/companion.h"
#include "game/game.h"
#include "game/features.h"
#include "wide/item.h"
#include "wide/room.h"
#include <jni.h>
#include <stdatomic.h>
#include <string.h>
#include <pthread.h>

// Packet for CompanionScreen: slot count, four assigned IDs, then a record per
// owned item (ID, remaining count, 16x16 ARGB icon). A/B plus 16 storage cells.
enum { HEADER = 5, RECORD = 258, CAPACITY = HEADER + 18 * RECORD };
static jint packet[CAPACITY];
static int length = HEADER, revision = 1;
static int requested_item, requested_slot;
static atomic_bool refresh;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// Header fields match the MAP_* indices in CompanionScreen. Dungeon floors follow consecutively.
static atomic_bool refreshMap;
static atomic_int guideGame; // 0 before file load, 1 for Ages, 2 for Seasons.
static int mapRevision = 1, mapStatus = -2;
static OverworldMap lastMap;
static DungeonMap lastDungeon;
static uint32_t mapPixels[DUNGEON_MAX_FLOORS * DUNGEON_FLOOR_PIXELS];

JNIEXPORT jint JNICALL Java_org_oracles_CompanionScreen_guideGame(JNIEnv *env, jclass cls) {
  (void)env; (void)cls;
  return atomic_load(&guideGame);
}

JNIEXPORT jintArray JNICALL Java_org_oracles_CompanionScreen_worldMap(JNIEnv *env, jclass cls, jint seen) {
  (void)cls;
  atomic_store(&refreshMap, true);
  pthread_mutex_lock(&lock);
  jintArray result = NULL;
  if (seen != mapRevision) {
    int width = mapStatus < 0 ? 0 : overworld_map_width(&lastMap);
    int height = mapStatus < 0 ? 0 : overworld_map_height(&lastMap);
    bool dungeon = mapStatus == 2;
    if (dungeon) width = height = 64;
    int count = width * height * (dungeon ? lastDungeon.floors : 1);
    jint header[] = {mapRevision, mapStatus, dungeon ? lastDungeon.link_cell : lastMap.room,
                     lastMap.seasons, width, height, lastDungeon.index, lastDungeon.link_floor,
                     lastDungeon.base_floor, lastDungeon.visible, lastDungeon.floors};
    int fields = sizeof header / sizeof header[0];
    result = (*env)->NewIntArray(env, count + fields);
    if (result) {
      (*env)->SetIntArrayRegion(env, result, 0, fields, header);
      if (count) (*env)->SetIntArrayRegion(env, result, fields, count, (const jint *)mapPixels);
    }
  }
  pthread_mutex_unlock(&lock);
  return result;
}

static void update_map(GB *gb, bool seasons, bool loaded) {
  OverworldMap next = {0};
  if (loaded) overworld_map_state(seasons, gb->wram[0], &next);
  int status = !loaded ? -2 : next.plane;
  DungeonMap dungeon = {0};
  if (status == 2) dungeon_map_state(seasons, gb->rom, gb->rom_size, gb->wram[0], gb->wram[2], &dungeon);
  pthread_mutex_lock(&lock);
  if (memcmp(&dungeon, &lastDungeon, sizeof dungeon) || status != mapStatus || memcmp(&next, &lastMap, sizeof next)) {
    if (status == 0 || status == 1)
      overworld_map_pixels(gb->rom, gb->rom_size, &next, mapPixels);
    else if (status == 2)
      dungeon_map_pixels(seasons, gb->rom, gb->rom_size, &dungeon, mapPixels);
    lastDungeon = dungeon;
    mapStatus = status;
    lastMap = next;
    mapRevision++;
  }
  pthread_mutex_unlock(&lock);
}

JNIEXPORT jintArray JNICALL Java_org_oracles_CompanionScreen_inventory(JNIEnv *env, jclass cls, jint seen) {
  (void)cls;
  atomic_store(&refresh, true);
  pthread_mutex_lock(&lock);
  jintArray result = NULL;
  if (seen != revision) {
    result = (*env)->NewIntArray(env, length + 1);
    if (result) {
      (*env)->SetIntArrayRegion(env, result, 0, 1, &revision);
      (*env)->SetIntArrayRegion(env, result, 1, length, packet);
    }
  }
  pthread_mutex_unlock(&lock);
  return result;
}

JNIEXPORT void JNICALL Java_org_oracles_CompanionScreen_assign(JNIEnv *env, jclass cls, jint seen, jint slot, jint item) {
  (void)env; (void)cls;
  pthread_mutex_lock(&lock);
  if (seen == revision && slot >= 0 && slot < packet[0] && item > 0 && item < 32) {
    requested_item = item;
    requested_slot = slot;
  }
  pthread_mutex_unlock(&lock);
  atomic_store(&refresh, true);
}

void companion_clear(void) {
  pthread_mutex_lock(&lock);
  memset(packet, 0, HEADER * sizeof(jint));
  length = HEADER;
  atomic_store(&guideGame, 0);
  mapStatus = -2;
  mapRevision++;
  revision++;
  requested_item = 0;
  pthread_mutex_unlock(&lock);
}

bool companion_update(GB *gb, bool seasons, bool playing) {
  bool inventory_requested = atomic_exchange(&refresh, false);
  bool map_requested = atomic_exchange(&refreshMap, false);
  bool loaded = !memcmp(&gb->wram[0][0x5b2], seasons ? "Z11216-0" : "Z21216-0", 8);
  atomic_store(&guideGame, loaded ? (seasons ? 2 : 1) : 0);
  if (map_requested) update_map(gb, seasons, loaded);
  if (!inventory_requested) return false;
  pthread_mutex_lock(&lock);
  int slot = requested_slot, item = requested_item;
  requested_item = 0;
  pthread_mutex_unlock(&lock);
  bool can_equip = playing && loaded && !gb->wram[0][0xc04] && features_touch_available(gb);
  bool assigned = item && can_equip && features_touch_equip(gb, slot, item);
  jint next[CAPACITY] = {0};
  int n = HEADER;
  if (loaded) {
    // A negative slot count means the current menu or cutscene blocks equipping.
    next[0] = (features.four_slots ? 4 : 2) * (can_equip ? 1 : -1);
    next[1] = features_hud_item(gb, wInventoryA);
    next[2] = features_hud_item(gb, wInventoryB);
    next[3] = features.slot_item[0];
    next[4] = features.slot_item[1];
    bool present[32] = {false};
    for (int i = 0; i < 18; i++) {
      int id = i < 2 ? next[i+1] : mem_rd(gb, wInventoryStorage+i-2);
      if (!id || id >= 32 || present[id]) continue;
      present[id] = true;
      next[n] = id;
      next[n+1] = wide_item_count(seasons, id, &gb->wram[0][0x600]);
      WideItemIcon icon;
      if (wide_item_icon(gb->rom, gb->rom_size, seasons, id, &gb->wram[0][0x600], &icon)) {
        for (int y = 0; y < 16; y++) for (int x = 0; x < icon.halves*8; x++) {
          int c = icon.ci[y][x];
          if (!c) continue;
          int p = icon.pal[x/8]*8 + c*2;
          int rgb = gb->ob_pal[p] | gb->ob_pal[p+1]<<8;
          next[n+2+y*16+x+(icon.halves==1 ? 4 : 0)] = 0xff000000u |
              ((rgb & 31)*255/31)<<16 | (((rgb>>5)&31)*255/31)<<8 | ((rgb>>10)&31)*255/31;
        }
      }
      n += RECORD;
    }
  }
  pthread_mutex_lock(&lock);
  if (length != n || memcmp(next, packet, n*sizeof(jint))) {
    memcpy(packet, next, n*sizeof(jint));
    length = n;
    revision++;
  }
  pthread_mutex_unlock(&lock);
  return assigned && slot >= 2;
}
