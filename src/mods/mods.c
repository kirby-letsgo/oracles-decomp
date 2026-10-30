#include "mods/mods.h"
#include "mods/treasures.h"
#include "mods/api.h"
#include "mods/sandbox.h"
#include "lua.h"
#include "lauxlib.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
// For open_store_for_rewrite/mod_dir_is_safe_to_write below: mingw-w64 has neither O_NOFOLLOW,
// lstat, nor S_ISLNK (confirmed by trying to compile against it), so the POSIX half of each
// function uses those and the Windows half uses GetFileAttributesA's reparse-point bit instead.
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// id doubles as the folder name and, from Task 9 on, a path component for a mod's store file.
// This is the gate against NAME-based path traversal: no byte this rejects can spell ".." or
// "/" or "\", so a validated id is unrepresentable as a traversal via its characters, on either
// a POSIX host or the MinGW/Windows target, where "\" is also a separator. It says nothing about
// a mod *folder* that is itself a symlink pointing outside the mods directory -- that is a
// separate concern, owned by Task 9's zip installer and the per-mod store writer, not this check.
bool mods_id_valid(const char *id) {
  if (!id || !*id) return false;
  for (const unsigned char *p = (const unsigned char *)id; *p; p++) {
    bool ok = (*p >= 'a' && *p <= 'z') || (*p >= '0' && *p <= '9') || *p == '-';
    if (!ok) return false;
  }
  return true;
}

#define MOD_FIELD_LEN 64

// How much of a failure reason mods_error() keeps. Deliberately short: this is for a one-line
// status display, not a full diagnostic -- the unabridged text (up to 512 bytes, see `reason` in
// run_main_lua) always also reaches stderr at the moment a mod is disabled, which is where a
// developer debugging a specific mod should look. Sized with headroom above the longest fixed
// message this file produces today (the "...via a nested coroutine..." budget/deadline wording,
// ~80 bytes) so none of those truncate, while still being far short of the 512-byte buffer a
// hostile main.lua's own error() text is escaped into before being cut down to this length --
// mods_escape_into's \xHH escaping already makes that truncation byte-safe (every escape sequence
// is plain ASCII, so cutting it short mid-sequence yields a shorter but still printable string,
// never a broken one).
#define MOD_ERROR_LEN 128

typedef struct {
  char id[MOD_FIELD_LEN];     // canonical: the validated folder name, never the mod.ini field directly
  char name[MOD_FIELD_LEN];
  char description[256], author[64], version[64];
  char ini_id[MOD_FIELD_LEN]; // raw "id=" value from mod.ini (escaped; see mods_escape_into), kept
                              // only to cross-check against id
  bool ini_id_present;
  bool enabled;         // Set true only once run_main_lua (or the api-too-new check in mods_init)
                        // actually succeeds for this mod. A mod that is accepted as applying to
                        // this game but whose own payload fails is still stored in mods[] with
                        // this false -- see mods_init's own comment for exactly where that line
                        // sits -- never silently dropped the way a mod that isn't ours at all is.
  char error[MOD_ERROR_LEN]; // "" while enabled is true; the escaped, truncated reason otherwise.
                             // Populated at the same place enabled is set to false, never earlier.
  bool ages, seasons;
  bool changes_game;    // "changes_game=yes": with api >= 2, may swap treasures and write WRAM
  bool api_present;     // true iff an "api=" line appeared at all
  bool api_parsed;      // true iff that value parsed as a well-defined int (meaningless unless
                        // api_present)
  int api;              // the parsed value iff api_parsed; otherwise unspecified. Whether it's an
                        // *acceptable* value is split across two different callers, not read_ini's
                        // call: load_one checks >= 1 (a malformed value, refused outright); the
                        // mods_init loop checks <= MODS_API_VERSION (a well-formed value this
                        // build is simply too old for, listed disabled rather than refused) --
                        // see the api= branch below for why that split exists.
  lua_State *L;         // Non-NULL exactly when `enabled` is true. A mod stored in mods[] with
                        // enabled false (api too new, run_main_lua failed, or -- Task 8 -- a
                        // frame handler errored or exhausted its per-frame budget) always has
                        // L == NULL: whichever path disables the mod closes and frees its own
                        // Lua state itself, in the same place it sets enabled = false, before
                        // ever leaving that invariant visible to anything else -- see
                        // mods_shutdown's own comment for why that invariant is what keeps it
                        // from double-closing, and call_handler below for the frame-time case.
  int on_frame_ref;     // A registry reference (luaL_ref(L, LUA_REGISTRYINDEX)) to this mod's
  int on_room_change_ref; // on_frame/on_room_change handler, or LUA_NOREF if never registered.
                        // Deliberately not a Lua global (`__on_frame`): a global the host sets
                        // from C is also a global a mod could set directly, bypassing
                        // oracles.on_frame/oracles.on_room_change entirely -- see
                        // mods_set_on_frame_ref's own comment in mods.h. Meaningless (and never
                        // read) once L is NULL; the ref itself dies with L at lua_close, nothing
                        // here needs to luaL_unref it on that path.
} Mod;

static Mod mods[MODS_MAX];
static int count;
// Set by mods_init, read by api.c's oracles.game() by way of mods_current_frame() -- not read
// directly here otherwise.
static bool host_seasons;

// The overlay every enabled mod's draw_rect/draw_text/draw_icon mutate, and the frame currently
// being served to a mod's Lua -- both held here (not static locals inside mods_frame) because
// api.c's bindings need to reach them from wherever a mod's C call lands, which is never on the
// same call stack as mods_frame itself. `frame` starts at a sane pre-first-frame default (width
// 160, seasons from whatever game was last initialised) rather than all-zero, so a mod calling
// oracles.screen()/oracles.game() from main.lua's own top-level body -- before mods_frame has
// ever run -- gets a real answer instead of width 0 (a lie no mod author would expect) or
// seasons false (which could just as easily be wrong as right); mods_init sets both explicitly
// from its own `seasons` argument, which is the one place that actually knows.
static ModOverlay overlay;
static ModsFrame frame = { .width = 160 };
// The room mods_frame last saw, so on_room_change fires only on an actual change -- -1 (no real
// room) rather than 0 so that the very first mods_frame call after a fresh mods_init always
// counts as "changed", whatever room number that first call reports. Reset by mods_shutdown (and
// so, transitively, by the mods_init that always calls it first): without that, a stale value
// left over from a previous game/mod_init could make the very first frame of a new session wrongly
// look like "no change" if the new session's first room happens to match the old one's last --
// the same stale-state class as last_log (Task 5) and the `active` index Task 9's own sketch
// left unreset; this is the third instance, closed here rather than left for a fourth.
static int last_room = -1;
// The mod whose Lua is currently executing -- main.lua's own top-level body (run_main_lua), or a
// frame handler (call_handler below) -- so oracles.on_frame/oracles.on_room_change can register
// onto *that* mod's own struct without a Lua global naming it. Set immediately around every
// point this file calls into a mod's Lua and reset to NULL immediately after, on every path,
// including every failure path: an "active" pointer set-but-never-reset is exactly the staleness
// class the comment on `last_room` above just finished describing, so this one is built reset
// from the start rather than retrofitted later the way Task 9's own sketch needed to be.
static Mod *active_mod;

void mods_set_on_frame_ref(lua_State *L, int ref) {
  if (!active_mod) return;      // no mod's Lua is running: should not happen (a mod has no other
                                 // way to call a C function), handled defensively regardless.
  // `L`, not active_mod->L: during main.lua's own top-level body (run_main_lua), active_mod->L
  // is still NULL -- it is only assigned after this whole call has returned -- while `L` (the
  // state the binding was actually called on, here run_main_lua's `runner` coroutine) is always
  // live, and any thread of the same global state is equally valid for a registry unref.
  if (active_mod->on_frame_ref != LUA_NOREF)
    luaL_unref(L, LUA_REGISTRYINDEX, active_mod->on_frame_ref);
  active_mod->on_frame_ref = ref;
}

void mods_set_on_room_change_ref(lua_State *L, int ref) {
  if (!active_mod) return;
  if (active_mod->on_room_change_ref != LUA_NOREF)
    luaL_unref(L, LUA_REGISTRYINDEX, active_mod->on_room_change_ref);
  active_mod->on_room_change_ref = ref;
}

// The swap table belongs to one mod, named by id: main.lua runs against a copy of its Mod that is
// only stored in mods[] afterwards, so a pointer would not outlive the load.
typedef struct { uint8_t group, room, id, subid, new_id, new_subid; } Swap;
static Swap swaps[MODS_SWAPS_MAX];
static int swap_count;
static char swap_owner[MOD_FIELD_LEN];

static bool may_change_game(void) { return active_mod && active_mod->api >= 2 && active_mod->changes_game; }

// A switched-off mod has no effect on the game any more.
static void drop_effects(const char *id) {
  if (!strcmp(swap_owner, id)) { swap_count = 0; swap_owner[0] = 0; }
}

const char *mods_add_swap(int group, int room, int id, int subid, int new_id, int new_subid) {
  if (!may_change_game()) return "needs api=2 and changes_game=yes in mod.ini";
  if (swap_owner[0] && strcmp(swap_owner, active_mod->id)) return "another mod already swaps treasures";
  if (group < 0 || group > 7 || room < 0 || room > 255 || id < 0 || id > 255 || subid < 0 || subid > 255)
    return "group must be 0-7 and room, id and subid 0-255";
  if (!mods_treasure_valid(host_seasons, new_id, new_subid)) return "the new treasure is not one of this game's";
  snprintf(swap_owner, sizeof swap_owner, "%s", active_mod->id);
  Swap s = {(uint8_t)group, (uint8_t)room, (uint8_t)id, (uint8_t)subid, (uint8_t)new_id, (uint8_t)new_subid};
  for (int i = 0; i < swap_count; i++)
    if (swaps[i].group == s.group && swaps[i].room == s.room && swaps[i].id == s.id && swaps[i].subid == s.subid) {
      swaps[i] = s;
      return NULL;
    }
  if (swap_count == MODS_SWAPS_MAX) return "the swap table is full (512 entries)";
  swaps[swap_count++] = s;
  return NULL;
}

