# Randomizer development

This is the first part of a randomizer: a deterministic placement generator, a separate
reachability verifier, and a Lua mod exporter. It is **not a playable full-game randomizer yet**.
The first Seasons preset shuffles three optional D1 chests, with every progression reward left
vanilla. Full-game progression logic is not bundled. Solver tests also use invented locations.

The generator places items into currently reachable checks, backtracking when it runs out of
reachable checks. It then independently replays the completed placement in progression spheres:
all checks reachable before collecting that sphere's rewards. Every check and the goal must be
reachable before export. A fixed search budget fails explicitly instead of accepting an
unverified seed. The seed, world fingerprint and algorithm version determine the result.

## First playable slice: Seasons D1 optional chests

```sh
python3 tools/randomizer/generate.py --preset seasons-d1-optional --seed first --output out/randomizer
```

This shuffles the map, compass and Gasha-seed chest rewards in Gnarled Root Dungeon. Small keys,
boss key, bombs, ring, seed satchel, boss and essence remain vanilla. It is for **unlinked Seasons**.
It does not randomize Hero's Cave, the overworld or the rest of the game.

`seasons.py` resolves all 108 chest swap targets from the existing disassembly and uses dungeon
metadata/layout membership to find D1's seven chest rewards. Only the three explicitly named
optional reward symbols enter this pool; the other four are recorded as exclusions. Every check
carries a source reference. No extracted ROM/assets or copied chest tables are added to the repo.

The preset deliberately makes no claim about room-by-room progression spheres: its single
region represents completion along the unchanged vanilla D1 route. None of its randomized
items enables movement, combat or a locked door. This limited model must not be reused to shuffle
keys or progression items. Its spoiler includes that limitation and the excluded checks.

Install the generated folder under the app's `mods` folder before starting a **fresh test save**.
Use only one randomizer mod at a time and do not switch the seed while playing. The generated
`world.json` records the audited targets and `spoiler.json` records the exact placement.

Hero's Cave's sword is specifically excluded from future simple chest assumptions:
`object_code/seasons/interactions/herosCaveSwordChest.s` calls `giveTreasure` directly before
creating the sword treasure object. Swapping only that object would still grant the original
sword. That path needs dedicated engine support before its reward can be randomized correctly.

### Validation and limits

The ROM-free tests audit the source table, group aliases, D1 membership and all six arrangements.
`randomizer_replay` loads a generated seed into the native engine, checks all three replacement
objects against the ROM's treasure definitions, then replays the recorded introduction and D1
chests. It requires a Gasha seed from the former map chest and the D1 map from the former Gasha
chest to actually appear in inventory. It skips when the local ROM or recording is unavailable.

This is not a full-dungeon completion test. Changing a pickup changes its dialog timing, which
can desynchronise the vanilla movie. An attempted fixed-input completion check was inconclusive:
even its unchanged-reward baseline had not completed D1 by the 180,000-frame cutoff. Human
playtesting through the boss and essence, all permutations and the EU ROM remains outstanding.
Only the USA ROM is covered by the native replay. The preset is a development preview, not an
official catalogue entry.

## Development command

```sh
python3 tools/randomizer/generate.py /path/to/world.json --seed example --output out/randomizer
python3 tests/test_randomizer.py
```

The output is a new `randomizer-<game>-<fingerprint>` folder containing `mod.ini`, `main.lua`, `world.json` and
`spoiler.json`. The Lua registers fixed `oracles.swap_treasure` calls through API 2. It does not
roll new items at runtime or require Python while playing. Existing output is never overwritten.
The official Mods catalogue does not offer randomizer seeds yet.

For eventual gameplay tests, install only one generated randomizer folder at a time: only one
mod can own treasure swaps. Keep the same seed mod for the whole run and use a fresh test save.
The current host does not bind a seed identity to a save or prevent switching seeds mid-run.

## World format 1

The JSON object has these fields:

| Field | Meaning |
| --- | --- |
| `format` | `1` |
| `game` | `ages` or `seasons` |
| `regions` | Distinct region names |
| `start` | Starting region |
| `items` | Item name → `{ "id": treasure_id, "subid": treasure_subid }` |
| `starting_items` | Optional item name → count; assumed inventory, **not granted by the mod** |
| `pool` | Item names, including duplicates; exactly one entry per location |
| `edges` | Directed `{ "from": region, "to": region, "requires": alternatives }` connections |
| `locations` | `{ "name": unique_name, "region": region, "target": [group, room, old_id, old_subid], "requires": alternatives }` |
| `goal` | Requirement alternatives defining completion |

Requirements are a list of alternatives. Each alternative is an object whose item counts must
all be met. For example, `[{"sword": 1, "feather": 1}, {"switch-hook": 1}]` means sword **and**
feather, **or** switch hook. `[{}]` is always reachable; `[]` is never reachable. Missing edge or
location requirements mean `[{}]`. An empty goal is rejected. All requirement items must be
declared, and counts must be positive integers.

Treasure IDs and subids are checked against the same generated validity table the C mod host
uses. Duplicate swap targets are rejected: the current API cannot distinguish two rewards with
the same group, room, original ID and original subid. A world is limited to the host's 512 swaps.
Display names never become Lua source; only validated numeric arguments are emitted.

A spoiler contains the seed, algorithm version, SHA-256 of the canonical world JSON, placement
map and progression spheres. Verification checks both reachability and preservation of the item
pool. The guarantees are **only as accurate as the supplied world logic**.

## Work needed before a playable release

1. Author and audit Seasons locations and access rules, initially for an unlinked game. Record
   each check's source routine and swap target; account for fixed rewards and other vanilla
   progression outside the randomized pool. Keep extracted assets, ROMs and saves out of git.
2. Model transitions accurately: seasons, companions, dungeon keys, shops, currency, one-way
   paths and state changes. The current monotonic inventory model assumes permanent access
   once a region is reached; it does not model consumable keys, spending rupees, or being trapped
   after a one-way transition. Do not encode those as ordinary item counts and call it verified.
3. Implement progressive-item handling. Repeated inventory capabilities in the solver do not
   automatically upgrade sword/bracelet treasure levels in the game.
4. Cover rewards outside the treasure loader. Essences currently cannot be swapped; some
   direct grants and shop display items need separate support. Swapping a loaded treasure alone
   is not proof every dialogue, preview and event flag remains correct.
5. Bind seed/settings to a new save, add seed entry and spoiler controls to the Mods UI, and
   expose a tracker for checked locations and progression items.
6. Test generated seeds against the actual engine, then add Ages and linked-game logic.

The checked-in test graph is deliberately synthetic. Passing these tests validates the placement
and export mechanisms; it does not establish that an Oracle playthrough is completable.
