# Mods

A mod is a small Lua script that runs alongside the game and draws on top of it. It can read the
game's memory, put text, boxes and item icons on the screen, react to room changes, and remember
things between sessions. Unless its `mod.ini` says otherwise it cannot change the game: the game runs
exactly as it always does, and your mod watches and draws. A mod that declares `changes_game=yes` can
also swap the items treasures give and write the game's memory; see
[Changing the game](#changing-the-game).

This page is for writing one. For playing, see the [README](README.md).

Mods are new. The API below is small on purpose, and it is versioned (`api=2` today, and an `api=1`
mod keeps working unchanged), so a mod written today keeps working as it grows. See [Stable and unstable](#stable-and-unstable) for what is safe to
build on.

## Official mods

Open **Settings → Mods** from the launcher or pause menu. Official mods are bundled with the
app and work offline, including on Android. Essence Tracker is the first entry: turn it on to
show your eight collected essences in either game. Your selection is saved in settings.ini.
Changes take effect the next time you start a game; when playing, leave to the launcher first.
Official scripts run in the same sandbox as folder mods.

If you previously installed the `essence-tracker` folder manually, remove that copy before
enabling the bundled one. The loader reports the conflict instead of running two copies.
Folder mods are independent of the official-mod switches and continue loading as before.

## Installing a folder mod

A mod is a folder. Drop it in the `mods` folder inside the app's own folder — the same folder your
saves live in:

| System | Folder |
|---|---|
| macOS | `~/Library/Application Support/oracles-decomp/oracles/mods/` |
| Windows | `%APPDATA%\oracles-decomp\oracles\mods\` |
| Linux | `~/.local/share/oracles-decomp/oracles/mods/` |
| Android | the app's own storage |

Create `mods` yourself if it isn't there. Mods are picked up when a game starts, so restart the game
(leaving to the launcher and choosing a file again is enough) after adding or editing one. They load
in alphabetical order by folder name, and at most 32 load at once.

The Mods screen manages bundled official mods. For third-party folder mods there is no in-app
installer or on/off switch yet: install by copying the folder and remove by deleting it.

**Only install mods you trust.** A mod is a program, and the app keeps it boxed in deliberately: it
cannot open your files (only its own small save file, through `oracles.store`), start other
programs, or reach the network (see [What a mod can and cannot
see](#what-a-mod-can-and-cannot-see)), and one that loops forever or eats memory is cut off rather
than taking the game with it. But a box is not a promise that a hostile script can do nothing
at all — see [Limitations, honestly](#limitations-honestly).

## What's in a mod

Two files, in a folder named after the mod:

```
mods/
  essence-tracker/
    mod.ini
    main.lua
```

The folder name is the mod's **id**. It may contain only lowercase letters, digits and hyphens
(`a`-`z`, `0`-`9`, `-`), and must be under 64 characters. No spaces, no capitals, no underscores.

### mod.ini

One `key=value` per line. Blank lines are fine, `#` starts a comment, and spaces around the key and
the value are ignored (spaces inside a value are kept). Both Unix and Windows line endings work.
Lines longer than 255 characters are cut.

```ini
id=essence-tracker
name=Essence Tracker
description=Eight tiny markers show your collected dungeon essences.
version=1.1
author=you
api=1
games=ages,seasons
```

| Key | Required | What it does |
|---|---|---|
| `id` | yes | Must be exactly the folder name. A mod that disagrees with its own folder is refused — this is how a package says who it is rather than relying on where it was unzipped. |
| `api` | yes | Which version of the API the mod was written against. Must be 1 or more. This build supports up to **2**. |
| `games` | yes | `ages`, `seasons`, or `ages,seasons`. A mod is only loaded for a game it names. Anything else in the list is reported and ignored, so a typo costs you that one word, not the mod. |
| `name` | no | The display name. Defaults to the id. |
| `description` | no | Short, single-line summary shown in the mod card (up to 255 bytes after escaping; keep the whole line within 255 bytes). |
| `author` | no | Creator shown in the mod card (up to 63 bytes after escaping). |
| `version` | no | Version shown in the mod card (up to 63 bytes after escaping). |
| `changes_game` | no | `yes` lets an `api=2` mod change the game (see [Changing the game](#changing-the-game)). Leave it out, or write anything else, and the mod only watches and draws. |

Unknown keys are ignored. Optional display fields default to empty; control bytes are escaped.
The official catalogue embeds the same `mod.ini` used by folder installs, so changing its name,
description, author or version updates the card on the next build.

Get `id`, `api` or `games` wrong and the folder is not treated as a mod at all: it is skipped, with a
line on the app's log saying why. Declare an `api` higher than the build supports and the mod is kept
but switched off, so it shows up as broken rather than silently missing.

### main.lua

Plain Lua 5.4 source text — compiled bytecode is refused, not run. It is read and run when your mod
loads, and a typical one registers its handlers and then gets out of the way:

```lua
oracles.log("hello from my first mod")

oracles.on_frame(function()
  -- a white 8x8 box near the top-left, redrawn every frame
  oracles.draw_rect(8, 8, 8, 8, 0x7fff)
end)
```

Everything a mod does afterwards happens inside the handlers it registered here. If `main.lua` is
missing, throws an error, loops forever or runs out of its budget, the mod is switched off for the
session and the game carries on without it.

## How a mod runs

- `main.lua` runs **once per scan** of the mods folder, in a brand new Lua world with nothing carried
  over from the last one. The app scans when the launcher opens and again when a game starts, so
  expect your script to run more than once per launch. Set up there; don't compute anything heavy.
- The launcher's scan hasn't got a game yet and assumes Seasons, so a mod that names only `ages`
  doesn't load at all until Ages is actually played. Nothing is drawn from the launcher either way.
- Your `oracles.on_frame` handler runs **once per drawn frame while you are playing** — about 60
  times a second. Nothing runs in the launcher or while the pause menu is open.
- Your `oracles.on_room_change` handler runs after it, on frames where the room changed (including
  the very first frame).
- The drawing surface is **wiped at the start of every frame**. Whatever you want visible, draw it
  again this frame.
- What you draw goes on the picture *before* the screen filter, so it scales and filters along with
  the game instead of sitting on top looking different.
- Each mod gets its own private Lua world. Two mods cannot see each other's variables, and one mod
  failing does not affect another.

The surface is the size of the picture: **160x144**, or **256x144** with widescreen on, with `x` to
the right and `y` down from the top-left. Drawing outside it is clipped, not an error. Ask
`oracles.screen()` for the width rather than assuming.

Colours are **RGB555** integers, the format the renderer itself uses: five bits each of red, green
and blue, red lowest.

```lua
local function rgb(r, g, b) return r + g * 32 + b * 1024 end   -- each 0-31
local WHITE, RED, GREEN, BLUE = 0x7fff, rgb(31, 0, 0), rgb(0, 31, 0), rgb(0, 0, 31)
```

Every pixel you draw is opaque; there is no blending. Where you draw nothing, the game shows through.

## API reference

Everything a mod can call lives in the `oracles` table. This is the whole of it — twenty-one functions.
Anything not listed here does not exist (and a test fails the build if this list and the real one
ever disagree).

Arguments called *integer* must be whole numbers: `5` and `5.0` are both fine, `5.5` is **an error**
rather than a rounded 5, so round it yourself with `math.floor`. A number far too big for a 32-bit
integer is clamped to the end of that range rather than wrapping around. Anything that isn't a
number at all raises an error too, which switches your mod off unless you catch it with `pcall`.

### `oracles.camera()`

Returns where the picture is looking, as `{ x, y, room_x, room_y }`, or `nil` if there is no picture
yet.

```lua
local c, s = oracles.camera(), oracles.sprites()
if c and c.room_x and s and s[1] then
  oracles.log("a sprite at room pixel " .. (c.room_x + s[1].x) .. "," .. (c.room_y + s[1].y - 16))
end
```

`room_x` and `room_y` are the pixel of the current room shown at the top-left of the play area, the
part of the picture under the 16-pixel status bar. They match `oracles.sprites()`, which describes
the same drawn picture: a sprite at picture position `(sx, sy)` is at `(room_x + sx, room_y + sy - 16)`
in the room. They can be a few pixels negative, or a few past the far edge, while the screen
shakes. **They are left out (`nil`)** whenever the picture is not showing the room in normal play:
menus, cutscenes, the scroll between two rooms, and side-scrolling areas.

`x` and `y` are the Game Boy's own background scroll registers for the top of the play area, 0-255,
and are always there when there is a picture. They wrap around at 256 and include an offset for
where the game placed the room in video memory, so for most purposes `room_x`/`room_y` are the ones
you want.

### `oracles.collision(x, y)`

Returns how solid the metatile at metatile coordinates `(x, y)` in the current room is, as a table,
or `nil` if there is no such tile. This is the value the game itself tests when something moves, so
it includes whatever the room changed on top of the tileset's defaults (see `oracles.solidity`).

```lua
local c = oracles.collision(4, 2)
if c and c.solid then oracles.log("wall at 4,2") end
```

The table:

| field | meaning |
|---|---|
| `raw` | the game's own byte, 0-255 |
| `special` | `true` when `raw` is `0x10` or more (see below) |
| `solid` | `true` only when the whole metatile is solid; `nil` when `special` |
| `quarters` | `{ tl, tr, bl, br }`, a boolean for each 8x8 quarter; `nil` when `special` |

Below `0x10`, the byte is four bits, one per quarter: `0x08` top-left, `0x04` top-right, `0x02`
bottom-left, `0x01` bottom-right. So `0x0f` is a solid block, `0x0c` a block whose top half is solid,
`0x05` a block whose right half is solid, and `0` is open ground. `special` values (holes, ledges,
bridges, water edges and the like) point into a table in the ROM that this API does not expose yet,
and what they mean depends on what is moving, so `solid` and `quarters` are left out rather than
guessed. `raw` is still there.

`x` and `y` follow the same rules as `oracles.metatile`: it returns `nil` with no game running, or for
a coordinate outside `oracles.room()`'s `width`/`height`.

### `oracles.draw_icon(item, x, y)`

Draws an item's own icon from your ROM, 16 pixels tall and 8 or 16 wide, with its top-left corner at
`x`, `y`. `item` is the game's item id (integer, only the low 8 bits are used). Returns `true` if
that item has an icon — including when you placed it off-screen — and `false` if it has none, or if
the ROM isn't available this frame. An item that comes in levels (the slingshot, for instance) is drawn at
the level you currently have.

```lua
oracles.draw_icon(0x13, 4, 120)     -- 0x13 is the slingshot
```

The icon is drawn in a grey ramp, not the game's own colours: the status bar draws these as sprites
with live palettes, which a mod has no access to.

### `oracles.draw_rect(x, y, w, h, colour)`

Fills a rectangle `w` by `h` pixels with `colour`. All five are integers. A width or height of zero
or less draws nothing. Returns nothing.

```lua
oracles.draw_rect(0, 0, oracles.screen(), 1, 0x7fff)   -- a white line across the top
```

### `oracles.draw_text(x, y, text, colour)`

Draws `text` in `colour`, with the top-left of the first character at `x`, `y`. Each character is 8
pixels wide and 16 tall, so text advances 8 pixels per character. Returns nothing.

```lua
oracles.draw_text(4, 4, "HELLO", 0x7fff)
```

The glyphs are the game's own font, read from your ROM, so stick to the characters the game itself
shows: capitals, digits and simple punctuation are the safe set. Bytes above 127 are skipped. A
string containing a zero byte stops there — only the part before it is drawn.

### `oracles.game()`

Returns `"ages"` or `"seasons"`, the game being played. Safe to call while `main.lua` is still
loading.

```lua
local in_seasons = oracles.game() == "seasons"
```

Worth calling even in a mod that only names one game in its `games=` line, because almost every
memory address differs between the two.

### `oracles.load(key)`

Reads back a value your mod saved with `oracles.store`, or `nil` if there is none. `key` is a string.
Numbers come back as the same kind of number they went in as — an integer stays an integer.

```lua
local best = oracles.load("best_time") or 0
```

A key that breaks the rules listed under `oracles.store`, or a `store.ini` the app cannot make sense
of, simply reads back `nil` — reading never raises an error, because a mod asking for a value back
isn't asking to be switched off over whatever is wrong with a file on disk.

### `oracles.log(text)`

Writes a line to the app's log. `text` is a string. Returns nothing.

```lua
oracles.log("essences: " .. count)
```

This is your `print` — `print` itself is not available (see
[What a mod can and cannot see](#what-a-mod-can-and-cannot-see)). The log is the app's standard
error: run the app from a terminal to watch it, or `adb logcat -s oracles` on Android. Bytes that
aren't printable text are shown as `\xNN` escapes rather than being sent to your terminal raw. Lines
over about 500 characters are cut, and logging on every single frame will flood the log — log when
something changes.

### `oracles.metatile(x, y)`

Returns the metatile id (an integer 0-255) at metatile coordinates `(x, y)` in the current room, or
`nil` if there is no such tile.

```lua
local r = oracles.room()
if r then
  local id = oracles.metatile(0, 0)   -- top-left metatile of the current room
  if id then oracles.log("top-left metatile is " .. id) end
end
```

`x` and `y` are metatile coordinates, the same unit `oracles.room()`'s `width`/`height` use — not
pixels and not the 8x8 tiles the game's own `wRoomWidth`/`wRoomHeight` count in.

**Returns `nil`, never a tile, for three different reasons, all collapsed into the same answer:**
no game is running yet; `(x, y)` is negative or at/past `oracles.room()`'s `width`/`height`; or (not
reachable with any room the game actually has, but checked anyway) the coordinate would fall
outside the memory this id actually lives in. The room's own storage is laid out in a fixed-width
row wider than most rooms are, so an `x` that looks plausible for a wide room can still be one
column past a narrower one's real width — that column holds unused padding, not a tile, and this
function could have handed it back as if it were one. It doesn't: a padding byte happens to look
exactly like the legitimate id `0`, so returning it would be a value indistinguishable from a real
tile and a mod would have no way to tell the two apart. `nil` is the only answer that isn't a lie,
the same reasoning `oracles.room()` already follows for "no game running".

### `oracles.on_frame(f)`

Registers `f` to be called once per drawn frame while you are playing. `f` takes no arguments.
Returns nothing. Calling this again replaces the handler, so there is exactly one per mod.

```lua
oracles.on_frame(function() oracles.draw_rect(8, 8, 8, 8, 0x7fff) end)
```

### `oracles.on_room_change(f)`

Registers `f` to be called when the room changes, with the new room number as its one argument. It
also fires on the first frame after a game starts. Returns nothing, and replaces any previous
handler.

```lua
oracles.on_room_change(function(room) oracles.log("room " .. room) end)
```

### `oracles.rd(addr)`

Reads one byte of the game's memory and returns it as an integer 0-255. `addr` is an integer address.

```lua
local essences = oracles.rd(oracles.game() == "seasons" and 0xc6bb or 0xc6bf)
```

**Only two windows are readable:** `$c000`-`$cfff` (work RAM bank 0) and `$ff80`-`$fffe` (high RAM).
A mod that declares `api=2` gets a third: `$d000`-`$dfff` as work RAM **bank 1**, where the game keeps
Link, enemies and the other objects, whatever bank the game has switched in at that moment. Anything
else (and `$d000`-`$dfff` for an `api=1` mod) **reads back 0** instead of raising an error. A
plausible-looking address outside your windows gives you a silent zero, not a complaint, so if a value
is stubbornly 0 check the address is in range before suspecting your arithmetic.

Addresses are the game's own, and nearly all of them differ between Ages and Seasons — so read them
through `oracles.game()`, as above. The community Oracle of Ages/Seasons disassembly is where the
names and addresses on this page come from.

### `oracles.wr(addr, value)`

Needs `api=2` and `changes_game=yes` (see [Changing the game](#changing-the-game)). Writes one byte of
the game's work RAM. `addr` is `$c000`-`$cfff` (bank 0) or `$d000`-`$dfff` (bank 1, the objects, the
same bank `oracles.rd` reads there); `value` is 0-255. Returns nothing.

```lua
local health, max = 0xc6aa, 0xc6ab                 -- Ages
if oracles.game() == "seasons" then health, max = 0xc6a2, 0xc6a3 end
oracles.on_room_change(function()
  oracles.wr(health, oracles.rd(max))              -- full health in every new room
end)
```

- Only inside `oracles.on_frame` and `oracles.on_room_change`. From `main.lua` it is an error: the
  game's memory is not set up yet while your mod loads.
- Writes wait until every mod has run this frame, and land before the game's next frame. A read in the
  same frame still sees the old value.
- A handler that errors or runs out of budget writes nothing at all, not even the writes it made
  before failing.
- 256 writes per frame, across all mods; the 257th is an error.
- `$c0b0`-`$c2ff` is refused with an error. It holds the game's stacks and thread state, and a wrong
  byte there stops the game outright rather than just confusing it.
- Anything else is allowed, which means a wrong address can corrupt the save you are playing on.

### `oracles.room()`

Returns a table describing the current room: `{ group, room, width, height }`. Returns `nil` if no
game is running yet.

```lua
local r = oracles.room()
if r then oracles.log("room " .. r.room .. " is " .. r.width .. "x" .. r.height .. " metatiles") end
```

**`width` and `height` are in metatiles, not 8x8 tiles.** A metatile is the game's own unit of
meaning — a bush, a wall, a stair is one metatile, two tiles wide and two tiles tall — and is the
unit a future id-to-metatile mapping will use. A room that is 20 tiles wide reports `width = 10`
here, not 20; halve any tile-based measurement you already have in mind, or double this one, before
comparing the two.

`room` is the same number `oracles.on_room_change`'s argument gives you. `group` and `room`
together identify the room uniquely across the whole map — `room` numbers repeat across different
groups, so `room` alone is not enough to tell two rooms apart.

### `oracles.screen()`

Returns the picture's width in pixels: 160, or 256 when the player has widescreen on. The height is
always 144. Safe to call while `main.lua` is still loading.

```lua
oracles.draw_rect(oracles.screen() - 8, 0, 8, 8, 0x7fff)   -- top-right corner, either width
```

### `oracles.setting(name, default)`

Registers one settings row for your mod — the menu that will show them is still to come, see the
caveats below — and returns a **function** that reads the row's current value. `name` is a short
string (up to 31 characters). There are two shapes:

```lua
local show = oracles.setting("Show pips", true)        -- an ON/OFF row; show() returns a boolean
local size = oracles.setting("Pip size", {1, 8, 5})    -- a number row {min, max, default}
                                                       -- size() returns an integer
```

Read it by calling the function, every time you need it — don't store the value in a local, or you
will miss the player changing it. Registering the same name twice from the same mod gives you back
the same row. Two different mods can use the same name without colliding. There are 32 rows in total
across all mods; asking for more raises an error.

Two honest caveats: **there is no settings menu for these yet** (it comes with the mod manager), so
today the function always returns your declared default; and a row's value is **not saved** — every
session starts at the default. Use `oracles.store` for anything that must survive a restart.

### `oracles.solidity(id)`

Returns how solid metatile `id` is in the current tileset by default, in the same table
`oracles.collision` returns. `id` is a metatile id, an integer 0-255, as `oracles.metatile` gives you;
anything outside 0-255 **is an error**, not some other tile's answer. Returns `nil` if no game is
running.

```lua
local id = oracles.metatile(0, 0)
local s = id and oracles.solidity(id)
if s and s.solid then oracles.log("metatile " .. id .. " is a wall in this tileset") end
```

This is the tileset's table, which the game reads when it loads a room. **Rooms change some of their
tiles on top of it** (one Seasons overworld room changes 13 of its 80), so to know what is solid
where, use `oracles.collision(x, y)`. `solidity` is for questions about a kind of tile: "what does
metatile `0x12` look like, everywhere it appears".

### `oracles.sprites()`

Returns the sprites on screen as an array of `{ x, y, tile, attr }`, in the hardware's own order, or
`nil` if there is no picture yet. This is where the game puts Link, enemies, items and effects.

```lua
local s = oracles.sprites()
if s then
  for _, sp in ipairs(s) do oracles.draw_rect(sp.x, sp.y, 8, 1, 0x001f) end   -- a red tick over each
end
```

`x` and `y` are the sprite's top-left corner in the 160x144 game picture. In widescreen the game
picture sits in the middle of a wider screen, so add `(oracles.screen() - 160) // 2` to `x` before
drawing on it. They can be a little negative for a sprite partly off the top or left edge. `tile` is
the sprite's tile number (0-255) and `attr` its attribute byte, as the Game Boy Color stores them
(palette, flips, priority).

Sprites the hardware does not show are left out, so you get the handful that are visible rather than
all 40 slots. Every sprite is 8 pixels wide; this game draws them 16 tall, and one character is
usually several sprites side by side. The values are the ones the picture you are looking at was
drawn with, not ones the game has already moved on to for the next frame.

### `oracles.store(key, value)`

Saves `value` under `key` in your mod's own `store.ini`, next to its `main.lua`, and returns nothing.
It persists across sessions, and each mod has its own file that no other mod can read.

```lua
oracles.store("best_time", frames)
```

`value` may be a string, a number or a boolean — nothing else. The rules, which are enforced by
refusing rather than by quietly mangling your data:

- `key`: 1 to 63 characters, printable ASCII, no `=`, not starting with `#` or a space, not ending
  with a space.
- a string `value`: up to 503 characters, printable ASCII (`=` is fine). No newlines, tabs or zero
  bytes.

Break one of those and this raises a Lua error — which switches your mod off unless you wrap the call
in `pcall`. If you are storing something a player typed or a value from the game, check it first.

### `oracles.swap_treasure(group, room, id, subid, new_id, new_subid)`

Needs `api=2` and `changes_game=yes` (see [Changing the game](#changing-the-game)). From now on,
whenever treasure `(id, subid)` appears in room `room` of group `group`, it is treasure
`(new_id, new_subid)` instead. Returns nothing.

```lua
-- Ages: the sword in group 0, room $39 becomes bombchus
oracles.swap_treasure(0, 0x39, 0x05, 0, 0x0d, 0)
```

- A treasure is almost any item Link holds up or picks up: chest contents, gifts from people, items
  lying in a room, what a shopkeeper hands over. `id` and `subid` are the treasure index and subid
  from the disassembly (`constants/common/treasure.s`, `data/{game}/treasureObjectData.s`).
  `group` and `room` are what `oracles.room()` returns.
- The place keeps its own way of showing the item (rising out of a chest, a puff of smoke, falling
  from the top of the screen, buried) and the room flag that remembers it was taken. Everything else
  comes from the new treasure: what Link gets, how he holds it up, the text and the sprite.
- Calling it again with the same first four arguments replaces that swap. It works from `main.lua`
  and from a handler, and applies the next time that treasure appears.
- `(new_id, new_subid)` must be a treasure of the game being played: one of its listed subids, or
  subid 0 for a treasure without subids. Anything else is an error, and so is a `group` outside 0-7
  or a `room`, `id` or `subid` outside 0-255.
- One mod owns swaps: the first one to call this. Another mod calling it gets an error.
- 512 swaps at most; a 513th different one is an error.
- When your mod is switched off (an error, a budget), its swaps go with it.
- Items that come in levels (swords, shields, the seed satchel): rewrite the swap from
  `oracles.on_frame` when the inventory changes, so the next one is the right level. Getting an item
  takes well over a second, so there is time.
- Not covered: items that are not treasures (the essences, and a few items the game hands over
  straight from its code), and the items a shop shows on its counter before you buy.

### `oracles.tile_palette(i)`

Returns background palette `i` as an array of four colours, or `nil` if there is no picture yet. `i`
is an integer 0-7; anything else **is an error**. Each colour is a 15-bit RGB555 integer, the same
format `oracles.draw_rect` takes, so you can draw with the game's own colours:

```lua
local p = oracles.tile_palette(0)
if p then oracles.draw_rect(4, 4, 8, 8, p[4]) end   -- colours are p[1] to p[4]
```

These are the palettes the picture you are looking at was drawn with, so they follow the season,
the tileset and every fade as they happen on screen. Sprite palettes are not exposed.

## Stable and unstable

The functions above are the **stable core**, and the reason they can promise that is worth
understanding, because it explains why some obvious-looking additions are not there.

`oracles.rd` takes a **raw address**. Addresses are a property of the game — they are fixed by the
ROM you own, the same bytes in the same places as in 2001. Nothing this project does to its own code
can move `wEssencesObtained`. So a mod built on `oracles.rd` keeps working through any amount of
refactoring on our side: there is nothing of ours in the middle to break. The same goes for the
drawing functions (pixels on a 144-line picture), `oracles.game()`, `oracles.screen()`, the two
event handlers, and the store. They describe the game and the screen, not our implementation.
`oracles.swap_treasure` names treasures and rooms by the game's own numbers, so it is stable for the
same reason.

What is **not** here, and will arrive clearly marked unstable when it does:

- **Symbol hooks** — running your code when a named routine of ours runs. Those names are *our*
  function names, not the game's data. They move, split and get renamed as the port is worked on, so
  a mod built on them can break on any build. That is exactly why they are a later stage and will be
  marked unstable rather than folded in beside `oracles.rd`.
`api=2` is the contract. It only added functions, for mods that ask for them, so an `api=1` mod runs
exactly as before. If a future build ever has to change the meaning of something above, it will do it
under a new `api` and keep honouring the old ones, rather than quietly changing what your mod does.

## Changing the game

A mod whose `mod.ini` has both `api=2` and `changes_game=yes` can swap the item a treasure gives, with
`oracles.swap_treasure`, and write the game's memory, with `oracles.wr`. Without both lines those two
functions raise an error, so an `api=1` mod can never change the game.

This is what a randomizer needs, and it is also enough to ruin a playthrough: give Link the wrong
item in the wrong place, or write the wrong byte, and the game can become impossible to finish or the
save can be corrupted. The limits on the two functions keep the app running; they cannot keep the
game's own logic happy. Only install mods like this from
sources you trust, and keep a copy of your save.

## What a mod can and cannot see

A mod gets the Lua language, three standard libraries, and the `oracles` table. The complete list of
globals inside a mod is:

```
_G  _VERSION  assert  collectgarbage  error  getmetatable  ipairs  math  next  oracles
pairs  pcall  rawequal  rawget  rawlen  rawset  select  setmetatable  string  table
tonumber  tostring  type  xpcall
```

So `string`, `table` and `math` are all there, and so is ordinary Lua: closures, metatables,
`pcall`, string patterns.

**Withheld**, and the first one will catch you out:

| Gone | Instead |
|---|---|
| `print`, `warn` | `oracles.log` |
| `io`, `os` | nothing — a mod cannot read or write files, start programs, or see the clock. Use `oracles.store` to save, and count frames rather than seconds. |
| `require`, `package` | nothing — a mod is one `main.lua`, with no second file to load |
| `load`, `loadfile`, `dofile`, `string.dump` | nothing — a mod cannot build and run new code at runtime |
| `coroutine` | nothing |
| `debug` | nothing |
| `utf8` | `string`, on plain bytes |

`setmetatable` is there but refuses a `__gc` field: a mod cannot install a finalizer.

Those names are gone for a reason in each case rather than as a blanket policy. `debug.sethook`
would let a mod remove the very hook that keeps it to its budget; `coroutine` offered a way to buy a
fresh budget forever; `load` and friends could be handed a crafted binary chunk; `io` and `os` reach
your filesystem and could block the game indefinitely or exit the app outright. `utf8` is simply not
one of the three libraries a mod is granted.

## Limits, and why

Your mod is given a bounded amount of work to do, so a buggy one slows nothing down for long:

| Limit | Value | When |
|---|---|---|
| Memory | **8 MiB** of Lua data | the whole session, main script and handlers together |
| Instructions | 50,000,000 | loading `main.lua` |
| Time | 2 seconds | loading `main.lua` |
| Instructions | 1,000,000 | each single handler call |
| Time | **4 milliseconds** | each single handler call |

A frame is 16.7 ms, which is where the 4 ms comes from: a handler that needs longer than a quarter of
a frame is not something a game can run 60 times a second. These are generous for what mods actually
do — a fresh Lua world costs about 19 KB, and a script building a thousand-entry table of tables
costs about 250 KB, against the 8 MiB cap; a typical handler finishes so far inside 4 ms that the
check never fires once.

Go past any of them and **your mod is switched off for the rest of the session**, with a reason, and
the game keeps running. It is not retried next frame: a handler given a fresh budget every frame
would be a permanently stuttering game rather than a disabled mod. The same happens if your code
raises an error it doesn't catch.

If you hit the memory cap, you are probably accumulating something per frame. If you hit the
per-frame limits, do the work once in `main.lua` or on `oracles.on_room_change`, and keep
`oracles.on_frame` to drawing.

## Limitations, honestly

Things worth knowing before you spend an evening on them.

**What is guaranteed.** A mod that loops forever, allocates without limit, writes a pathological
string pattern, installs a looping finalizer, ships a binary chunk instead of text, or throws an
error is refused or switched off, by itself, without affecting the game, the other mods, or the rest
of the scan. That is the large majority of what goes wrong, and all of it is tested.

**The one hang that is still possible.** A script that calls `pcall` on itself recursively — the
shape `local function guard() while true do pcall(guard) end end guard()` — can wedge the game. It
livelocks inside Lua's own error-recovery machinery, where no instruction ever finishes executing, so
neither the instruction budget nor the time limit is ever reached to cut it off. This is Lua's own
behaviour, not something this app does, and it takes deliberate construction rather than happening by
accident. It is left unfixed on purpose for now, because the alternative is machinery
(watchdog threads, abandoning Lua worlds mid-run) out of proportion to the risk. It is also the
concrete reason for the advice above: **install mods only from sources you trust.**

**`oracles.rd` sees two windows, not all of memory.** `$c000`-`$cfff` and `$ff80`-`$fffe`, plus
`$d000`-`$dfff` as bank 1 for an `api=2` mod. The other work RAM banks are not exposed, and an address
outside the windows reads 0 rather than raising an error. See `oracles.rd` above.

**Numbers are clamped, not wrapped.** Lua integers are 64-bit, the drawing functions take 32-bit
ones. Pass `4294967301` as a width and you get a very wide rectangle (clipped to the screen), not the
five-pixel one that wrapping the number around would have given you.

**Zero bytes in strings are handled, differently in two places.** `oracles.log` escapes a zero byte
as `\x00` and logs the whole string; `oracles.draw_text` stops at it, drawing only the part before.
`oracles.store` refuses a key or value containing one.

**Settings do not persist, and have no menu yet.** See `oracles.setting`.

**Folder mods have no manager yet.** The official catalogue has saved On/Off switches, but folder
mods have no enable/disable or zip install UI. The sign a player gets that
something went wrong is a **Mod disabled** message at the bottom of the screen the moment a mod is
switched off; the reason goes to the log.

## A worked example: an essence tracker

Eight tiny pips near the bottom of the screen, one per essence, lit as you collect them. The
bundled tracker uses a 33x5-pixel strip in the widescreen side area or at bottom centre.

The essences live in one byte, `wEssencesObtained`, with one bit per essence — and that byte is at a
**different address in each game**, which is what `oracles.game()` is for.

`mods/essence-tracker/mod.ini`:

```ini
id=essence-tracker
name=Essence Tracker
description=Eight tiny markers show your collected dungeon essences.
version=1.1
api=1
games=ages,seasons
```

`mods/essence-tracker/main.lua`:

```lua
-- Read the current save directly: loading a state or switching files can remove
-- essences as well as add them. No persistent progress or game writes needed.
-- wEssencesObtained (USA and EU): Ages $c6bf, Seasons $c6bb.
local ESSENCES = oracles.game() == "seasons" and 0xc6bb or 0xc6bf
local show = oracles.setting("Show essences", true)

local function rgb(r, g, b) return r + g * 32 + b * 1024 end
local BACK = rgb(2, 3, 5)
local EDGE = rgb(9, 11, 13)
local EMPTY = rgb(5, 6, 8)
local GOLD = rgb(31, 25, 7)
local LIGHT = rgb(31, 31, 21)

local function pip(x, y, have)
  oracles.draw_rect(x, y, 3, 3, have and GOLD or EDGE)
  oracles.draw_rect(x + 1, y + 1, 1, 1, have and LIGHT or EMPTY)
end

oracles.on_frame(function()
  if not show() then return end
  -- room_x is absent in menus, cutscenes, room transitions and side-view areas.
  local camera = oracles.camera()
  if not camera or camera.room_x == nil then return end

  local bits = oracles.rd(ESSENCES)
  local width = oracles.screen()
  local side = (width - 160) // 2
  -- 33x5 pixels: one row, no count or large backing panel. In widescreen
  -- it fits entirely inside the side strip; otherwise centre it at the bottom.
  local x = side >= 33 and (side - 33) // 2 or (width - 33) // 2
  local y = 137
  oracles.draw_rect(x, y, 33, 5, BACK)
  for i = 0, 7 do pip(x + 1 + i * 4, y + 1, ((bits >> i) & 1) == 1) end
end)

oracles.log("essence-tracker ready")
```

Points worth taking from it:

- **Pick addresses by game, once, at load.** `oracles.game()` works while `main.lua` is still
  running, so resolve the address there rather than branching on every frame.
- **Ask for the width.** `oracles.screen()` is 160 or 256; using it lets the tracker fit in a widescreen side strip
  or centre itself on the original picture.
- **Draw every frame.** The surface is wiped each frame; there is nothing to erase and nothing that
  stays.
- **Read settings through the function.** `show()`, never a cached `show` value.
- **Keep `oracles.on_frame` cheap.** One memory read and seventeen tiny rectangles is nothing against the 4 ms
  limit.

To make it remember something — the most essences you have ever had, say — add this to the same
file:

```lua
local best = oracles.load("best") or 0

oracles.on_room_change(function()
  local n, bits = 0, oracles.rd(ESSENCES)
  for i = 0, 7 do n = n + ((bits >> i) & 1) end
  if n > best then
    best = n
    oracles.store("best", best)      -- written to this mod's own store.ini
  end
end)
```

On a room change rather than every frame, deliberately: `oracles.store` rewrites its whole file on
every call, which is not something to do sixty times a second.

## When something goes wrong

- **Mod disabled** at the bottom of the screen means a mod was switched off this session. The reason
  goes to the log.
- **Nothing happens at all.** The usual causes, in order: the folder name and `id=` don't match;
  `games=` doesn't name the game you're playing; the folder is not in the `mods` folder next to your
  saves; you didn't restart the game after adding it.
- **Reading the log.** Run the app from a terminal and watch its standard error; on Android,
  `adb logcat -s oracles`. Every refusal says which mod and why.
- **A value is always 0.** Check the address is inside `$c000`-`$cfff` or `$ff80`-`$fffe`, and that
  you used the right game's address.
