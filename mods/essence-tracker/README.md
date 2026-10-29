# Essence Tracker

Eight markers show which dungeon essences your current save has collected, in dungeon order.
Works with Ages and Seasons, including their European releases. It only reads progress and draws
an overlay; it does not change your game or save.

Eight 3x3 markers sit in a single 33x5-pixel strip: at the bottom of the left side area in
widescreen, or at bottom centre in the original aspect ratio. There is no count or large panel. The overlay hides in
menus, cutscenes, room transitions and side-view areas.

## Enable in Oracles

Open **Settings → Mods → Essence Tracker** and turn it on. The official mod is bundled with the
app, so no download or folder copy is needed. Start a game to apply your choice. If you changed
it while playing, leave to the launcher and start again. The choice survives app restarts.

## Manual installation (older builds)

Use this only on a build without the official Mods screen. Remove a manually installed copy
before enabling the bundled version, to avoid duplicate copies.

Copy the entire `essence-tracker` folder into your app data folder's `mods` directory:

- macOS: `~/Library/Application Support/oracles-decomp/oracles/mods/`
- Windows: `%APPDATA%\oracles-decomp\oracles\mods\`
- Linux: `~/.local/share/oracles-decomp/oracles/mods/`
- Portable mode: `oracles-data/mods/` beside the app.

Create `mods` if needed. The resulting path must be `mods/essence-tracker/main.lua`, with
`mod.ini` beside it. Leave to the launcher and start the game again to load it. Remove the folder
and restart the game to uninstall.

Progress always comes from the active game: loading an older save state immediately restores its
essence markers. No separate tracker save file is created. The mod registers a “Show essences”
setting, but the app does not yet expose a mod settings menu; it is on by default.

No ROM, font or game graphics are included. The markers are drawn as simple rectangles.