static ModWrite writes[MODS_WRITES_MAX];
static int write_count;
static bool in_handler;

int mods_active_api(void) { return active_mod ? active_mod->api : 0; }

const char *mods_queue_write(int addr, int value) {
  if (!may_change_game()) return "needs api=2 and changes_game=yes in mod.ini";
  if (!in_handler) return "only inside on_frame or on_room_change";
  if (addr < 0xc000 || addr > 0xdfff) return "the address is outside $c000-$dfff";
  if (addr >= 0xc0b0 && addr <= 0xc2ff) return "$c0b0-$c2ff holds the stacks and thread state";
  if (value < 0 || value > 255) return "the value is outside 0-255";
  if (write_count == MODS_WRITES_MAX) return "more than 256 writes this frame";
  writes[write_count++] = (ModWrite){(uint16_t)addr, (uint8_t)value};
  return NULL;
}

int mods_take_writes(ModWrite *out, int cap) {
  int n = write_count < cap ? write_count : cap;
  memcpy(out, writes, (size_t)n * sizeof *out);
  write_count = 0;
  return n;
}

bool mods_treasure_swap(int group, int room, uint8_t id, uint8_t subid, uint8_t *new_id, uint8_t *new_subid) {
  for (int i = 0; i < swap_count; i++)
    if (swaps[i].group == group && swaps[i].room == room && swaps[i].id == id && swaps[i].subid == subid) {
      *new_id = swaps[i].new_id;
      *new_subid = swaps[i].new_subid;
      return true;
    }
  return false;
}

const ModOverlay *mods_overlay(void) { return &overlay; }
ModOverlay *mods_overlay_mut(void) { return &overlay; }
const ModsFrame *mods_current_frame(void) { return &frame; }

// The per-frame budget mod_resume_budgeted (sandbox.h) enforces on every single on_frame/
// on_room_change call -- deliberately tiny next to MOD_LOAD_INSTRUCTION_BUDGET/
// MOD_LOAD_WALLCLOCK_DEADLINE_SECONDS (sandbox.h), which only ever run once per mod per session.
// A handler runs every frame, so calling it unbudgeted (a plain lua_pcall, with neither an
// instruction count nor a wall-clock cutoff -- what this task's own original sketch did) hangs
// the game on its very first frame and every one after: the single most important thing this
// file does differently from that sketch, not a refinement of it.
//
// Measured on this dev machine with mod_resume_budgeted itself, not guessed: a tight
// `while true do end` loop trips the instruction-count hook's first check-in (sandbox.c's
// MOD_INSTRUCTION_CHECKIN -- a constant mod_resume_budgeted does not expose as a per-call
// parameter, shared as-is with the load-time path) in ~2.8-3.9ms; `while true do
// pcall(function() error('x') end) end` -- Lua's own error-raise-and-catch machinery, 50-150x
// more expensive per VM instruction than plain arithmetic, the same shape sandbox.c's own
// MOD_LOAD_INSTRUCTION_BUDGET comment documents at load scale -- took ~60-67ms to reach that
// same first check-in when MOD_INSTRUCTION_CHECKIN was still 1,000,000 (this repo's own
// benchmark and an independent reviewer re-measurement agreed on that figure to within noise).
// A frame is 16.7ms, so 60-67ms was 15-17x the 4ms deadline below -- not because the deadline
// value was wrong, but because the wall-clock deadline is only ever consulted when the
// instruction-count hook actually fires, and at the time that firing interval was coarser than
// the deadline itself.
//
// That gap is closed in sandbox.c, not here: MOD_INSTRUCTION_CHECKIN was lowered from 1,000,000
// to 10,000 (see its own comment in sandbox.c for the full measurement table across candidate
// values and confirmation that legitimate load-path scripts show no measurable cost from
// checking in 100x more often). With that change, the same adversarial handler above is now
// caught at a measured ~4.2ms median (9 trials) -- within ~5% of this deadline, not 15-17x over
// it -- while the cheap-loop case is unchanged (~2.8-3.9ms, the same run-to-run noise band as
// before). Reusing mod_resume_budgeted exactly as sandbox.h says it was built to be reused, with
// its own check-in constant doing the work, rather than widening mod_resume_budgeted's own
// contract to let this one caller shrink that interval itself, was the deliberate choice both
// times this was considered: the constant is shared with (and sized for) the load path too, and
// changing it there was simpler, better-tested by the full existing hostile-fixture suite, and
// leaves mod_resume_budgeted's signature exactly as documented. See
// a_pathologically_expensive_on_frame_handler_is_poisoned_after_one_bounded_stall in
// tests/test_mods.c, which pins "bounded and one-time" rather than a literal millisecond figure,
// since the figure is host-speed-dependent (now ~4-5ms on this machine, not ~60-67ms).
#define MOD_FRAME_INSTRUCTION_BUDGET 1000000
// "Low single-digit milliseconds", per the task this implements: large enough that, for the
// common cheap-loop case, the instruction-count mechanism above -- not this -- is what actually
// reports the failure (measured at ~2.8-3.9ms, comfortably under this), while still being the
// backstop for a handler whose own instructions are legitimately, not adversarially, somewhat
// more expensive than bare arithmetic (table indexing, a log line's string concatenation)
// without yet being the deliberately pathological shape the comment above measures. Now actually
// enforceable to within ~5% of this value for that pathological shape too, once
// MOD_INSTRUCTION_CHECKIN (sandbox.c) was lowered to make the hook check it often enough.
#define MOD_FRAME_WALLCLOCK_DEADLINE_SECONDS 0.004

// Calls mod `m`'s handler stored at registry ref `ref` (a no-op if `ref` is LUA_NOREF, i.e.
// never registered, or `m->L` is already NULL, i.e. already disabled), passing `arg` as its one
// argument iff `has_arg`. Every call goes through mod_resume_budgeted under the frame budget
// above -- never a plain lua_pcall -- and on a fresh thread made for this call alone:
// mod_resume_budgeted's own doc comment is explicit that a thread it has already driven to
// exhaustion cannot simply be lua_resume'd again (the next resume would continue the old,
// already-failed call where it left off, not start a new one), so reusing one thread across
// frames is not an option without also calling lua_resetthread on every healthy return, and a
// fresh lua_newthread each call is simpler and just as cheap -- Lua threads are small, and the
// old one, once popped off m->L's stack below, is ordinary garbage the collector reclaims like
// anything else. (Popping it matters beyond tidiness: lua_newthread pushes the new thread onto
// *m->L*'s stack, not the new thread's own -- sandbox.h's own comment notes run_main_lua's single
// load-time thread is deliberately left unpopped because that only ever happens once per mod per
// session; this function is called every frame, so leaving each one on m->L's stack would grow
// that stack by one slot forever.)
//
// A handler that errors, or whose call exhausts the frame budget, disables `m` right here --
// enabled = false, error set, L closed and freed -- the same treatment run_main_lua gives a
// load-time failure, so mods_enabled()/mods_error() need no separate frame-time case to check.
// Without this, mod_resume_budgeted's own doc comment is explicit about what would happen
// instead: a mod whose handler loops forever would simply be handed a fresh budget next frame,
// forever -- not a disabled mod, a permanently wedged game.
static void call_handler(Mod *m, int ref, bool has_arg, int arg) {
  if (!m->L || ref == LUA_NOREF) return;
  active_mod = m;
  lua_State *runner = lua_newthread(m->L);
  lua_rawgeti(runner, LUA_REGISTRYINDEX, ref);
  int argc = 0;
  if (has_arg) { lua_pushinteger(runner, arg); argc = 1; }
  bool exhausted = false, deadline_hit = false;
  int staged = write_count;
  in_handler = true;
  int status = mod_resume_budgeted(runner, argc, MOD_FRAME_INSTRUCTION_BUDGET,
                                    MOD_FRAME_WALLCLOCK_DEADLINE_SECONDS, &exhausted, &deadline_hit);
  in_handler = false;
  if (status != LUA_OK || exhausted) {
    char reason[256];
    if (status == LUA_YIELD || (status == LUA_OK && exhausted)) {
      // The LUA_OK-but-exhausted branch mirrors run_main_lua's own "absorbed by a nested
      // coroutine" case -- currently unreachable with "coroutine" withheld from mods entirely,
      // kept as the same defence-in-depth tripwire run_main_lua keeps for the identical shape.
      if (deadline_hit)
        snprintf(reason, sizeof reason,
            "a frame handler ran for over %.0fms and was cut off", MOD_FRAME_WALLCLOCK_DEADLINE_SECONDS * 1000.0);
      else
        snprintf(reason, sizeof reason,
            "a frame handler exceeded its per-frame instruction budget (possible infinite loop)");
    } else {
      size_t elen = 0;
      const char *emsg = lua_tolstring(runner, -1, &elen);
      if (!emsg) { emsg = "(non-string error object)"; elen = strlen(emsg); }
      char escaped[200];
      mods_escape_into(escaped, sizeof escaped, emsg, elen);
      snprintf(reason, sizeof reason, "a frame handler errored: %s", escaped);
    }
    fprintf(stderr, "mods: '%s' %s; disabling it for this session\n", m->id, reason);
    // Deliberately truncating: `reason` is the full stderr text, m->error is the short
    // status-line copy. The explicit precision says so to the compiler, which otherwise
    // warns about the truncation on the MinGW target and buries real warnings in noise.
    snprintf(m->error, sizeof m->error, "%.*s", (int)(sizeof m->error - 1), reason);
    m->enabled = false;
    drop_effects(m->id);
    write_count = staged;
    void *ud = NULL;
    lua_getallocf(m->L, &ud);
    lua_close(m->L);
    free(ud);
    m->L = NULL;
  } else {
    lua_pop(m->L, 1);          // drop `runner`: see this function's own comment on why it must
                                // not be left to accumulate on m->L's stack every frame.
  }
  active_mod = NULL;
}

