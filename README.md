# The Legend of Zelda: Oracle of Ages & Oracle of Seasons

Native macOS, Windows, Linux and Android versions of **Oracle of Ages** and **Oracle of Seasons**
(Capcom, 2001), rebuilt from the games' own code in C. There is no emulator underneath: the games
run as native programs, which is what makes widescreen, save states and save sync across your
devices possible.

**No game data is included.** You need your own Oracle of Ages and/or Oracle of Seasons ROM (USA or
Europe).
The app reads it once, keeps the graphics, sound and data, and never needs it again.

## Quality-of-life improvements

What this port adds on top of the original games:

- **Widescreen (16:9)** — see the rooms around you, not just the one you're in
- **Save states** — 4 slots with thumbnails, plus a Resume state from wherever you last quit
- **Save sync** — your saves and states follow you between desktop and phone, no passwords
- **Remappable controls** — every button, for keyboard and gamepad
- **Fast-forward** — hold Tab, or the right trigger; 3x by default, 2x to 8x in Settings
- **Screen filters** — sharp, scanlines, LCD grid, CRT, or the xBRZ and hqx pixel-art scalers, and true Game Boy Color colours
- **Fast text and faster menus** — optional, off by default
- **Quick swap and 4 item slots** — optional, off by default
- **Mods** — small Lua scripts that read the game and draw on top of it; see [MODDING.md](MODDING.md)

The gameplay toggles are all off until you turn them on, so the default is the game as it shipped.

---

**AI DISCLOSURE**

This native implementation has used help from generative AI, specifically Claude Opus.

If you don't want to use it for that I completely understand.

---

## Download

Get the latest build for your system from the
[**nightly release**](https://github.com/kirby-letsgo/oracles-decomp/releases/tag/nightly), rebuilt
automatically on every change:

| System | File | First launch |
|---|---|---|
| macOS 11+ (Apple Silicon and Intel) | `Oracles-macOS.dmg` | Open the DMG and drag Oracles out. It isn't notarized, so the first time right-click it and choose **Open**. |
| Windows 10/11 (64-bit) | `Oracles-windows-x64.zip` | Unzip and run `Oracles.exe`. If SmartScreen warns about an unknown publisher, choose **More info → Run anyway**. |
| Linux (x86_64) | `Oracles-linux-x86_64.AppImage` | `chmod +x` the file, then run it. |
| Android 8+ (arm64) | `Oracles-android.apk` | Install the APK (allow installs from your browser or file manager). |

Each desktop download is a single self-contained file; nothing else needs installing.

## Adding your ROM

The app opens on its main menu, with a tab each for Ages and Seasons. A game you have no ROM for yet
shows **Not installed** and an **Add ROM...** row — choose it and the file picker opens. Do the same
on the other tab to add the second game. Nothing is asked for before the menu appears.

The ROM can be a plain `.gbc`/`.gb` file **or a `.zip` or `.gz`** holding one, so a freshly
downloaded archive works as it came. From a zip the `.gbc`/`.gb` file inside is taken, otherwise the
largest one. `.7z` isn't read — extract it first.

Your ROM has to be the USA or the European release of either game; the app checks it and says so if
it isn't. The European ROMs play in English, French, German, Italian or Spanish: the game asks the
first time it starts, and B on the file menu brings the language screen back. Each ROM keeps its own
saves and save states, so a game's USA and European files never mix; adding the other release of a
game makes it the one that tab starts, and adding the first one again switches back, its saves
where you left them. On Android, copy the ROM to the phone's Downloads first so the picker can reach
it.

## Playing

A game's tab leads with **Resume** (the state from when you last quit) and its four save-state slots,
each with when it was saved and the newest marked **Latest**; choosing one starts from it. Below them,
**Save files** opens the game's own three files (name, hearts, essences) — choosing one boots
straight into it — and **Title screen** starts the game from the beginning. Every menu works with a
keyboard, a controller, the mouse, or by tapping its rows.

### Controls

| | Keyboard | Gamepad (Xbox / PlayStation) |
|---|---|---|
| Move | Arrow keys | D-pad or left stick |
| A | `X` | A / Cross |
| B | `Z` | X / Square |
| Start | Return | Start / Options |
| Select | Backspace or Right Shift | Back / Share |
| Pause | Esc | Guide / PS button |
| Fast-forward | Hold Tab | Hold RT / R2 |
| Quick swap | `C` | LB / L1 |
| Item X / Item Y | `A` / `S` | Y / Triangle, B / Circle |
| Mute | `M` | |
| Fullscreen | F11 or Cmd+F | |
| Save / load state slot 1 | Cmd+S / Cmd+R | |

Every button, including Pause, Fast-forward, Swap and the item buttons, can be remapped in
Settings > CONTROLS: pick a row, then press the key or controller button (triggers included) you
want for it. Quick swap and the item buttons only do something once their gameplay toggles are on.
The window resizes in whole-pixel steps.

The status bar, the item boxes and the menu hints name the key or controller button each action is
bound to, and switch between keyboard and controller as you use them. Settings > Pad button names
picks how a controller's buttons are named: Auto follows the connected pad (Xbox for anything it
doesn't recognise), or choose Nintendo (the Game Boy's own B and A), Xbox or PlayStation.

