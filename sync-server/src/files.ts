// What a device may sync: the game's own save, the save states and their thumbnails, the item
// buttons, and the shared settings. Never the ROM-derived cache files (rom.bin, cyctab.bin).
const perGame = /^(sram\.sav|item_buttons|state_(?:[1-4]|auto)(?:\.thumb)?)$/;

export const GAMES = ['ages', 'seasons'] as const;
export const SHARED = 'shared';

export function allowedFile(game: string, name: string): boolean {
  if (game === SHARED) return name === 'settings.ini';
  return (GAMES as readonly string[]).includes(game) && perGame.test(name);
}

// Save states are engine snapshots: a device must not load one written by another format.
export function needsFormatVersion(name: string): boolean {
  return name.startsWith('state_') && !name.endsWith('.thumb');
}

export const MAX_FILE_BYTES = 1024 * 1024;
export const KEEP_VERSIONS = 3;