// Runs every enabled mod's on_frame handler, then (iff f->room has changed since the last call)
// its on_room_change handler, leaving the overlay refilled for both to have drawn into. Guards
// `f` itself: the launcher may call this before any game is running, and mods_frame dereferencing
// a NULL `f` would be exactly the same class of bug l_rd/l_draw_icon below guard against for
// f->wram/f->rom -- a NULL that only shows up once no ROM is loaded, easy to never hit in a
// quick manual test.
void mods_frame(const ModsFrame *f) {
  if (!f) return;
  frame = *f;
  overlay_begin(&overlay, f->width);
  bool room_changed = f->room != last_room;
  last_room = f->room;
  write_count = 0;
  for (int i = 0; i < count; i++) {
    if (!mods[i].enabled) continue;
    call_handler(&mods[i], mods[i].on_frame_ref, false, 0);
    // Re-checked, not assumed still true: call_handler may have just disabled this mod (and
    // closed its L) for the on_frame call above, in which case on_room_change must not also run
    // against an already-freed state -- call_handler's own `!m->L` guard would catch it too, but
    // checking here as well keeps the skip visible at the call site rather than relying solely on
    // a defensive check buried inside the callee.
    if (room_changed && mods[i].enabled) call_handler(&mods[i], mods[i].on_room_change_ref, true, f->room);
  }
}

static void trim(char *s) {
  char *start = s;
  while (*start == ' ' || *start == '\t') start++;
  size_t len = strlen(start);
  while (len > 0 && (start[len - 1] == ' ' || start[len - 1] == '\t')) len--;
  start[len] = 0;
  if (start != s) memmove(s, start, len + 1);
}

// See mods.h for why this exists and why it is shared with api.c. Takes an explicit src_len
// (rather than relying on strlen) so a hostile value containing an embedded NUL -- a mod.ini
// value can't, since contains_nul already refuses those, but a Lua string from oracles.log() or
// a failed main.lua's error text can -- is escaped in full rather than silently cut short at the
// first NUL byte.
void mods_escape_into(char *dst, size_t dst_size, const char *src, size_t src_len) {
  size_t o = 0;
  for (size_t i = 0; i < src_len && o + 1 < dst_size; i++) {
    unsigned char c = (unsigned char)src[i];
    if (c >= 0x20 && c < 0x7f) {
      dst[o++] = (char)c;
    } else {
      char esc[5];
      snprintf(esc, sizeof esc, "\\x%02x", c);
      for (int j = 0; esc[j] && o + 1 < dst_size; j++) dst[o++] = esc[j];
    }
  }
  dst[o] = 0;
}

// Reads one logical line into `buf` (size n), discarding anything past the buffer on an
// over-long line rather than leaving the tail for the next call to misparse as its own
// key=value pair. Returns false at EOF with nothing read. Relies on the caller having already
// refused any mod.ini containing an embedded NUL (see contains_nul below): strlen(buf) stops at
// the first NUL, which would otherwise silently defeat the "buffer is exactly full" check this
// function uses to detect an over-long line, letting a NUL-padded tail smuggle a fake key=value
// past the discard.
static bool read_logical_line(FILE *f, char *buf, size_t n) {
  if (!fgets(buf, (int)n, f)) return false;
  size_t len = strlen(buf);
  if (len == n - 1 && buf[len - 1] != '\n') {
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {}
  }
  while (len > 0 && (buf[len - 1] == '\n' || buf[len - 1] == '\r')) buf[--len] = 0;
  return true;
}

// Scans the whole file for an embedded NUL byte, in bounded chunks so an attacker-sized mod.ini
// can't force an unbounded allocation. Leaves the file position wherever reading stopped --
// callers that go on to parse the file must rewind() first.
static bool contains_nul(FILE *f) {
  unsigned char buf[8192];
  size_t nread;
  while ((nread = fread(buf, 1, sizeof buf, f)) > 0)
    if (memchr(buf, 0, nread)) return true;
  return false;
}

// mod.ini is the same shape as settings.ini: key=value, '#' comments, blanks ignored, whitespace
// tolerated around both key and value. Unlike settings_parse's `%31[a-z_] = %255s` approach, this
// keeps interior spaces in the value, since mod names and authors contain them. Every value is
// treated as hostile: over-long, empty, or full of coincidental '=' or '#' characters are all
// handled, not just well-formed input. `label` is the mod's folder name, used only to name the
// mod in this function's own error messages and in a per-token "games=" warning.
static void parse_ini_line(char *line, const char *label, Mod *m) {
  char *p = line;
  while (*p == ' ' || *p == '\t') p++;
  // The whitespace-skip above means an indented '#' is already at `p` by the time this check
  // runs, so this is a readability nicety, not a safety boundary: `key` below always starts at
  // `p`, trim() never strips '#', so a comment's "key" (e.g. "# api" from "# api=99") can never
  // collide with a real keyword even if this exact check were deleted -- confirmed by mutation.
  // The actual safety net for both comments and blank lines is the `!eq` fallthrough just below.
  if (*p == '#' || *p == 0) return;

  char *eq = strchr(p, '=');
  if (!eq) return;
  *eq = 0;
  char *key = p, *value = eq + 1;
  trim(key);
  trim(value);
  if (!*key) return;

  if (!strcmp(key, "id")) {
    mods_escape_into(m->ini_id, sizeof m->ini_id, value, strlen(value));
    m->ini_id_present = true;
  } else if (!strcmp(key, "name")) {
    mods_escape_into(m->name, sizeof m->name, value, strlen(value));
  } else if (!strcmp(key, "description")) {
    mods_escape_into(m->description, sizeof m->description, value, strlen(value));
  } else if (!strcmp(key, "author")) {
    mods_escape_into(m->author, sizeof m->author, value, strlen(value));
  } else if (!strcmp(key, "version")) {
    mods_escape_into(m->version, sizeof m->version, value, strlen(value));
  } else if (!strcmp(key, "api")) {
    // This function's job is only to establish whether the (int) cast is well-defined and to
    // record that the key was present at all -- not whether the resulting number is an
    // *acceptable* api version. Folding that policy decision in here is what previously made
    // "api=0" (a well-formed number) report as "missing or non-numeric" (a syntax complaint),
    // which is simply false, and it hid the >= 1 policy bound behind the same sentinel as every
    // syntax error, destroying mutation signal on the most security-relevant bound in the file.
    // load_one owns the >= 1 policy check instead (api < 1 is a malformed declaration, refused
    // outright); mods_init owns the separate > MODS_API_VERSION case (a well-formed declaration
    // this build is simply too old for, which is listed disabled rather than refused -- see
    // load_one's own comment on its api= branch for why those two get different treatment).
    //
    // The errno ERANGE check matters beyond the obvious "huge decimal string" case: on an
    // LLP64 target (MinGW/Windows), `long` is only 32 bits, the same width as `int`, so strtol
    // silently clamps an overflowing value to LONG_MAX (== INT_MAX there) unless errno is
    // checked -- `v <= INT_MAX` alone can't tell "parsed exactly INT_MAX" apart from "overflowed
    // and got clamped to it", so api=2147483648 would be wrongly accepted on Windows while
    // correctly refused on a 64-bit-long host. Checking errno catches it on both.
    m->api_present = true;
    errno = 0;
    char *end;
    long v = strtol(value, &end, 10);
    bool consumed_all = end != value && *end == 0;
    m->api_parsed = consumed_all && errno != ERANGE && v >= INT_MIN && v <= INT_MAX;
    m->api = m->api_parsed ? (int)v : 0;
  } else if (!strcmp(key, "changes_game")) {
    m->changes_game = !strcmp(value, "yes");
  } else if (!strcmp(key, "games")) {
    m->ages = m->seasons = false;
    char buf[256];
    snprintf(buf, sizeof buf, "%s", value);
    for (char *tok = strtok(buf, ","); tok; tok = strtok(NULL, ",")) {
      trim(tok);
      if (!strcmp(tok, "ages")) m->ages = true;
      else if (!strcmp(tok, "seasons")) m->seasons = true;
      else if (*tok) {
        char esc[128];
        mods_escape_into(esc, sizeof esc, tok, strlen(tok));
        fprintf(stderr, "mods: '%s' names unrecognised game '%s' in its games=, ignoring that token\n",
                label, esc);
      }
    }
  }
}

static bool read_ini(const char *path, const char *label, Mod *m) {
  FILE *f = fopen(path, "rb");
  if (!f) {
    fprintf(stderr, "mods: '%s' has no mod.ini, skipping\n", label);
    return false;
  }
  if (contains_nul(f)) {
    fclose(f);
    fprintf(stderr, "mods: '%s/mod.ini' contains a NUL byte, refusing\n", label);
    return false;
  }
  rewind(f);

  char line[256];
  while (read_logical_line(f, line, sizeof line)) {
    parse_ini_line(line, label, m);
  }
  fclose(f);
  return true;
}

// ---------------------------------------------------------------------------------------------
// Task 9: settings rows.
// ---------------------------------------------------------------------------------------------

typedef struct {
  char name[32];              // escaped, mod-supplied label (see mods_setting_name's own comment)
  char mod_id[MOD_FIELD_LEN]; // the owning mod's canonical id -- NOT an index into mods[]. An
                              // index would be wrong half the time: a setting can be registered
                              // from main.lua's own top-level body, where active_mod points at
                              // mods_init's own stack-local `Mod m` (not yet copied into mods[]
                              // -- see run_main_lua's own comment on exactly this), so
                              // "active_mod - mods" would be pointer arithmetic on a pointer that
                              // may not point inside mods[] at all: undefined behaviour, not
                              // merely a wrong answer. `id` is stable and meaningful in both
                              // cases (already populated by load_one before run_main_lua ever
                              // runs), so it is what this is scoped by instead.
  ModSettingKind kind;
  bool bvalue;
  int ivalue, imin, imax;     // meaningful only when kind == MOD_SETTING_NUMBER
} ModSetting;

static ModSetting settings[MODS_MAX_SETTINGS];
static int setting_count;

