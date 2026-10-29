#pragma once
// The mod host. Mods are folders under the app's mods directory, each with a mod.ini and a
// main.lua. This file covers discovery (deciding which mod folders exist, are well-formed, and
// apply to the game being played) and now also runs each accepted mod's main.lua, under its own
// bounded Lua state: an instruction-count budget, a wall-clock deadline, and a memory cap per
// mod (see src/mods/sandbox.c), plus a handful of specific standard-library holes closed or
// wrapped there (mod_open_libs_hardened) because they would otherwise let a mod hang or crash
// the host regardless of those budgets -- so that a broken or hostile mod can be contained, not
// let to take the host down. A mod whose main.lua fails is disabled, not removed: see mods_init's
// own comment below for exactly where the line between "not a mod for this game" (skipped,
// occupies no slot) and "a real mod whose payload failed" (listed, mods_enabled() false) sits.
// The Lua API surface a mod sees (the `oracles` table) lives in api.h/api.c, not
// here. What main.lua is permitted to do beyond that containment is the spec's sandbox (the
// standard string, table and math libraries, plus oracles): io, os, require/package,
// load/loadfile/dofile, string.dump, print/warn and utf8 are all withheld (see
// mod_open_libs_hardened in sandbox.c for exactly which and why). "Linked into the apps only,
// never into the engine libraries" is the design constraint this module holds, and as of Task 10
// it is present fact rather than intent: oraclesmods is linked into the SDL app targets (and
// test_mods/test_mods_rom) and into nothing else, so oraclescore/oraclesnative -- the TAS
// verification path -- still cannot load a mod. MODDING.md, at the repo root, is the
// player-facing reference for everything below that a mod can actually see; test_mods's
// the_docs_document_exactly_the_live_api fails the build if the two ever disagree.
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
// For ModOverlay/UiFont: mods_frame hands a mod's drawing bindings an overlay to mutate and a
// frame's dimensions/resources to read, and ModsFrame below embeds both a ModOverlay* consumer
// (api.c) and a UiFont* -- the real, tagless `typedef struct {...} UiFont` overlay.h already
// pulls in via ui/ui.h, not a `struct UiFont;` forward declaration, which would name an
// unrelated incomplete type and silently make every real UiFont* argument incompatible. See
// overlay.h's own comment on exactly this trap.
#include "mods/overlay.h"

// For mods_set_on_frame_ref/mods_set_on_room_change_ref below. A forward declaration at file
// scope, not only inside those functions' own parameter lists: a struct tag first introduced
// inside a prototype's parameter-list scope is local to that scope, not file scope, and would
// conflict with the file-scope "struct lua_State" lua.h's typedef later establishes once a .c
// file includes both headers -- confirmed by the build failing with exactly that "conflicting
// types" error without this line present. api.h and sandbox.h both already declare it the same
// way, for the same reason.
struct lua_State;

#define MODS_API_VERSION 2
// The hard cap on how many mods occupy a slot in mods[] at once -- counting a disabled mod
// exactly the same as an enabled one, since a disabled mod still has an id, a name and a reason
// to show on a status line; only a mod never accepted for this game at all (wrong game, bad
// mod.ini, bad folder name) consumes no slot and so cannot be what pushes a later mod over this
// cap. See loading_stops_at_mods_max in tests/test_mods.c for the 33rd-mod boundary this guards.
#define MODS_MAX 32

