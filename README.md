# The Legend of Zelda: Oracle of Ages & Oracle of Seasons

Native macOS, Windows, Linux and Android versions of **Oracle of Ages** and **Oracle of Seasons**
(Capcom, 2001), rebuilt from the games' own code in C. There is no emulator underneath: the games
run as native programs, which is what makes widescreen, save states and save sync across your
devices possible.

**No game data is included.** You need your own Oracle of Ages and/or Oracle of Seasons (USA) ROM.
The app reads it once, keeps the graphics, sound and data, and never needs it again.

## Quality-of-life improvements

What this port adds on top of the original games:

- **Widescreen (16:9)** — see the rooms around you, not just the one you're in
- **Save states** — 4 slots with thumbnails, plus a Resume state from wherever you last quit
- **Save sync** — your saves and states follow you between desktop and phone, no passwords
- **Remappable controls** — every button, for keyboard and gamepad
- **Fast-forward** — hold Tab, or the right trigger
- **Screen filters** — sharp, scanlines, LCD grid, CRT, or xBRZ smoothing at three strengths, and true Game Boy Color colours
- **Fast text and faster menus** — optional, off by default
- **Quick swap and 4 item slots** — optional, off by default

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

The app opens on its launcher, with Ages and Seasons side by side. A game you have no ROM for yet
shows **NOT INSTALLED** and an **ADD ROM...** row — choose it and the file picker opens. Do the same
on the other tab to add the second game. Nothing is asked for before the launcher appears.

The ROM can be a plain `.gbc`/`.gb` file **or a `.zip` or `.gz`** holding one, so a freshly
downloaded archive works as it came. From a zip the `.gbc`/`.gb` file inside is taken, otherwise the
largest one. `.7z` isn't read — extract it first.

Your ROM has to be the USA release of either game; the app checks it and says so if it isn't. On
Android, copy the ROM to the phone's Downloads first so the picker can reach it.

## Playing

The launcher shows each game's three save files with name, hearts and essences, plus **Resume** (the
state from when you last quit), **Load state** and the title screen. Choosing a file boots straight
into it.

### Controls

| | |
|---|---|
| Move | Arrow keys |
| A / B | `X` / `Z` |
| Start / Select | Return / Backspace or Right Shift |
| Pause | Esc (gamepad: Guide) |
| Fast-forward | Hold Tab (gamepad: right trigger) |
| Mute | `M` |
| Fullscreen | F11 or Cmd+F |
| Save / load state slot 1 | Cmd+S / Cmd+R |

Every button — plus Pause, Fast-forward, Swap and the item buttons — can be remapped for keyboard
and gamepad in Settings > CONTROLS. The window resizes in whole-pixel steps.

### Settings

From the launcher or the pause menu: volume, screen filter (sharp, scanlines, LCD grid, CRT, and
XBRZ2/XBRZ3/XBRZ4 — the xBRZ pixel-art scaler, where the number is how hard it smooths: 2 rounds off
jagged diagonals and leaves the rest of the art alone, 4 is smoothest but softens small shapes), scale
(**pixel** for whole-pixel steps, **fill** for the largest size that fits), Game Boy Color colours,
fullscreen and controls.

There are also quality-of-life toggles, **all off by default**:

- **Fast text** and **faster menus**
- **Quick swap**: `C` swaps the A and B items
- **4 slots**: `A`/`S` become two more item buttons — highlight an item in the inventory and press
  one to assign it, then hold it in play to use that item

### Save states

Esc pauses the game: Resume, Save state, Load state (4 slots, each with a thumbnail and its date)
and Quit, which saves a Resume state and returns to the launcher. Cmd+S and Cmd+R are shortcuts for
slot 1. Messages like *state saved* or *sound off* appear at the bottom of the screen.

Save states work across every platform — all 64-bit builds share one layout, so a state made on your
desktop loads on your phone.

## Widescreen

Settings > WIDESCREEN widens the picture to 256x144 (16:9 at the game's own 144 lines). The game's
original 160 pixels stay in the middle, untouched, with 48 pixels either side showing the rooms
around you, decoded from your ROM: the overworld grid (with the right season in Seasons) and, in
dungeons, the rooms through a doorway once you've visited them. Large dungeon rooms simply show more
of themselves.

The sides scroll along with every room change. Nothing moves in them — the game only runs the room
you're in — and they show each room as the ROM describes it, so a chest you opened or grass you cut
won't update until you walk back there. Houses, caves, menus and cutscenes use the game's border
colour instead. **DIM SIDES** (on by default) draws the sides slightly darker than the live room.

## Save sync

Settings > SYNC keeps your saves in step across devices, with no passwords: **CREATE ACCOUNT** gives
you a 16-digit code, and **ENTER CODE** on another device joins the same account.

Synced: each game's save, the save-state slots with their pictures and item buttons, and your
settings. Never your ROM or anything derived from it.

It syncs when the launcher opens, before a game starts, and when you leave a game or the app;
**SYNC NOW** does it by hand. A sync that fails because you're offline is retried automatically, and
again the moment the app comes back to the foreground. If the same save changed on two devices, a
**KEEP WHICH?** screen shows you both — the files and hearts, or the state's picture — and you pick.

## Android

Touch controls sit under the game in portrait and at its sides in landscape: D-pad (diagonals by
touching between the arms), A, B, Start, Select, Pause and fast-forward (`>>`, hold), plus X and Y
when 4 slots is on. They hide when you use a controller or keyboard and come back on the next touch.
The back button pauses, and goes back in menus.

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