int mods_setting_count(void) { return setting_count; }
const char *mods_setting_name(int i) { return i >= 0 && i < setting_count ? settings[i].name : ""; }
const char *mods_setting_mod_id(int i) { return i >= 0 && i < setting_count ? settings[i].mod_id : ""; }
ModSettingKind mods_setting_kind(int i) { return i >= 0 && i < setting_count ? settings[i].kind : MOD_SETTING_BOOL; }
bool mods_setting_value(int i) {
  return i >= 0 && i < setting_count && settings[i].kind == MOD_SETTING_BOOL && settings[i].bvalue;
}
int mods_setting_number(int i) {
  return i >= 0 && i < setting_count && settings[i].kind == MOD_SETTING_NUMBER ? settings[i].ivalue : 0;
}
int mods_setting_min(int i) { return i >= 0 && i < setting_count ? settings[i].imin : 0; }
int mods_setting_max(int i) { return i >= 0 && i < setting_count ? settings[i].imax : 0; }
void mods_setting_toggle(int i) {
  if (i >= 0 && i < setting_count && settings[i].kind == MOD_SETTING_BOOL) settings[i].bvalue = !settings[i].bvalue;
}

// Escapes `name` (exactly `name_len` bytes -- a Lua string may legally contain an embedded NUL,
// so this is never assumed NUL-terminated going in) into `dst`, REFUSING rather than truncating
// if the escaped form would not fit: the same truncation class Task 4 closed for a mod's folder
// id, where two different over-long ids colliding after 31 bytes let one mod silently masquerade
// as another. `name_len` itself is bounded well below dst_size's capacity before escaping is
// even attempted, since escaping can only grow a string (every non-printable byte becomes a
// 4-byte \xHH), so a name that's short enough raw but still too long once escaped is also
// refused here, not half-escaped into dst and left looking fine until something compares it.
static bool escape_setting_name(char *dst, size_t dst_size, const char *name, size_t name_len) {
  if (name_len == 0 || name_len >= dst_size) return false;
  char scratch[256];
  mods_escape_into(scratch, sizeof scratch, name, name_len);
  if (strlen(scratch) >= dst_size) return false;
  memcpy(dst, scratch, strlen(scratch) + 1);
  return true;
}

// Shared by mods_setting_add (BOOL) and mods_setting_add_number (NUMBER): looks a row up by
// (active mod's id, escaped name) -- not by name alone, which is the bug a pre-dispatch audit
// found in this task's own planning sketch: two different mods both calling
// oracles.setting("enabled", true) would otherwise share one row, so toggling one silently
// toggled the other. Idempotent re-registration (the same mod calling oracles.setting with the
// same name twice, e.g. because its own main.lua does so conditionally) returns the existing
// row's index unchanged, keeping whatever kind/value it already has rather than re-initialising
// it to this call's own arguments.
static int setting_add_common(const char *name, size_t name_len, ModSettingKind kind, bool bvalue,
                               int ivalue, int imin, int imax) {
  if (!active_mod) return -1;         // should not happen: see mods_active_id's own comment
  char escaped[32];
  if (!escape_setting_name(escaped, sizeof escaped, name, name_len)) {
    fprintf(stderr, "mods: '%s' tried to register a setting with an empty, too-long, or "
                     "unrepresentable name; refusing it\n", active_mod->id);
    return -1;
  }
  for (int i = 0; i < setting_count; i++)
    if (!strcmp(settings[i].mod_id, active_mod->id) && !strcmp(settings[i].name, escaped)) return i;
  if (setting_count >= MODS_MAX_SETTINGS) {
    // Logged here, unconditionally, rather than relying solely on the Lua error api.c raises for
    // this: a mod can pcall() around oracles.setting (pcall is not withheld -- see sandbox.c),
    // which would otherwise let this refusal vanish silently from the mod author's point of view.
    fprintf(stderr, "mods: '%s' tried to register more than %d settings; refusing the extra one\n",
            active_mod->id, MODS_MAX_SETTINGS);
    return -1;
  }
  ModSetting *s = &settings[setting_count];
  snprintf(s->name, sizeof s->name, "%s", escaped);
  snprintf(s->mod_id, sizeof s->mod_id, "%s", active_mod->id);
  s->kind = kind;
  s->bvalue = bvalue;
  s->ivalue = ivalue;
  s->imin = imin;
  s->imax = imax;
  return setting_count++;
}

int mods_setting_add(const char *name, size_t name_len, bool initial) {
  return setting_add_common(name, name_len, MOD_SETTING_BOOL, initial, 0, 0, 0);
}

int mods_setting_add_number(const char *name, size_t name_len, int minv, int maxv, int initial) {
  if (minv > maxv) { int t = minv; minv = maxv; maxv = t; }   // a typo, not a hostile input: tolerate it
  if (initial < minv) initial = minv;
  if (initial > maxv) initial = maxv;
  return setting_add_common(name, name_len, MOD_SETTING_NUMBER, false, initial, minv, maxv);
}

// ---------------------------------------------------------------------------------------------
// Task 9: the per-mod store.
// ---------------------------------------------------------------------------------------------

// Recorded by mods_init (the one place that actually knows it) so mods_store_path/mods_store_set/
// mods_store_get can build "<mods_dir>/<id>/store.ini" without every caller threading `dir`
// through. Always overwritten at the top of the next mods_init before any mod's Lua can possibly
// run again, so -- unlike last_log/last_room/active_mod -- there is no stale-state hazard in
// leaving it untouched by mods_shutdown: nothing can reach mods_store_set/get without an
// active_mod, and active_mod cannot exist without a mods_init that has already set this first.
static char mods_dir[768];

static bool is_store_byte(unsigned char c) { return c >= 0x20 && c < 0x7f; }

bool mods_store_key_valid(const char *key, size_t key_len) {
  if (!key || key_len == 0 || key_len > MOD_STORE_KEY_MAX) return false;
  for (size_t i = 0; i < key_len; i++) {
    unsigned char c = (unsigned char)key[i];
    if (!is_store_byte(c) || c == '=') return false;    // '=' is the key/value separator itself
  }
  // Refuse the keys the reader could not give back. store() writing a key that load() can never
  // retrieve is worse than refusing it: the mod is told it saved, and silently has not. Three
  // shapes do that, all found by storing them and reading them back rather than by reading the
  // parser: a leading '#' makes the whole line a comment, and leading or trailing space is
  // trimmed off, so " k" and "k" would collide and neither round-trips. A '#' anywhere else in
  // the key is fine -- only a line-leading one starts a comment.
  if (key[0] == '#') return false;
  if (key[0] == ' ' || key[0] == '\t') return false;
  if (key[key_len - 1] == ' ' || key[key_len - 1] == '\t') return false;
  return true;
}

bool mods_store_string_valid(const char *s, size_t len) {
  if (!s || len > MOD_STORE_STRING_MAX) return false;
  for (size_t i = 0; i < len; i++)
    if (!is_store_byte((unsigned char)s[i])) return false;
  return true;
}

void mods_store_path(const char *id, char *out, size_t n) {
  if (!out || n == 0) return;
  out[0] = 0;
  if (!mods_id_valid(id)) return;
  snprintf(out, n, "%s/%s/store.ini", mods_dir, id);
}

// True iff `dir_path` exists, is a plain directory, and -- the whole reason this function exists
// -- is not a symlink (POSIX) or reparse point (Windows) standing in for one. Discovery
// (load_one, via `stat`, which follows symlinks) deliberately tolerates a mod folder that is
// itself a symlink, because that's a legitimate way to develop a mod in place without copying it
// into the mods folder, and because until something writes, following it is harmless. The
// moment something writes -- this task -- it stops being harmless: a folder with a perfectly
// valid [a-z0-9-]+ name can point anywhere on disk, and there is no byte in that name for
// mods_id_valid to have rejected. `lstat`, unlike `stat`, reports the link itself rather than
// its target, which is exactly what distinguishes "a real directory" from "a symlink that
// currently resolves to one" -- a distinction mingw-w64 cannot express at all (it has neither
// lstat nor S_ISLNK), so the Windows half uses GetFileAttributesA's reparse-point bit instead.
static bool mod_dir_is_safe_to_write(const char *dir_path) {
#ifdef _WIN32
  DWORD attr = GetFileAttributesA(dir_path);
  if (attr == INVALID_FILE_ATTRIBUTES) return false;
  if (attr & FILE_ATTRIBUTE_REPARSE_POINT) return false;
  return (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
#else
  struct stat st;
  if (lstat(dir_path, &st) != 0) return false;
  if (S_ISLNK(st.st_mode)) return false;
  return S_ISDIR(st.st_mode);
#endif
}

// Opens `path` for a truncating rewrite, refusing if `path` itself (not its parent -- that is
// mod_dir_is_safe_to_write's job, checked by the caller before this is ever reached) is a
// symlink/reparse point: a mod's own folder, even once confirmed to be a real directory and not
// itself a symlink, could still contain a planted "store.ini -> /somewhere/else" symlink (trivial
// to put in a zip -- see the follow-up plan's installer, which must refuse symlink archive
// entries outright for the identical reason). On POSIX, O_NOFOLLOW makes open() fail outright
// (ELOOP) if the final path component is a symlink, with no separate lstat-then-open race.
// mingw-w64 has no O_NOFOLLOW (confirmed: it does not compile), so the Windows half instead
// checks GetFileAttributesA's reparse-point bit just before opening -- a real, if narrow,
// TOCTOU window remains between that check and fopen() below, accepted here because nothing else
// in this single-player, single-process app ever races a mod's own store write.
static FILE *open_store_for_rewrite(const char *path) {
#ifdef _WIN32
  DWORD attr = GetFileAttributesA(path);
  if (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_REPARSE_POINT)) return NULL;
  return fopen(path, "wb");
#else
  int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC | O_NOFOLLOW, 0600);
  if (fd < 0) return NULL;
  FILE *f = fdopen(fd, "wb");
  if (!f) close(fd);
  return f;
#endif
}