// Scans `dir` for mod folders and decides which apply to `seasons` (true) or ages (false), then
// runs each applicable one's main.lua under sandbox.c's containment. Returns true once discovery
// (and, for every mod found applicable, its load attempt) has completed, including when it found
// zero mods: a `dir` that does not exist yet is zero mods (none installed), not a failure.
// Returns false if `dir` is NULL or empty, or if `dir` exists but could not be scanned for some
// other reason (e.g. permissions) -- silently reporting "no mods" in that case would hide a real
// problem from the caller instead of surfacing it.
//
// A mod whose own main.lua fails -- a Lua error, an exhausted instruction budget or wall-clock
// deadline, an out-of-memory allocation, a rejected __gc metatable or binary chunk, a missing or
// unreadable main.lua, or a declared `api=` this build is too old to support -- is *not* a
// mods_init failure and does not make the mod disappear: it is kept in the list (mods_count()
// counts it, mods_id()/mods_name() still describe it), with mods_enabled() false and mods_error()
// giving the reason, so a status line can tell the player their mod is there but broken rather
// than making it look like the install silently failed. The distinction mods_init itself draws is
// "is this folder a mod for this game at all" (a missing/unparseable mod.ini, a folder name
// outside the accepted charset, a stray non-directory entry, a mod.ini that names a different
// game) -- those are not mods for this game and are skipped outright, occupying no slot and
// reported only on stderr -- versus "this is a real, accepted mod for this game whose own payload
// did not run", which is what mods_enabled()/mods_error() exist to describe.
bool mods_init(const char *dir, bool seasons);
void mods_shutdown(void);

// The number of mods accepted for the current game, counting both the ones running normally and
// the ones listed disabled by mods_enabled()/mods_error() below -- a mod is dropped from this
// count only when it was never accepted as a mod for this game at all (wrong game, malformed
// mod.ini, bad folder name, MODS_MAX overflow), never because its own main.lua failed.
int mods_count(void);
// Returns the mod at `i`'s canonical id (its folder name) or display name. For `i` outside
// [0, mods_count()), returns "" -- never NULL, and never a read past the end of the loaded-mods
// array. That range check matters beyond ordinary robustness: a later task will index these from
// Lua with an integer the mod itself chooses, so `i` must be treated as hostile input here, not
// just a programmer mistake. Both are populated identically whether the mod ends up enabled or
// disabled -- id/name describe the package, not whether it runs.
const char *mods_id(int i);
const char *mods_name(int i);
// Returns whether the mod at `i` loaded and ran successfully, or false for `i` outside
// [0, mods_count()) -- same out-of-range contract, and the same reason it matters, as
// mods_id/mods_name above. False for an in-range `i` means the mod is accepted and listed but
// disabled for this session (see mods_error() for why): it has no live Lua state, consumes none
// of its 8 MiB allocator budget, and a per-frame caller (Task 8) must skip it.
bool mods_enabled(int i);
// Returns why the mod at `i` is disabled, or "" if it is enabled, or `i` is outside
// [0, mods_count()) -- never NULL, matching mods_id/mods_name's convention of never returning
// NULL for a valid-looking call. Attacker-controlled (a Lua error message is the mod author's own
// text) and so already passed through mods_escape_into before being stored: no raw control byte
// or invalid-UTF-8 sequence can reach whatever prints this, directly or on a future status line.
// Deliberately short (see MOD_ERROR_LEN in mods.c) -- this is for a one-line display, not a full
// diagnostic; the unabridged detail always also goes to stderr at the moment the mod is disabled,
// which is where a developer debugging a specific mod should look.
const char *mods_error(int i);

// The most recent oracles.log() message, or NULL if no mod has logged anything since the last
// mods_init/mods_shutdown. Implemented in api.c (that's where the Lua binding and its storage
// live); declared here because this is the mod host's public surface -- tests and a future status
// line both read it through mods.h, not api.h, the same way they read mods_name()/mods_id().
// Cleared by mods_shutdown (and so, transitively, by the mods_init that always calls it first) so
// a silent mod loaded after a chatty one never inherits its predecessor's stale log line.
//
// oracles.log('') is indistinguishable from no call at all: both leave last_log empty and this
// returns NULL either way. That's an accepted limitation, not a bug -- "logged nothing" and
// "logged the empty string" are the same thing to a one-line status display, which is the only
// consumer this exists for today -- but it does mean this function cannot be used to detect
// *that a mod called oracles.log at all* if the string it passed happened to be empty.
const char *mods_last_log(void);

