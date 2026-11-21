#include <SDL3/SDL_system.h>
#include "platform/companion.h"
#include "game/game.h"
#include "game/features.h"
#include "wide/item.h"
#include "wide/room.h"
#include <jni.h>
#include <stdatomic.h>
#include <string.h>
#include <pthread.h>

// Packet for CompanionScreen: slot count, six assigned IDs, then a record per
// owned item (ID, remaining count, 16x16 ARGB icon, equipment level). A/B plus 16 storage cells.
enum { LABELS = 9, HEADER = LABELS + HUD_SLOTS, RECORD = 259, CAPACITY = HEADER + 18 * RECORD };
static jint packet[CAPACITY];
static InventoryArt inventoryArt;
static bool hasInventoryArt;
static int length = HEADER, revision = 1;
static int requested_item, requested_slot, request_id, last_result;

void companion_set_enabled(bool enabled) {
  JNIEnv *env = SDL_GetAndroidJNIEnv();
  if (!env) return;
  jobject activity = SDL_GetAndroidActivity();
  if (!activity) return;
  jclass cls = (*env)->GetObjectClass(env, activity);
  jmethodID setter = (*env)->GetMethodID(env, cls, "setCompanionEnabled", "(Z)V");
  if (setter) (*env)->CallVoidMethod(env, activity, setter, (jboolean)enabled);
  (*env)->DeleteLocalRef(env, cls);
  (*env)->DeleteLocalRef(env, activity);
}
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

JNIEXPORT jint JNICALL Java_org_oracles_CompanionScreen_assign(JNIEnv *env, jclass cls, jint seen, jint slot, jint item) {
  (void)env; (void)cls;
  pthread_mutex_lock(&lock);
  int accepted = 0;
  if (seen == revision && slot >= 0 && slot < packet[0] && item > 0 && item < 32) {
    request_id = request_id == INT32_MAX ? 1 : request_id + 1;
    accepted = request_id;
    requested_item = item;
    requested_slot = slot;
  }
  pthread_mutex_unlock(&lock);
  atomic_store(&refresh, true);
  return accepted;
}

void companion_clear(void) {
  pthread_mutex_lock(&lock);
  last_result = 0;
  hasInventoryArt = false;
  memset(packet, 0, HEADER * sizeof(jint));
  length = HEADER;
  atomic_store(&guideGame, 0);
  mapStatus = -2;
  mapRevision++;
  revision++;
  requested_item = 0;
  pthread_mutex_unlock(&lock);
}

// Item levels distinguish upgrades that share an inventory ID and slot.
static int companion_item_level(const uint8_t *page, bool seasons, int id) {
  switch (id) {
    case 1: return page[seasons ? 0xa9 : 0xaf]; // shield
    case 5: return page[seasons ? 0xac : 0xb2]; // sword
    case 6: return seasons ? page[0xb1] : 1;    // boomerang
    case 10: return seasons ? 1 : page[0xb6];   // switch hook
    case 14: return page[seasons ? 0xaf : 0xb5]; // flute
    case 19: return seasons ? page[0xb3] : 1;   // slingshot
    case 22: return seasons ? 1 : page[0xb8];   // bracelet
    case 23: return seasons ? page[0xb4] : 1;   // feather
    default: return 0;
  }
}

bool companion_update(GB *gb, bool seasons, bool playing, const uint8_t glyphs[HUD_SLOTS]) {
  bool inventory_requested = atomic_exchange(&refresh, false);
  bool map_requested = atomic_exchange(&refreshMap, false);
  bool loaded = !memcmp(&gb->wram[0][0x5b2], seasons ? "Z11216-0" : "Z21216-0", 8);
  atomic_store(&guideGame, loaded ? (seasons ? 2 : 1) : 0);
  if (map_requested) update_map(gb, seasons, loaded);
  if (!inventory_requested) return false;
  pthread_mutex_lock(&lock);
  int slot = requested_slot, item = requested_item, ticket = request_id;
  requested_item = 0;
  pthread_mutex_unlock(&lock);
  bool can_equip = playing && loaded && !gb->wram[0][0xc04] && features_touch_available(gb);
  bool assigned = item && can_equip && features_touch_equip(gb, slot, item);
  jint next[CAPACITY] = {0};
  if (item) last_result = assigned ? ticket : -ticket;
  next[7] = last_result;
  int n = HEADER;
  if (loaded) {
    // A negative slot count means the current menu or cutscene blocks equipping.
    next[0] = (2 + features.extra_slots) * (can_equip ? 1 : -1);
    static const HudSlot companion_order[] = {HUD_A, HUD_B, HUD_X, HUD_Y, HUD_L, HUD_R};
    for (int i = 0; i < HUD_SLOTS; i++) next[LABELS+i] = glyphs[companion_order[i]];
    next[1] = features_hud_item(gb, wInventoryA);
    next[2] = features_hud_item(gb, wInventoryB);
    for (int i = 0; i < 4; i++) next[3+i] = features.slot_item[i];
    // With full storage, Biggoron's Sword cannot displace two occupied hands.
    next[8] = !next[1] || !next[2] || next[1] == 0x0c || next[2] == 0x0c ||
        memchr(&gb->wram[0][wInventoryStorage - 0xc000], 0, 16) != NULL;
    if (!hasInventoryArt) {
      inventory_load_art(gb->rom, gb->rom_size, seasons, &inventoryArt);
      hasInventoryArt = true;
    }
    bool present[32] = {false};
    for (int i = 0; i < 18; i++) {
      int id = i < 2 ? next[i+1] : mem_rd(gb, wInventoryStorage+i-2);
      if (!id || id >= 32 || present[id]) continue;
      present[id] = true;
      next[n] = id;
      next[n+1] = wide_item_count(seasons, id, &gb->wram[0][0x600]);
      uint32_t pixels[256] = {0};
      inventory_item_icon(gb->rom, gb->rom_size, seasons, id, &gb->wram[0][0x600], &inventoryArt, pixels);
      memcpy(next + n + 2, pixels, sizeof pixels);
      next[n + 258] = companion_item_level(&gb->wram[0][0x600], seasons, id);
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