// Parses one store.ini value, exactly as mods_store_set below writes it: "b:0"/"b:1" for a
// boolean, "n:i:<decimal>" for an integer, "n:f:<%.17g>" for a float, "s:<text>" for a string --
// the explicit "i"/"f" tag (not just whichever %g a naive formatter would pick) is what lets
// oracles.load() hand back the same Lua *type* that was stored, not merely a value that
// tostring()s the same way: store(42) must load back as the integer 42, not the float 42.0.
// Returns false (leaving *out untouched) for anything that doesn't match one of these four
// shapes -- a hand-edited or corrupted store.ini line is "not found" for that key, not a crash.
static bool decode_store_value(const char *text, ModValue *out) {
  if (!strncmp(text, "b:", 2)) {
    out->kind = MOD_VALUE_BOOL;
    out->b = text[2] == '1';
    return true;
  }
  if (!strncmp(text, "n:i:", 4)) {
    errno = 0;
    char *end;
    long long v = strtoll(text + 4, &end, 10);
    if (end == text + 4 || *end != 0 || errno == ERANGE) return false;
    out->kind = MOD_VALUE_INT;
    out->i = v;
    return true;
  }
  if (!strncmp(text, "n:f:", 4)) {
    char *end;
    double v = strtod(text + 4, &end);
    if (end == text + 4 || *end != 0) return false;
    out->kind = MOD_VALUE_FLOAT;
    out->f = v;
    return true;
  }
  if (!strncmp(text, "s:", 2)) {
    size_t len = strlen(text + 2);
    if (!mods_store_string_valid(text + 2, len)) return false;
    snprintf(out->s, sizeof out->s, "%s", text + 2);
    out->kind = MOD_VALUE_STRING;
    return true;
  }
  return false;
}

// Total size store.ini is allowed to reach, enforced on both the read path (a file bigger than
// this is treated as unreadable -- "not found", never partially parsed) and the write path (a
// rewrite that would grow past this is refused rather than silently truncating some other key's
// row). Generous for even a few hundred ordinary keys while keeping the in-memory rewrite buffer
// below mods_store_set a fixed, small heap allocation rather than something a hostile or merely
// enthusiastic mod could grow without bound by calling oracles.store with ever more distinct keys.
#define MOD_STORE_FILE_MAX ((size_t)16384)

// Copies every "key=value" line of `in` except the one for `key` into `keep` (size `keep_size`,
// always left NUL-terminated), re-using read_logical_line/trim exactly as read_ini already does
// for mod.ini, rather than writing a second line-reader. A comment or blank line in an existing
// store.ini is dropped, not preserved -- store.ini is a file this module alone reads and writes,
// never meant for hand-editing, so there is no round-trip-formatting promise being broken.
// Returns false (refusing the whole rewrite) if the surviving content would not fit in
// `keep_size`, rather than silently dropping whichever keys ran out of room.
static bool rewrite_store_without_key(FILE *in, const char *key, char *keep, size_t keep_size) {
  keep[0] = 0;
  char line[MOD_STORE_KEY_MAX + 16 + MOD_STORE_STRING_MAX];
  while (read_logical_line(in, line, sizeof line)) {
    char *p = line;
    while (*p == ' ' || *p == '\t') p++;
    if (*p == '#' || *p == 0) continue;
    char *eq = strchr(p, '=');
    if (!eq) continue;
    *eq = 0;
    char *k = p;
    trim(k);
    if (!strcmp(k, key)) continue;              // dropped: the caller is about to replace it
    *eq = '=';                                   // restore for verbatim re-emission below
    size_t cur = strlen(keep);
    size_t add = strlen(p);
    if (cur + add + 2 > keep_size) return false; // +2: the '\n' this loop appends, and the NUL
    memcpy(keep + cur, p, add);
    keep[cur + add] = '\n';
    keep[cur + add + 1] = 0;
  }
  return true;
}

bool mods_store_set(const char *key, size_t key_len, const ModValue *value) {
  if (!active_mod) return false;
  if (!mods_store_key_valid(key, key_len)) return false;
  char keybuf[MOD_STORE_KEY_MAX + 1];
  memcpy(keybuf, key, key_len);
  keybuf[key_len] = 0;

  char valtext[MOD_STORE_STRING_MAX + 16];
  switch (value->kind) {
  case MOD_VALUE_BOOL: snprintf(valtext, sizeof valtext, "b:%d", value->b ? 1 : 0); break;
  case MOD_VALUE_INT: snprintf(valtext, sizeof valtext, "n:i:%lld", value->i); break;
  case MOD_VALUE_FLOAT: snprintf(valtext, sizeof valtext, "n:f:%.17g", value->f); break;
  case MOD_VALUE_STRING:
    if (!mods_store_string_valid(value->s, strlen(value->s))) return false;
    snprintf(valtext, sizeof valtext, "s:%s", value->s);
    break;
  default: return false;
  }

  const char *id = active_mod->id;
  char dir_path[896];
  int n1 = snprintf(dir_path, sizeof dir_path, "%s/%s", mods_dir, id);
  if (n1 < 0 || (size_t)n1 >= sizeof dir_path) return false;
  if (!mod_dir_is_safe_to_write(dir_path)) {
    fprintf(stderr, "mods: '%s' is a symlinked (or otherwise unsafe) mod directory; "
                     "refusing to write its store\n", id);
    return false;
  }
  char file_path[1024];
  mods_store_path(id, file_path, sizeof file_path);
  if (!file_path[0]) return false;

  char *keep = malloc(MOD_STORE_FILE_MAX);
  if (!keep) return false;
  keep[0] = 0;

  struct stat st;
  if (stat(file_path, &st) == 0) {
    if (!S_ISREG(st.st_mode)) {
      fprintf(stderr, "mods: '%s/store.ini' is not a regular file; refusing to write to it\n", id);
      free(keep);
      return false;
    }
    if ((size_t)st.st_size > MOD_STORE_FILE_MAX) {
      fprintf(stderr, "mods: '%s/store.ini' is larger than this host will rewrite; refusing\n", id);
      free(keep);
      return false;
    }
    FILE *in = fopen(file_path, "rb");
    if (in) {
      if (contains_nul(in)) {
        fclose(in);
        fprintf(stderr, "mods: '%s/store.ini' contains a NUL byte; refusing to touch it\n", id);
        free(keep);
        return false;
      }
      rewind(in);
      bool ok = rewrite_store_without_key(in, keybuf, keep, MOD_STORE_FILE_MAX);
      fclose(in);
      if (!ok) {
        fprintf(stderr, "mods: '%s/store.ini' is too large to add another key; refusing\n", id);
        free(keep);
        return false;
      }
    }
  }

  size_t cur = strlen(keep);
  int added = snprintf(keep + cur, MOD_STORE_FILE_MAX - cur, "%s=%s\n", keybuf, valtext);
  if (added < 0 || (size_t)added >= MOD_STORE_FILE_MAX - cur) {
    fprintf(stderr, "mods: '%s' store would grow past the size limit; refusing\n", id);
    free(keep);
    return false;
  }

  FILE *out = open_store_for_rewrite(file_path);
  if (!out) {
    fprintf(stderr, "mods: could not write '%s/store.ini' (symlinked path, or another error)\n", id);
    free(keep);
    return false;
  }
  size_t total = strlen(keep);
  size_t wrote = fwrite(keep, 1, total, out);
  fclose(out);
  free(keep);
  return wrote == total;
}

bool mods_store_get(const char *key, size_t key_len, ModValue *out) {
  if (!active_mod) return false;
  if (!mods_store_key_valid(key, key_len)) return false;
  char keybuf[MOD_STORE_KEY_MAX + 1];
  memcpy(keybuf, key, key_len);
  keybuf[key_len] = 0;

  const char *id = active_mod->id;
  char file_path[1024];
  mods_store_path(id, file_path, sizeof file_path);
  if (!file_path[0]) return false;

  // Deliberately NOT mod_dir_is_safe_to_write here -- see mods_store_get's own comment in
  // mods.h for why reading through a symlinked mod directory stays allowed.
  struct stat st;
  if (stat(file_path, &st) != 0) return false;       // no store yet: not found, not an error
  if (!S_ISREG(st.st_mode)) {
    fprintf(stderr, "mods: '%s/store.ini' is not a regular file; ignoring it\n", id);
    return false;
  }
  if ((size_t)st.st_size > MOD_STORE_FILE_MAX) {
    fprintf(stderr, "mods: '%s/store.ini' is larger than this host will read; ignoring it\n", id);
    return false;
  }

  FILE *f = fopen(file_path, "rb");
  if (!f) return false;
  if (contains_nul(f)) {
    fclose(f);
    fprintf(stderr, "mods: '%s/store.ini' contains a NUL byte; ignoring it\n", id);
    return false;
  }
  rewind(f);

  char line[MOD_STORE_KEY_MAX + 16 + MOD_STORE_STRING_MAX];
  bool found = false;
  while (read_logical_line(f, line, sizeof line)) {
    char *p = line;
    while (*p == ' ' || *p == '\t') p++;
    if (*p == '#' || *p == 0) continue;
    char *eq = strchr(p, '=');
    if (!eq) continue;
    *eq = 0;
    char *k = p, *v = eq + 1;
    trim(k);
    if (strcmp(k, keybuf) != 0) continue;
    found = decode_store_value(v, out);
    break;
  }
  fclose(f);
  return found;
}

const char *mods_active_id(void) { return active_mod ? active_mod->id : ""; }