// Copies src (exactly src_len bytes, which may contain embedded NULs -- it is not assumed to be a
// NUL-terminated C string) into dst (bounded by dst_size, always NUL-terminated), replacing every
// byte outside printable ASCII (0x20..0x7e) with a \xHH escape. A value under this module's
// control reaches a terminal three ways -- this process's own stderr diagnostics, whatever
// mods_name()/mods_id() hand back to a future caller (a mod list UI, say), and now oracles.log()
// and a failed main.lua's own error text, both straight from untrusted Lua -- so sanitizing once,
// at the point a value is stored or about to be printed, keeps all of them safe rather than
// re-escaping at every print site. Shared between mods.c (mod.ini values) and api.c (oracles.log
// and the "mod failed to load" message) rather than each keeping its own copy. Without this, a
// value like "victim\r\x1b[2K\rmods: 'victim' loaded OK (api 1)" overwrites the current terminal
// line with a forged success message the moment it's printed raw.
void mods_escape_into(char *dst, size_t dst_size, const char *src, size_t src_len);

// True iff `id` is non-empty and every byte is a lowercase letter, digit, or hyphen. A mod's id
// doubles as a filesystem path component (its folder name, and later its store file's name), so
// this is the gate against NAME-based path traversal: no byte this rejects can ever spell ".."
// or "/" or "\". It is not a complete defence against every path hazard a mod folder could pose:
// a mod folder that is itself a symlink pointing outside the mods directory is a separate
// concern, left to Task 9's zip installer and the per-mod store writer to close, not something
// this check already covers. Shared with the folder-name check in mods_init, and meant to be
// reused by those later call sites rather than reinventing a charset check at each one.
bool mods_id_valid(const char *id);

// What the app hands the host each frame: game RAM windows, the picture's dimensions, the
// current room's geometry, and the resources a mod's drawing bindings need (the ROM for
// draw_icon, the UI font for draw_text).
// `wram`/`wram3`/`hram`/`bg_pal`/`oam`/`rom`/`font` may each be NULL -- before any ROM/game is actually running,
// or (in tests) deliberately, to exercise the guards every binding that touches them must have.
// `wram`, when non-NULL, is always the full 4096 bytes $c000..$cfff: draw_icon relies on that to
// read `wram + 0x600` as 256 further bytes without a separate bounds check of its own. `wram3`,
// when non-NULL, is likewise always the full 4096 bytes $d000..$dfff of WRAM bank 3 -- the fixed
// physical bank w3TileCollisions always lives in, not whichever bank SVBK currently has switched
// into the $d000 window -- and it is NULL in exactly the same circumstances `wram` is: no live
// game yet, or a test that deliberately left it unset. That matters beyond this struct: a later
// binding built on `wram3` (e.g. oracles.solidity) must return `nil` in that case rather than a
// plausible-looking zero, because a solidity byte of 0 means "walkable" -- defaulting to 0 for
// "no data" would be actively wrong, not merely uninformative, the same lesson Stage 1 learned
// from oracles.screen() returning 0 before the first frame.
// `room_w`/`room_h` are the current room's width/height in 8x8 tiles, exactly as wRoomWidth/
// wRoomHeight hold them (a metatile-based consumer must halve them); `room_group` is the room's
// group, read alongside `room` from the same pair of bank-0 addresses test_room.c's record_cb
// and app.c's own write_crash_log already use for the identical purpose. All three are
// derived from `wram` and so are only meaningful while it is non-NULL; like `room`, they default
// to 0 otherwise rather than being independently NULL-able (they are plain ints, not pointers).
typedef struct {
  const uint8_t *wram;          // $c000..$cfff, 4096 bytes, or NULL
  const uint8_t *wram3;         // $d000..$dfff as WRAM bank 3, 4096 bytes, or NULL
  const uint8_t *wram1;         // $d000..$dfff as WRAM bank 1 (the objects), 4096 bytes, or NULL
  const uint8_t *hram;          // $ff80..$fffe, 127 bytes, or NULL
  bool seasons;
  int width;                    // the picture's width: 160, or 256 in widescreen
  int room;                     // the current room, for on_room_change
  int room_w, room_h;           // the room in 8x8 tiles, as wRoomWidth/wRoomHeight hold them
  int room_group;               // the room's group, for callers that need it with `room`
  const uint8_t *bg_pal;        // the 8 BG palettes the picture was drawn with, 64 bytes, or NULL
  const uint8_t *oam;           // the OAM the picture was drawn with, 160 bytes, or NULL
  bool scroll;                  // scroll_x/scroll_y hold a picture's scroll (false: no picture)
  int scroll_x, scroll_y;       // SCX/SCY the play area's first line was drawn with
  bool origin;                  // origin_x/origin_y hold a room position (false: not in play)
  int origin_x, origin_y;       // the room pixel at the play area's top-left
  const uint8_t *rom;           // for draw_icon; may be NULL
  size_t rom_size;
  const UiFont *font;           // for draw_text; may be NULL
} ModsFrame;

