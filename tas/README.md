# TAS input movies

Used as deterministic playthrough scripts for the verification suite. See `docs/design.md`.

| File | Game | Author | Source | Core | Frames |
|---|---|---|---|---|---|
| `ages-consoleverified.bk2` | Ages (USA) | scorpianman42, resynced by alyosha | https://github.com/alyosha-tas/GB-C_replay_files | BizHawk GBHawk, GBA-on-CGB mode | 289,519 |
| `ages-gambatte.bk2` | Ages (USA) | scorpianman42 | https://tasvideos.org/3127M | BizHawk 1.11.5 Gambatte, GBA-on-CGB mode | 289,340 |

Both expect ROM SHA1 `880374fb978b18af4aa529e2e32f7ffb4d7dd2f4`.

BK2 files are zip archives. `Input Log.txt` holds one line per frame in the order
Up, Down, Left, Right, Start, Select, B, A, Power.