// Validates and fully populates *m for the mod folder `name` directly under `dir`: charset and
// length of the id, that it's actually a directory, that its mod.ini is a regular file (not a
// FIFO, a symlink to a character device, or anything else that could block or loop forever in
// fopen/fgets), free of embedded NUL bytes, parses, declares a matching id, has a usable api=
// (present, parseable, and >= 1 -- but NOT necessarily <= MODS_API_VERSION: see the api= branch
// below for why that upper bound is deliberately not checked here), and a usable games=. Prints
// its own stderr diagnostic on every refusal path, where "refusal" means the whole mod is dropped
// outright -- it is not a mod for this game at all, so it never occupies a slot in mods[] and
// mods_count() never counts it. Does NOT check that main.lua exists, is runnable, or applies to
// the game being played, and does not apply the MODS_MAX cap: those either depend on state that
// spans the whole scan (which game, how many mods are already loaded), or -- for main.lua --
// belong to a different category of failure entirely. A mod this function accepts (returns true
// for) is a real, identified mod for some game; what happens to it next (listed enabled, or
// listed disabled because its own payload failed) is mods_init's call, together with
// run_main_lua below.
static bool load_one(const char *dir, const char *name, Mod *m) {
  if (!mods_id_valid(name)) {
    fprintf(stderr, "mods: '%s' is not a valid mod id (lowercase letters, digits, hyphen only), skipping\n", name);
    return false;
  }
  if (strlen(name) >= MOD_FIELD_LEN) {
    fprintf(stderr, "mods: '%s' is longer than the %d-byte id limit, skipping\n", name, MOD_FIELD_LEN - 1);
    return false;
  }

  char entry_path[1024];
  int n1 = snprintf(entry_path, sizeof entry_path, "%s/%s", dir, name);
  if (n1 < 0 || (size_t)n1 >= sizeof entry_path) {
    fprintf(stderr, "mods: '%s' path too long, skipping\n", name);
    return false;
  }
  struct stat st;
  if (stat(entry_path, &st) != 0 || !S_ISDIR(st.st_mode)) {
    fprintf(stderr, "mods: '%s' in the mods folder is not a directory, skipping\n", name);
    return false;
  }

  char ini_path[1040];
  int n2 = snprintf(ini_path, sizeof ini_path, "%s/mod.ini", entry_path);
  if (n2 < 0 || (size_t)n2 >= sizeof ini_path) {
    fprintf(stderr, "mods: '%s' path too long, skipping\n", name);
    return false;
  }

  // Require a regular file before ever calling fopen on it. A FIFO blocks fopen/fread
  // indefinitely waiting for a writer that will never come, and a symlink to a character device
  // (e.g. mod.ini -> /dev/zero) opens fine and then streams forever, since fgets never sees a
  // newline. stat() follows symlinks, so this also catches the symlink case, and it unifies the
  // "mod.ini is actually a directory" message across platforms: on macOS, fopen() on a directory
  // succeeds (reading as if empty), which previously produced a confusing "has no id=" instead of
  // "has no mod.ini", where Linux's fopen just fails outright for the same input.
  struct stat ini_st;
  // Distinguish absent from present-but-unusable. A FIFO, or a symlink to a device, is a
  // mispackaged or hostile mod; telling its author "has no mod.ini" about a file that plainly
  // exists sends them looking for the wrong problem. Test the stat result rather than errno,
  // which is untouched when stat succeeds and S_ISREG is what fails.
  if (stat(ini_path, &ini_st) != 0) {
    fprintf(stderr, "mods: '%s' has no mod.ini (%s), skipping\n", name, strerror(errno));
    return false;
  }
  if (!S_ISREG(ini_st.st_mode)) {
    fprintf(stderr, "mods: '%s' has a mod.ini that is not a regular file, skipping\n", name);
    return false;
  }

  if (!read_ini(ini_path, name, m)) return false;    // read_ini already printed why

  // The folder name is the canonical id, and mod.ini must declare the same id itself -- a mod
  // package should say who it is, not rely solely on whatever folder it happens to be unpacked
  // into. mod.ini's "id=" is never used to build a path, so a traversing value here (e.g.
  // "../evil") simply fails this string comparison rather than ever being dereferenced as a
  // filename.
  if (!m->ini_id_present) {
    fprintf(stderr, "mods: '%s/mod.ini' has no id=; it must declare id=%s to agree with its folder name\n",
            name, name);
    return false;
  }
  if (strcmp(m->ini_id, name) != 0) {
    fprintf(stderr, "mods: '%s/mod.ini' declares id '%s', which does not match its folder name '%s'; refusing\n",
            name, m->ini_id, name);
    return false;
  }

  if (!m->api_present) {
    fprintf(stderr, "mods: '%s' has no api=, refusing\n", name);
    return false;
  }
  if (!m->api_parsed) {
    fprintf(stderr, "mods: '%s' has an unparseable or out-of-range api=, refusing\n", name);
    return false;
  }
  if (m->api < 1) {
    fprintf(stderr, "mods: '%s' has api=%d, which is below the minimum of 1, refusing\n", name, m->api);
    return false;
  }
  // Deliberately NOT refused here if m->api > MODS_API_VERSION: unlike every check above, this
  // mod has a well-formed, validly-declared identity -- a real package the player installed, not
  // a malformed or absent one. mods_init treats "needs a newer build than this one" as a mod that
  // is accepted but disabled (mods_enabled() false, mods_error() names the required version),
  // the same way a failing main.lua is, rather than making it vanish the way the checks above do.
  if (!m->ages && !m->seasons) {
    fprintf(stderr, "mods: '%s' declares no supported game, refusing\n", name);
    return false;
  }

  // Whether main.lua exists, is a regular file, or runs successfully is no longer this function's
  // job: by this point mod.ini alone has fully qualified `name` as a real mod for some game, and
  // every failure past here (missing/non-regular main.lua, a Lua error, an exhausted budget, ...)
  // is "a real mod whose own payload didn't work", which mods_init lists as disabled rather than
  // dropping -- see run_main_lua, which now makes that same stat() check before ever touching the
  // file, for exactly the same FIFO/symlink-to-device reasons this used to guard against here.
  snprintf(m->id, sizeof m->id, "%s", name);
  if (!m->name[0]) snprintf(m->name, sizeof m->name, "%s", name);
  return true;
}

// The containment a mod's Lua state runs under -- the custom allocator, the instruction/
// wall-clock budget, and the hardened library set (mod_open_libs_hardened, mod_resume_budgeted,
// ModAlloc/mod_lua_alloc) -- lives in sandbox.{c,h}, not here: it is one cohesive
// security-critical unit with a clean seam, and mod_resume_budgeted is also what a later
// per-frame callback runner (Task 8) needs to call from outside this file.