// Runs every enabled mod's on_frame handler, then (when f->room has changed since the last call)
// its on_room_change handler, then leaves the overlay refilled for this frame -- see
// overlay_begin, called first so draw_rect/draw_text/draw_icon land on a clean buffer. Each
// handler call is budgeted (see the MOD_FRAME_* constants documented in mods.c, next to the
// call_handler helper that enforces them via sandbox.h's mod_resume_budgeted): an error, an
// exhausted per-frame instruction budget, or an exceeded per-frame wall-clock deadline disables
// that one mod -- mods_enabled() false, mods_error() the reason -- exactly as a load-time failure
// does, never the whole scan and never merely "try again next frame" with a fresh budget, which
// would be a permanently wedged game rather than a disabled mod -- see mod_resume_budgeted's own
// doc comment in sandbox.h for why a per-frame caller must poison on exhaustion itself. A NULL
// `f` is a no-op: the launcher may call this before any game is actually running.
void mods_frame(const ModsFrame *f);
// The overlay the app composites over the picture after this frame's mods_frame call.
const ModOverlay *mods_overlay(void);
// For api.c: the frame currently being served to a mod's Lua. Valid both while mods_frame is
// calling a handler and at any other time a mod's Lua runs (including main.lua's own top-level
// body, before mods_frame has ever been called) -- see mods_init's own comment for the sane
// pre-first-frame default (seasons plumbed through from mods_init's own argument, width 160) it
// sets so oracles.game()/oracles.screen() never lie to a mod that calls them at load time.
const ModsFrame *mods_current_frame(void);
// For api.c's drawing bindings: the overlay a mod is allowed to mutate.
ModOverlay *mods_overlay_mut(void);

// For api.c's oracles.on_frame/oracles.on_room_change bindings: registers a Lua-registry
// reference (from luaL_ref(L, LUA_REGISTRYINDEX), on the state the binding was called from) as
// the mod that is *currently executing Lua*'s handler for that event, replacing and unref'ing
// whatever was registered before. Looked up by an internal "which mod is currently running"
// pointer, not a Lua global: a global the binding itself sets (e.g. lua_setglobal(L,
// "__on_frame")) is also a global a mod could set directly, bypassing oracles.on_frame
// entirely, which is exactly the hole this avoids. A call from outside any mod's own Lua
// execution (should not happen -- a mod has no other way to call a C function at all) is a
// silent no-op.
//
// `L` is the state the binding itself was called on -- during main.lua's own top-level body
// that is run_main_lua's `runner` coroutine, not the mod's main state (which active_mod->L
// cannot stand in for: it is still NULL at that point, run_main_lua only assigns it after this
// whole call has already returned) -- and is used for the luaL_unref of any previous ref rather
// than active_mod->L, since any thread belonging to the same global state is equally valid for a
// registry operation. Confirmed necessary by crashing without it: a mod that registers on_frame
// twice from its own top-level body dereferences a NULL lua_State on the second call otherwise.
//
// The parameter is named `lua_vm`, not the conventional `L` every .c file in this module uses for
// the same type: game/asm.h, which src/platform/app.c already includes (via game/game.h) before
// this header, #defines `L` as `gb->l` for the CPU emulator's own convenience and never #undefs
// it, so a prototype parameter named `L` here fails to parse in any translation unit that
// includes both -- confirmed by app.c's own build once it had to include this header. Only the
// declaration's parameter name needs to change; a declaration's parameter names are documentation
// only, so mods.c's actual definitions keep `L`, as does every other Lua-facing file here.
void mods_set_on_frame_ref(struct lua_State *lua_vm, int ref);
void mods_set_on_room_change_ref(struct lua_State *lua_vm, int ref);