### Settings

From the launcher or the pause menu: volume, screen filter (sharp, scanlines, LCD grid, CRT, and the
two pixel-art scalers XBRZ and HQX, which round off jagged diagonals instead of turning each pixel
into a square — they are two different algorithms, so it is worth trying both), scale
(**pixel** for whole-pixel steps, **fill** for the largest size that fits), Game Boy Color colours,
fullscreen, fast-forward speed (2x to 8x, 3x by default, the same on any display), controls and
[portable mode](#portable-mode).

There are also quality-of-life toggles, **all off by default**:

- **Fast text** and **faster menus**
- **Quick swap**: `C` swaps the A and B items
- **4 slots**: `A`/`S` become two more item buttons — highlight an item in the inventory and press
  one to assign it, then hold it in play to use that item. In widescreen they get their own X and Y
  boxes at the left of the status bar, so all four items are on screen at once; without widescreen
  the boxes go in the black bars either side of the picture when the window is wide enough

### Save states

Esc pauses the game: Resume, Save state, Load state (4 slots, each with a thumbnail and the time
it was saved, to the second) and Quit to menu, which saves a Resume state and returns to the main
menu. Cmd+S and Cmd+R are shortcuts for slot 1. Messages like *Saved to slot 2* or *Sound off*
appear at the bottom of the picture; after a save the message offers **Undo**, which puts back what
the slot held before.

Save states work across every platform — all 64-bit builds share one layout, so a state made on your
desktop loads on your phone.

## Widescreen

Settings > WIDESCREEN widens the picture to 256x144 (16:9 at the game's own 144 lines). The game's
original 160 pixels stay in the middle, untouched, with 48 pixels either side showing the rooms
around you, decoded from your ROM: the overworld grid (with the right season in Seasons) and, in
dungeons, the rooms through a doorway once you've visited them. Large dungeon rooms simply show more
of themselves.

The status bar runs the whole way across rather than sitting in a box in the middle. With **4 slots**
on, the X and Y items get their own boxes at its left end and the game's own bar slides right to make
room, so the four item slots read across as X, Y, B, A. Holding X or Y then leaves the B box showing
its own item, instead of borrowing it to display the one you pressed.

The sides scroll along with every room change. Nothing moves in them — the game only runs the room
you're in — and they show each room as the ROM describes it, so a chest you opened or grass you cut
won't update until you walk back there. Houses, caves and cutscenes use the game's border colour
instead, while a menu fills them with its own colour and keeps the status bar across the top.
**DIM SIDES** (on by default) draws the sides slightly darker than the live room.

## Save sync

Settings > SYNC keeps your saves in step across devices, with no passwords: **CREATE ACCOUNT** gives
you a 16-digit code, and **ENTER CODE** on another device joins the same account.

Synced: each game's save, the save-state slots with their pictures and item buttons, and your
settings. Never your ROM or anything derived from it.

It syncs when the launcher opens, before a game starts, and when you leave a game or the app;
**SYNC NOW** does it by hand. A sync that fails because you're offline is retried automatically, and
again the moment the app comes back to the foreground. If the same save changed on two devices, a
**KEEP WHICH?** screen shows you both — the files and hearts, or the state's picture — and you pick.
The copy you turn down isn't thrown away: it stays in the game's folder as `<name>.bak` (a save
state's picture too), until the next KEEP WHICH? about the same file replaces it.

## Android

Touch controls sit under the game in portrait and at its sides in landscape: D-pad (diagonals by
touching between the arms), A, B, Start, Select, Pause and fast-forward (`>>`, hold), plus X and Y
when 4 slots is on. They hide when you use a controller or keyboard and come back on the next touch.
The back button pauses, and goes back in menus. In the menus, tap a row to choose it and drag a long
list to scroll it; while a menu fills the screen the on-screen controls step aside.

Pressing a control or tapping a menu gives a short tick, the same one as the keyboard, and follows
your phone's touch feedback setting. **Settings → Vibration** (Android only) turns it off for the game.
Volume has **−** and **+** either side of its bars, and tapping a bar jumps to that level.

Leaving the app saves the game and a Resume state; coming back opens the pause menu.

## Where your files are

Everything — the data read from your ROM, your saves, save states and settings — lives in one
per-user folder:

| System | Folder |
|---|---|
| macOS | `~/Library/Application Support/oracles-decomp/oracles/` |
| Windows | `%APPDATA%\oracles-decomp\oracles\` |
| Linux | `~/.local/share/oracles-decomp/oracles/` |
| Android | the app's own storage (removed when you uninstall) |

Deleting that folder resets the app; you'll be asked for your ROM again.

### Portable mode

To keep everything next to the app instead (on a USB stick or external drive, or shared between
computers), turn on **Settings → Portable** and choose **Restart** on the message that appears. The
app writes an empty `portable.txt` beside itself and, from then on, keeps its data in an
`oracles-data` folder there:

| System | Beside |
|---|---|
| macOS | `Oracles.app` (never inside it) |
| Windows | `Oracles.exe` |
| Linux | the `.AppImage` file |

Creating `portable.txt` by hand does the same, so a folder holding the app, `portable.txt` and
`oracles-data` can be copied anywhere as it is.

On the first start in the new place, the app copies your current data across, so your saves come
with you. It never overwrites: if the folder it switches to already holds data, that data is used as
it is, and the folder it left stays untouched as a backup. Turning Portable off again goes back to
the per-user folder, as it was when you left it.

The app needs to be able to write where it sits. On macOS, a freshly downloaded app runs from a
read-only copy until it has been moved once, so drag it to its final place first. Portable mode
isn't offered on Android.

## Reporting problems

Please open an [issue](https://github.com/kirby-letsgo/oracles-decomp/issues) with your system, which
game, and what you were doing. Never attach a ROM or a save made from one. On Android,
`adb logcat -s oracles` shows the app's messages.

## How it works

Every routine the games run is readable C: all of Ages, and 99% of Seasons (shared with Ages where
the two games agree, hand-written where Seasons differs; the rest generated from the disassembly).
The native build runs both games with no CPU emulator and no ROM code at all — on first launch the
app takes the graphics, sound and data out of your ROM and zeroes the code bytes, because it no
longer needs them.

Behaviour is checked against the original ROM by replaying full playthroughs and comparing every C
routine with the original code it replaces.

To build it yourself, or work on it, see [DEVELOPMENT.md](DEVELOPMENT.md).

## Credits

- The variable-width font support is a port of Drenn's `vwf` branch of
  [oracles-disasm](https://github.com/Stewmath/oracles-disasm/tree/vwf), the disassembly this
  project is built on.
- The variable-width font is by Toruzz, from the Link's Awakening DX VWF hack. It isn't bundled
  yet: we're asking for permission first.