// Actually runs a mod's main.lua -- the one part of loading a mod that executes untrusted code,
// which is why it happens here, in mods_init's loop, rather than inside load_one: by the time this
// is called, the mod has already cleared every load_one check *and* been confirmed to apply to the
// game being played *and* still fit under MODS_MAX, so Lua only ever runs for a mod that is
// actually going to be kept (at least one slot in mods[], enabled or disabled). Also checks that
// main.lua exists and is a regular file -- load_one used to do this, but a mod whose zip forgot
// main.lua is exactly the same category of failure as one whose main.lua errors out, so this
// function owns both now: whatever it refuses, the mod stays listed, disabled, with the reason in
// *m's `error` field (see MOD_ERROR_LEN) -- it is mods_init's caller, not this function, that
// decides a mod is dropped outright, and only for reasons load_one already covers.
//
// What "a broken mod must never take the app down" actually covers today, stated plainly rather
// than claimed in the abstract -- this comment has twice before asserted a guarantee that didn't
// hold, so each line here is something a test actually pins, not something assumed true because
// the design says it should be:
//
// Caught cleanly, mod disabled, host unaffected: a Lua-level infinite loop, however it's shaped
// (bare, pcall-wrapped, or -- before "coroutine" was withheld -- via a freshly-minted thread); a
// looping __gc finalizer (never allowed to arm at all, see l_setmetatable_no_gc); an unbounded
// allocation, in either of Lua's own allocator call shapes; a corrupted or malicious binary
// chunk, from load() or from main.lua itself; and a hostile __tostring metamethod reached while
// reporting an unrelated error. Each of those has a dedicated test under alarm().
//
// Not guaranteed, and now closed by Task 7's sandbox rather than merely scoped out of this one:
// before Task 7, io and os were still fully open, so a mod could call os.exit() to take the
// whole process down outright, or os.execute("sleep 30") (or any other blocking os/io call) to
// stall past the wall-clock deadline with no VM instructions ever executing for the hook to
// catch. mod_open_libs_hardened (sandbox.c) now withholds both libraries entirely -- along with
// require/package, load/loadfile/dofile, string.dump, print/warn and utf8 -- so neither route is
// reachable any more; this comment is kept to record what the gap was and that it is the reason
// those specific names, not some other plausible-looking set, are the ones withheld.
//
// One known, accepted gap that is neither of the above: a mod that recursively calls pcall on
// itself (`local function guard() while true do pcall(guard) end end guard()`) livelocks inside
// lua_resume's own C-level error recovery, never returning to the VM to retire instructions, so
// neither the budget nor the deadline is ever reachable. This is upstream Lua behaviour, requires
// deliberate construction rather than being a plausible accident, and is left unfixed by
// deliberate decision -- not a TODO, a decision -- rather than taking on out-of-band machinery
// (a signal-based watchdog, abandoning Lua states on timeout) disproportionate to the risk at
// this stage. MODDING.md carries it forward to mod authors, under "Limitations, honestly",
// together with the advice it implies: install mods only from sources you trust.
static bool run_main_lua(const char *dir, const char *name, Mod *m, const unsigned char *source, size_t source_size) {
  if (!source) {
    char lua_path[1056];
    int n = snprintf(lua_path, sizeof lua_path, "%s/%s/main.lua", dir, name);
    if (n < 0 || (size_t)n >= sizeof lua_path) {
      fprintf(stderr, "mods: '%s' path too long; disabling it for this session\n", name);
      snprintf(m->error, sizeof m->error, "path too long");
      return false;
    }

    // Require a regular file before ever calling luaL_loadfilex on it, for exactly the same reason
    // load_one's own mod.ini check does: a FIFO blocks fopen/fread indefinitely waiting for a writer
    // that will never come, and a symlink to a character device (main.lua -> /dev/zero) streams
    // forever with no newline for lauxlib's loader to stop on. stat() follows symlinks, so this
    // catches that case too.
    struct stat lua_st;
    if (stat(lua_path, &lua_st) != 0) {
      fprintf(stderr, "mods: '%s' has no main.lua; disabling it for this session\n", name);
      snprintf(m->error, sizeof m->error, "has no main.lua");
      return false;
    }
    if (!S_ISREG(lua_st.st_mode)) {
      fprintf(stderr, "mods: '%s' has a main.lua that is not a regular file; disabling it for this session\n", name);
      snprintf(m->error, sizeof m->error, "main.lua is not a regular file");
      return false;
    }

  }

  ModAlloc *a = malloc(sizeof *a);
  if (!a) {
    fprintf(stderr, "mods: '%s' out of memory allocating its Lua accounting; disabling it for this session\n", name);
    snprintf(m->error, sizeof m->error, "out of memory");
    return false;
  }
  a->used = 0;
  a->cap = MOD_MEM_CAP;

  // lua_newstate, not luaL_newstate: every byte this mod's Lua allocates -- its own strings and
  // tables as well as the interpreter's internal objects -- is counted against that mod's own
  // cap, not the host process's whole heap. Without this, an unbounded allocation loop would grow
  // until the OS killed the process rather than failing cleanly inside Lua.
  lua_State *L = lua_newstate(mod_lua_alloc, a);
  if (!L) {
    // Not silent: a NULL here means even the initial handful of KB the interpreter needs to boot
    // itself failed to allocate, which the caller deserves to know about rather than just losing
    // the mod with no explanation.
    fprintf(stderr, "mods: '%s' could not create a Lua state; disabling it for this session\n", name);
    snprintf(m->error, sizeof m->error, "could not create a Lua state");
    free(a);
    return false;
  }

  // mod_open_libs_hardened (sandbox.c) cuts the standard libraries down to the spec's sandbox --
  // string, table and math -- plus whatever CPU/memory containment holes the earlier tasks found
  // and closed regardless of where that line falls; io, os, require/package,
  // load/loadfile/dofile, string.dump, print/warn and utf8 are all withheld there.
  mod_open_libs_hardened(L);
  mods_api_open(L);

  // Run on its own coroutine thread, driven by mod_resume_budgeted (lua_resume) rather than a
  // plain lua_pcall -- see the long comment above mod_instruction_limit_hook for why that is what
  // actually makes the instruction budget unbypassable by a mod's own pcall or coroutine.resume.
  lua_State *runner = lua_newthread(L);
  char reason[512] = "";
  bool failed = false;

  // Task 8: a mod's top-level main.lua body can itself call oracles.on_frame/on_room_change
  // (the sketch's own test does exactly this), which needs to register onto *this* mod -- the
  // one being constructed in mods_init's stack frame, not yet copied into mods[] -- rather than
  // a Lua global. active_mod is that pointer; reset to NULL before every return out of this
  // function (including every failure path below), never left dangling the way an identically-
  // shaped `active` index was in Task 9's own sketch.
  active_mod = m;

  // "t" forced, not luaL_loadfile's NULL-mode default of "bt": main.lua itself is read straight
  // off disk, and a file that happens to look like a Lua binary chunk (crafted, or merely
  // corrupted) can crash the host outright -- fuzzed to a SIGSEGV on the first trial. This is the
  // host-side half of that fix; `load` itself (the other half) is now withheld from mods
  // entirely rather than kept wrapped to force the same mode -- see mod_open_libs_hardened's own
  // comment in sandbox.c.
  char path[1056];
  snprintf(path, sizeof path, "%s/%s/main.lua", dir, name);
  int load_result = source ? luaL_loadbufferx(runner, (const char *)source, source_size, name, "t")
                           : luaL_loadfilex(runner, path, "t");
  if (load_result != LUA_OK) {
    failed = true;
    size_t elen = 0;
    const char *emsg = lua_tolstring(runner, -1, &elen);
    // A non-string error object (a mod calling error({}), or bare error() raising nil) makes
    // plain lua_tolstring return NULL -- the spec requires detail on stderr either way, so fall
    // back to a literal rather than printing "...: " with nothing after the colon. Deliberately
    // not luaL_tolstring here: it would invoke a __tostring metamethod the error object's
    // metatable might define, which is itself a Lua call -- and this is not a context (just
    // after a load/resume failure, outside any protection of our own) where it's safe to reason
    // about a second failure landing cleanly. A hostile mod could define __tostring to loop
    // forever or to error again itself; handing it that extra entry point is not worth the
    // richer message it would sometimes produce, so this always takes the plain, inert literal
    // instead.
    if (!emsg) { emsg = "(non-string error object)"; elen = strlen(emsg); }
    mods_escape_into(reason, sizeof reason, emsg, elen);
  } else {
    bool exhausted = false, deadline_hit = false;
    int status = mod_resume_budgeted(runner, 0, MOD_LOAD_INSTRUCTION_BUDGET,
                                      MOD_LOAD_WALLCLOCK_DEADLINE_SECONDS, &exhausted, &deadline_hit);
    if (status == LUA_YIELD) {
      failed = true;
      // Two different messages for two different mod authors: one whose script loops forever
      // (the instruction budget caught it quickly, deadline_hit false) and one whose script is
      // just legitimately slow -- expensive per instruction, or merely unlucky on a busy host --
      // and was cut off by the wall-clock deadline instead. Telling them apart means a mod
      // author debugging a timeout is pointed at the right half of the problem.
      if (deadline_hit) {
        snprintf(reason, sizeof reason,
            "ran for longer than %.0f seconds and was cut off", MOD_LOAD_WALLCLOCK_DEADLINE_SECONDS);
      } else {
        snprintf(reason, sizeof reason, "exceeded its instruction budget (possible infinite loop)");
      }
    } else if (status != LUA_OK) {
      failed = true;
      size_t elen = 0;
      const char *emsg = lua_tolstring(runner, -1, &elen);
      if (!emsg) { emsg = "(non-string error object)"; elen = strlen(emsg); }
      mods_escape_into(reason, sizeof reason, emsg, elen);
    } else if (exhausted) {
      // The chunk itself returned cleanly (LUA_OK), but a coroutine it created along the way
      // burned through the whole shared budget and had that absorbed by its own
      // coroutine.resume call rather than ever yielding *this* thread -- see
      // mod_resume_budgeted's doc comment. A clean-looking LUA_OK is not the end of the story.
      //
      // Currently unreachable with "coroutine" withheld -- a mod cannot create the nested
      // thread this branch exists to catch -- kept deliberately as a tripwire against that
      // protection ever being loosened without this branch (and the two "via a nested
      // coroutine" messages below) being revisited at the same time.
      failed = true;
      if (deadline_hit) {
        snprintf(reason, sizeof reason,
            "ran for longer than %.0f seconds via a nested coroutine and was cut off",
            MOD_LOAD_WALLCLOCK_DEADLINE_SECONDS);
      } else {
        snprintf(reason, sizeof reason,
            "exceeded its instruction budget via a nested coroutine (possible infinite loop)");
      }
    }
  }

  // No Lua call for this mod happens past this point in this function -- reset now, on every
  // path (success or failure alike), rather than only on one of them.
  active_mod = NULL;

  if (failed) {
    // reason is attacker-controlled either way: main.lua's own error() argument, the literal
    // budget-exceeded text above, or Lua's fixed "not enough memory" string when the allocator
    // cap was hit. mods_escape_into is the same escaper mod.ini values go through, reused here
    // rather than duplicated, for the identical reason: printed raw, a crafted error string
    // could forge a fake success line on a terminal via an embedded CR and ANSI escape. Already
    // escaped by the time it's copied into m->error below, so simply truncating it to
    // MOD_ERROR_LEN there (via snprintf, not another escape pass) stays byte-safe -- it can only
    // ever shorten a run of plain, printable \xHH/ASCII text, never reintroduce a raw control
    // byte. stderr keeps the untruncated version (`reason`, up to 512 bytes); the stored,
    // short copy is for mods_error()'s status-line use, a different audience with a different
    // length budget.
    fprintf(stderr, "mods: '%s' failed to run main.lua: %s\n", name, reason);
    // Deliberately truncating: `reason` is the full stderr text, m->error is the short
    // status-line copy. The explicit precision says so to the compiler, which otherwise
    // warns about the truncation on the MinGW target and buries real warnings in noise.
    snprintf(m->error, sizeof m->error, "%.*s", (int)(sizeof m->error - 1), reason);
    drop_effects(m->id);
    lua_close(L);
    free(a);
    return false;
  }

  m->L = L;
  return true;
}

static int cmp_name(const void *a, const void *b) {
  return strcmp(*(const char *const *)a, *(const char *const *)b);
}