// ---------------------------------------------------------------------------------------------
// Task 9: settings and the per-mod store.
// ---------------------------------------------------------------------------------------------

// The hard cap on how many settings rows exist across every mod at once -- a fixed-size array,
// the same shape as MODS_MAX, so a single pathological or buggy mod cannot grow this table
// without bound. api.c's oracles.setting() raises a Lua error (and this file logs to stderr
// unconditionally, so the failure is visible even if the mod wraps the call in pcall) once this
// is reached; it does not refuse the mod outright.
#define MODS_MAX_SETTINGS 32

// A settings row is either an ON/OFF toggle or a bounded integer -- "the two shapes the settings
// menu already draws", per the spec. oracles.setting(name, true/false) makes a BOOL row;
// oracles.setting(name, {min, max, default}) makes a NUMBER row. Nothing about the *kind* a mod
// asked for can be inferred from its default value alone (both shapes start somewhere), so a
// host UI must call mods_setting_kind() before deciding whether to draw a toggle or a stepper.
typedef enum { MOD_SETTING_BOOL, MOD_SETTING_NUMBER } ModSettingKind;

// Session-only, by deliberate decision, not an oversight: nothing here persists a setting's
// *value* to disk, so every row returns to the mod's own declared default each time mods_init
// runs. This mirrors the precedent Task 6 already set for a mod's enabled/disabled state --
// "full enable/disable persistence... is deliberately not in this plan; it belongs to the
// follow-up plan with the manager UI and zip install" -- and a setting's value is the same kind
// of host-UI state as that, not the kind of mod-owned data oracles.store()/oracles.load() exist
// for. Implementing persistence now would mean inventing a storage location Plan 2's manager UI
// might not actually want (the spec's mods.ini, which also owns load order and enabled state, is
// squarely that plan's file to design): the two given tests for this task don't exercise setting
// persistence across a reload either, only store/load does. MODDING.md says so plainly under
// oracles.setting, rather than leaving a mod author to discover it by losing a user's toggle.
//
// The rows themselves (how many exist, and which mod and name each one is for) also do not
// survive a reload -- mods_shutdown clears the whole table -- which is correct given the above:
// a session-only value in a table that *did* survive a reload would be actively misleading (a
// row with no value behind it).
int mods_setting_count(void);
// The mod-supplied label, already passed through mods_escape_into (so no raw control byte or
// invalid-UTF-8 sequence can reach a terminal or a future settings-menu UI the way an unescaped
// mod.ini field or oracles.log() string could) and never truncated: mods_setting_add/
// mods_setting_add_number refuse (do not register) a name whose escaped form would not fit
// in full, rather than silently cutting two different over-long names down to the same
// 31-byte prefix the way a bare snprintf truncation would -- the same collision class Task 4
// closed for a mod's own folder id. "" for `i` outside [0, mods_setting_count()).
const char *mods_setting_name(int i);
// Which mod (by its canonical id -- the validated folder name, the same string mods_id()
// returns for that mod's own slot) registered the row at `i`, or "" if `i` is out of range. A
// settings menu is expected to combine this with the row's name to label two different mods'
// identically-named rows distinguishably (e.g. "Opt: Show" vs "Other: Show") -- deliberately not
// baked into mods_setting_name() itself, since that is what the existing fixed test
// (a_mod_adds_a_setting) asserts returns exactly the mod's own unprefixed label.
const char *mods_setting_mod_id(int i);
ModSettingKind mods_setting_kind(int i);
// True iff `i` is in range, the row at `i` is a BOOL row, and its current value is true. False
// for a NUMBER row at `i`, not an error -- a host UI that confuses the two kinds gets a
// consistently-false read, never a read of the wrong row's data.
bool mods_setting_value(int i);
// The current value of a NUMBER row at `i`, already clamped into [min, max]; 0 for a BOOL row,
// or for `i` out of range.
int mods_setting_number(int i);
int mods_setting_min(int i);
int mods_setting_max(int i);
// Flips a BOOL row's value. A no-op for a NUMBER row, or for `i` out of range -- not a crash and
// not a silent reinterpretation of imin/imax/ivalue as if they were a bool.
void mods_setting_toggle(int i);

