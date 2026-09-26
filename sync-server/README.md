# Oracles sync server

Keeps a player's saves in step across devices. There are no passwords: creating an account returns
a 16-digit **sync code** (`4827-1930-5561-0284`), and any device that knows the code can read and
write that account's files. The code is random (not the database id), so it can't be guessed.

Synced per game (`ages`, `seasons`): `sram.sav`, `state_1`-`state_4`, `state_auto`, their `.thumb`
files and `item_buttons`; shared: `settings.ini`. Never the ROM-derived cache files. Each upload is a
new version; the last three are kept.

## API

| Method | Path                                        |                                                                                                                   |
| ------ | ------------------------------------------- | ----------------------------------------------------------------------------------------------------------------- |
| `POST` | `/accounts`                                 | new account: `201 {"code": "4827-1930-5561-0284"}` (10 per hour per address)                                      |
| `GET`  | `/accounts/:code`                           | the current version of every file (game, name, version, size, sha256, device, formatVersion, updatedAt)           |
| `GET`  | `/accounts/:code/manifest`                  | the same as plain text, one line per file: `game name version sha256 formatVersion\|- updatedAt(unix s) device`   |
| `PUT`  | `/accounts/:code/files/:game/:name`         | upload (`application/octet-stream`, up to 1 MB): `201 {version, sha256}`, or `200` when unchanged                 |
| `GET`  | `/accounts/:code/files/:game/:name`         | download the current version (`?version=N` for an older one); headers `x-version`, `x-sha256`, `x-format-version` |
| `GET`  | `/accounts/:code/files/:game/:name/history` | the kept versions                                                                                                 |
| `GET`  | `/health`                                   | `{"ok": true}`                                                                                                    |

Codes are accepted with or without dashes. Upload headers: `x-device` (a name to show), `x-format-version`
(required for save states: a device must not load a state from another engine build) and
`x-base-version` (the version the device last saw; a different current version answers `409` with it,
so a device never overwrites a newer save by accident).

## Rate limits

Per client address: 300 requests a minute on every route, and 10 new accounts an hour. Per sync
code: 60 uploads a minute, from any address. Over a limit the server answers `429` with
`retry-after`. The client address is taken from the proxy's `X-Forwarded-For` (Coolify's Traefik);
set `TRUST_PROXY=false` when the server is reachable directly, or anyone could fake their address.

## Development

```bash
pnpm install
cp .env.example .env
pnpm db:up          # Postgres on localhost:5433 (compose.dev.yml)
pnpm dev            # migrates, then serves on :3000
pnpm test           # in-memory Postgres (PGlite), no Docker needed
pnpm typecheck && pnpm lint
```

After changing `src/db/schema.ts`, run `pnpm db:generate` and commit the new file in `drizzle/`;
the server applies pending migrations when it starts.

## Deploying on Coolify

New resource > Docker Compose, this repository, base directory `/sync-server`, compose file
`docker-compose.yml`. Coolify generates the database user and password and a domain for the `api`
service (port 3000); set your own domain on the `api` service if you prefer. Data lives in the
`db-data` volume.