bool mods_init(const char *dir, bool seasons) {
  mods_shutdown();
  host_seasons = seasons;
  // Task 8 correction: oracles.screen()/oracles.game() must not lie to a mod that calls them
  // from main.lua's own top-level body, before mods_frame has ever run once. mods_init is the
  // one place that actually knows which game this session is -- plumb it through now, with a
  // sane width (160, the non-widescreen default) rather than leaving `frame` all-zero.
  frame = (ModsFrame){ .seasons = seasons, .width = 160 };

  if (!dir || !*dir) {
    fprintf(stderr, "mods: no mods directory given\n");
    return false;
  }
  // Recorded now, with `dir` already confirmed non-NULL/non-empty: mods_store_path/
  // mods_store_set/mods_store_get all build "<mods_dir>/<id>/store.ini" from this rather than
  // threading `dir` through every call, and none of them can run before this line has set it,
  // since all three require an active_mod, and no mod's Lua runs before this point.
  snprintf(mods_dir, sizeof mods_dir, "%s", dir);

  errno = 0;
  DIR *d = opendir(dir);
  if (!d) {
    if (errno == ENOENT) return true;                    // no mods installed yet is not a failure
    fprintf(stderr, "mods: could not open mods directory '%s': %s\n", dir, strerror(errno));
    return false;
  }

  // Collect folder names first and sort them alphabetically before parsing anything, so load
  // order (and, from Task 5 on, main.lua run order) is deterministic rather than following
  // readdir's platform- and history-dependent order. This is alphabetical for determinism only;
  // it is not yet the install-order the spec eventually wants.
  char **names = NULL;
  size_t n = 0, cap = 0;
  struct dirent *e;
  while ((e = readdir(d))) {
    // Skips ".", "..", every hidden entry, and (on macOS) ".DS_Store" -- none of these can ever
    // pass mods_id_valid anyway, so this buys silence rather than correctness: without it, a
    // freshly-unzipped mods folder would print a "not a valid mod id" line for Finder's own
    // litter on every single launch.
    if (e->d_name[0] == '.') continue;
    if (n == cap) {
      size_t new_cap = cap ? cap * 2 : 16;
      char **grown = realloc(names, new_cap * sizeof *names);
      if (!grown) { fprintf(stderr, "mods: out of memory scanning %s\n", dir); break; }
      names = grown;
      cap = new_cap;
    }
    names[n] = strdup(e->d_name);
    if (!names[n]) { fprintf(stderr, "mods: out of memory scanning %s\n", dir); break; }
    n++;
  }
  closedir(d);
  if (n > 0) qsort(names, n, sizeof *names, cmp_name);   // n == 0 leaves names NULL; don't touch it

  int loaded = 0, dropped_for_cap = 0;
  for (size_t i = 0; i < n; i++) {
    const char *name = names[i];
    Mod m = {0};                                          // enabled starts false; set true below
                                                            // only once this mod actually succeeds
    m.on_frame_ref = LUA_NOREF;                            // NOT 0 -- {0} would zero-init these to
    m.on_room_change_ref = LUA_NOREF;                      // 0, a real (if unlikely) ref value,
                                                            // not "unregistered"; LUA_NOREF is -2.
    if (!load_one(dir, name, &m)) continue;                // not a mod for us at all: no slot used
    if (!(seasons ? m.seasons : m.ages)) continue;         // valid mod, just not for this game

    if (loaded >= MODS_MAX) {
      dropped_for_cap++;
      continue;
    }

    // From here on this is a real, identified mod for this game, and it occupies slot `loaded`
    // regardless of what happens next: a failure past this point disables the mod rather than
    // dropping it, so a player can see it listed with a reason instead of concluding their
    // install silently failed. See mods.h's mods_init comment for the full rationale.
    if (m.api > MODS_API_VERSION) {
      // A well-formed, validly-declared api= this build is simply too old for -- unlike every
      // refusal inside load_one, this is not a malformed package, so it is listed disabled
      // rather than dropped (see load_one's comment on its api= branch).
      fprintf(stderr, "mods: '%s' wants api %d, this build only supports %d; disabling it for this session\n",
              name, m.api, MODS_API_VERSION);
      snprintf(m.error, sizeof m.error, "needs mod api %d (this build supports up to %d)",
               m.api, MODS_API_VERSION);
      m.enabled = false;
    } else {
      // Only now -- known to apply to this game, known to still have a slot, and declaring an api
      // this build actually supports -- is it worth the cost and risk of actually running this
      // mod's Lua. run_main_lua already printed why, and filled m.error, on failure.
      m.enabled = run_main_lua(dir, name, &m, NULL, 0);
    }
    mods[loaded++] = m;
  }
  count = loaded;
  if (dropped_for_cap > 0)
    fprintf(stderr, "mods: %d mod(s) dropped, only the first %d are loaded\n", dropped_for_cap, MODS_MAX);

  for (size_t i = 0; i < n; i++) free(names[i]);
  free(names);
  return true;
}

// Official scripts use exactly the same Lua sandbox as folder mods. The bytes are
// embedded at build time, so releases remain self-contained on every platform.
extern const unsigned char official_essence_tracker[];
extern const unsigned int official_essence_tracker_size;
extern const unsigned char official_tracker_ini[];
extern const unsigned int official_tracker_ini_size;
extern const unsigned char official_randomizer_main[], official_randomizer_ini[];
extern const unsigned int official_randomizer_main_size, official_randomizer_ini_size;
static Mod official_metadata(const unsigned char *data, size_t size) {
  Mod m = {0};
  size_t at = 0;
  while (at < size) {
    char line[256];
    size_t n = 0;
    while (at < size && data[at] != '\n') {
      if (n < sizeof line - 1) line[n++] = (char)data[at];
      at++;
    }
    at++;
    line[n] = 0;
    parse_ini_line(line, "official mod", &m);
  }
  snprintf(m.id, sizeof m.id, "%s", m.ini_id);
  return m;
}

static ModInfo info_of(const Mod *m) {
  ModInfo info = {.ages = m->ages, .seasons = m->seasons, .changes_game = m->changes_game};
  snprintf(info.name, sizeof info.name, "%s", m->name);
  snprintf(info.description, sizeof info.description, "%s", m->description);
  snprintf(info.author, sizeof info.author, "%s", m->author);
  snprintf(info.version, sizeof info.version, "%s", m->version);
  return info;
}
ModInfo mods_info(int index) {
  return index >= 0 && index < count ? info_of(&mods[index]) : (ModInfo){0};
}
ModInfo mods_official_tracker_info(void) {
  Mod m = official_metadata(official_tracker_ini, official_tracker_ini_size);
  return info_of(&m);
}
const char *mods_load_official_tracker(void) {
  for (int i = 0; i < count; i++)
    if (!strcmp(mods[i].id, "essence-tracker"))
      return "Remove the folder copy of Essence Tracker first";
  if (count >= MODS_MAX) return "Too many mods loaded";
  Mod m = official_metadata(official_tracker_ini, official_tracker_ini_size);
  m.on_frame_ref = m.on_room_change_ref = LUA_NOREF;
  m.enabled = run_main_lua(mods_dir, m.id, &m, official_essence_tracker, official_essence_tracker_size);
  mods[count++] = m;
  return m.enabled ? NULL : mods[count - 1].error;
}

ModInfo mods_official_randomizer_info(void) {
  Mod m = official_metadata(official_randomizer_ini, official_randomizer_ini_size);
  return info_of(&m);
}
const char *mods_load_official_randomizer(void) {
  if (!host_seasons) return NULL;
  for (int i = 0; i < count; i++)
    if (!strcmp(mods[i].id, "d1-randomizer"))
      return "Remove the folder copy of D1 Randomizer first";
  if (count >= MODS_MAX) return "Too many mods loaded";
  Mod m = official_metadata(official_randomizer_ini, official_randomizer_ini_size);
  m.on_frame_ref = m.on_room_change_ref = LUA_NOREF;
  m.enabled = run_main_lua(mods_dir, m.id, &m, official_randomizer_main, official_randomizer_main_size);
  mods[count++] = m;
  return m.enabled ? NULL : mods[count - 1].error;
}

void mods_shutdown(void) {
  // Close every mod's Lua state (and free the per-mod allocator accounting that went with it)
  // before resetting count -- not after, since mods[] is about to become meaningless. The
  // `!mods[i].L` skip is no longer a defensive no-op kept "just in case": since Task 6, a
  // disabled mod (mods_enabled(i) false) is routinely stored in mods[] with L == NULL -- run_main_lua
  // either never created one (main.lua missing, an allocation failure before lua_newstate) or
  // already closed and freed it itself on the failure path before ever returning -- so this is
  // the thing that keeps mods_shutdown from calling lua_close(NULL) on every disabled mod in the
  // list.
  for (int i = 0; i < count; i++) {
    if (!mods[i].L) continue;
    void *ud = NULL;
    lua_getallocf(mods[i].L, &ud);
    lua_close(mods[i].L);
    free(ud);
  }
  count = 0;
  // Correction: a stale oracles.log() message must not outlive the mod that logged it, or a
  // silent mod loaded afterwards would be misreported as having said something. mods_init always
  // calls mods_shutdown first, so this also covers "the top of mods_init", not just an explicit
  // shutdown call.
  mods_api_clear_log();
  // Task 8 correction: last_room must not outlive the session that set it, for the identical
  // reason last_log above does not -- see last_room's own comment for what goes wrong if it
  // did. active_mod is reset defensively too, though every path that sets it already resets it
  // itself; a mods_shutdown called mid-callback should not be possible, but this is cheap
  // insurance against ever leaving it pointing at a Mod slot that mods[] is about to reuse.
  last_room = -1;
  active_mod = NULL;
  swap_count = 0;
  swap_owner[0] = 0;
  write_count = 0;
  // Task 9: settings rows are session-only by deliberate decision (see mods_setting_count's own
  // comment in mods.h), so they must not outlive the session that registered them either -- the
  // same stale-state class as last_log/last_room above, now a third instance, closed the same
  // way: cleared here, which mods_init's own leading mods_shutdown() call also covers.
  setting_count = 0;
  memset(settings, 0, sizeof settings);
}

int mods_count(void) { return count; }
const char *mods_id(int i) { return i >= 0 && i < count ? mods[i].id : ""; }
const char *mods_name(int i) { return i >= 0 && i < count ? mods[i].name : ""; }
bool mods_enabled(int i) { return i >= 0 && i < count && mods[i].enabled; }
const char *mods_error(int i) { return i >= 0 && i < count ? mods[i].error : ""; }

static int installed_compare(const void *a, const void *b) {
  return strcmp(((const ModInstalled *)a)->id, ((const ModInstalled *)b)->id);
}
int mods_scan_installed(const char *dir, ModInstalled *out, int capacity) {
  if (!dir || !*dir || !out || capacity <= 0) return -1;
  DIR *d = opendir(dir);
  if (!d) return errno == ENOENT ? 0 : -1;
  int n = 0;
  struct dirent *e;
  while ((e = readdir(d))) {
    if (e->d_name[0] == '.') continue;
    Mod m = {0};
    if (!load_one(dir, e->d_name, &m)) continue;
    ModInstalled entry = {.info = info_of(&m)};
    snprintf(entry.id, sizeof entry.id, "%s", m.id);
    // Keep the alphabetically first entries even when the menu capacity is reached.
    if (n < capacity) out[n++] = entry;
    else if (strcmp(entry.id, out[n - 1].id) < 0) out[n - 1] = entry;
    qsort(out, n, sizeof *out, installed_compare);
  }
  closedir(d);
  return n;
}