// Called from api.c's oracles.setting(name, true/false): registers (or, for a second call from
// the same mod with the same name, simply finds) a BOOL row, scoped by (the currently active
// mod's id, name) -- not by `name` alone, which is what let two different mods' same-named
// settings share one row before this task closed it. Returns the row's index, or -1 if there is
// no active mod (should not happen -- see mods_active_id()'s own comment), `name` is empty,
// longer than this function tolerates, or its escaped form would not fit in
// mods_setting_name()'s 32-byte field (mods.c logs this refusal to stderr itself, unconditionally,
// before returning -1, since api.c's caller may raise a Lua error the mod itself goes on to
// pcall and swallow), or MODS_MAX_SETTINGS rows already exist (same unconditional stderr log).
int mods_setting_add(const char *name, size_t name_len, bool initial);
// Called from oracles.setting(name, {min, max, default}): the NUMBER-row equivalent of
// mods_setting_add above, with the identical scoping, refusal and logging behaviour. `minv` and
// `maxv` are tolerated in either order (swapped if minv > maxv, rather than raising an error over
// what is almost certainly a mod author's typo, not a hostile input); `initial` is clamped into
// the resulting [min, max] range.
int mods_setting_add_number(const char *name, size_t name_len, int minv, int maxv, int initial);

// The id of the mod whose Lua is currently running -- main.lua's own top-level body
// (run_main_lua), or a frame handler (call_handler) -- reusing the exact same notion Task 8
// already built for mods_set_on_frame_ref/mods_set_on_room_change_ref (active_mod) rather than
// adding a second "which mod is active" mechanism: this task's own planning sketch proposed a
// separate `active` index for this purpose, which the pre-dispatch review flagged as both
// redundant with active_mod and, unlike active_mod, never reset after use, so whichever mod last
// touched the store would silently keep "owning" every later load/store/setting call reached
// from outside any mod's own Lua. Returns "" if no mod's Lua is currently running.
const char *mods_active_id(void);

// Builds the path to `id`'s per-mod store file into `out` (size `n`), or leaves `out` as an
// empty string if `id` fails mods_id_valid or `out`/`n` cannot hold the result. For display and
// test inspection; mods_store_set/mods_store_get do not trust a path built elsewhere -- each
// re-derives and re-validates its own, including the symlinked-directory and symlinked-file
// checks that writing (and only writing) needs. Never creates or opens anything itself.
void mods_store_path(const char *id, char *out, size_t n);

// Limits enforced on every oracles.store() key and string value -- refused outright, never
// mangled or silently truncated, if exceeded. A key may not contain '=' (the key/value
// separator) or any byte outside printable ASCII (0x20..0x7e, which also excludes NUL, CR and
// LF -- the exact bytes that could otherwise inject a second key=value line or corrupt the
// store's one-line-per-key shape); a string value may contain '=' freely (a line is split at
// only its *first* '=', the same convention mod.ini's own reader already uses) but is held to
// the same printable-ASCII, no-control-byte rule. Both bounds are deliberately generous for an
// ordinary settings/save-state value while keeping store.ini a small, bounded text file rather
// than something a hostile or buggy mod could grow without limit.
#define MOD_STORE_KEY_MAX 63
#define MOD_STORE_STRING_MAX 503

// A value oracles.store()/oracles.load() can carry -- exactly the three Lua types the spec
// grants ("strings, numbers and booleans"), plus the int/float split Lua 5.4 itself makes
// between an integer-subtype number and a float-subtype one, which the store's own on-disk
// format tags explicitly (see mods.c) so oracles.load() hands back the same Lua type that was
// stored rather than merely the same tostring() rendering -- store(42) must load back as the
// integer 42, not the float 42.0, even though tostring() cannot tell those apart in a test
// assertion.
typedef enum { MOD_VALUE_BOOL, MOD_VALUE_INT, MOD_VALUE_FLOAT, MOD_VALUE_STRING } ModValueKind;
typedef struct {
  ModValueKind kind;
  bool b;
  long long i;
  double f;
  char s[MOD_STORE_STRING_MAX + 1];
} ModValue;

// Byte-for-byte validity checks api.c's bindings run before ever handing a key or string value
// to mods_store_set -- exposed so api.c can refuse with a clear Lua error itself rather than
// only learning indirectly from a false return out of mods_store_set. Both take an explicit
// length rather than trusting strlen(): a Lua string may legally contain an embedded NUL, and
// trusting strlen() on one is exactly the NUL-smuggling class Task 4 already found and fixed
// once, in mod.ini's own parsing -- a length-taking check closes it here the same way, since any
// embedded NUL simply fails the printable-ASCII byte test at its own index rather than silently
// truncating the string being validated.
bool mods_store_key_valid(const char *key, size_t key_len);
bool mods_store_string_valid(const char *s, size_t len);

// Saves `value` under `key` in the currently active mod's own store.ini, replacing any existing
// row for that key and leaving every other key untouched. Returns false, writing nothing, if:
// there is no active mod; `key` fails mods_store_key_valid; a string `value` fails
// mods_store_string_valid; the active mod's directory is itself a symlink (or, on Windows, a
// reparse point) -- discovery already follows a symlinked mod *directory* deliberately (it is
// read-only there, and refusing it would break the legitimate workflow of symlinking a
// mod-in-development into the mods folder), but writing through one would let a validly-named
// mod redirect this write anywhere on disk, which is exactly the hazard this function exists to
// close; store.ini itself is a symlink (opened with O_NOFOLLOW on POSIX, or refused via a
// reparse-point check on Windows, where neither O_NOFOLLOW nor lstat/realpath exist); the
// existing store.ini is not a regular file, is larger than this host will rewrite, or contains an
// embedded NUL byte (refused rather than risking further corruption of a file that already
// doesn't look like one this function wrote); or rewriting it would grow past the same bound.
bool mods_store_set(const char *key, size_t key_len, const ModValue *value);
// Reads `key` from the currently active mod's own store.ini into *out, leaving *out untouched
// and returning false if there is no active mod, `key` is invalid, no row for it exists, or the
// store file is anything this function cannot safely make sense of (absent, a directory, a
// FIFO or other non-regular file, larger than this host will read, containing an embedded NUL,
// or holding a value with a type tag this build does not recognise) -- every one of those is
// "not found", not an error, since a mod asking for a value back is not asking to be crashed by
// whatever is wrong with the file on disk. Deliberately does NOT apply the write path's
// symlinked-directory refusal: discovery already follows a symlinked mod directory for reading,
// and refusing a read here would break that same legitimate mod-in-development workflow for no
// safety benefit, since nothing is written on this path.
bool mods_store_get(const char *key, size_t key_len, ModValue *out);

#define MODS_SWAPS_MAX 512
// For the engine (features.treasure_swap): true, with the replacement, when the mod that owns the
// swap table set one for treasure (id, subid) loading in (group, room).
bool mods_treasure_swap(int group, int room, uint8_t id, uint8_t subid, uint8_t *new_id, uint8_t *new_subid);
// For api.c: oracles.swap_treasure for the running mod. NULL on success, else why it was refused.
const char *mods_add_swap(int group, int room, int id, int subid, int new_id, int new_subid);

#define MODS_WRITES_MAX 256
typedef struct { uint16_t addr; uint8_t value; } ModWrite;
// For api.c: oracles.wr for the running mod's handler. NULL on success, else why it was refused.
const char *mods_queue_write(int addr, int value);
// For the app, between frames: moves up to `cap` of this frame's writes into `out` and empties the
// queue. A handler that failed left none of its writes in it.
int mods_take_writes(ModWrite *out, int cap);
// For api.c: the running mod's declared api, or 0 when no mod's Lua is running.
int mods_active_api(void);

// After mods_init: load the bundled tracker, or return a diagnostic. No disk writes.
const char *mods_load_official_tracker(void);

// Display metadata from mod.ini. Missing optional fields are empty; text is escaped.
typedef struct {
  char name[64], description[256], author[64], version[64];
  bool ages, seasons, changes_game;
} ModInfo;
ModInfo mods_info(int index);
ModInfo mods_official_tracker_info(void);
