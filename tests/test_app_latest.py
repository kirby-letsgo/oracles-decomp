"""Newest save/state launching, exact ROM scope, ties and non-destructive errors."""
import hashlib
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile

app, helper, rom_dir, tas_dir = sys.argv[1:]
env = dict(os.environ, SDL_VIDEO_DRIVER="dummy", SDL_AUDIO_DRIVER="dummy")


def saved_file(ages):
    data = bytearray(0x550)
    data[2:10] = b"Z21216-0" if ages else b"Z11216-0"
    data[0x52:0x56] = b"LINK"
    health = 0xfa if ages else 0xf2
    data[health:health + 2] = bytes((12, 12))
    checksum = sum(int.from_bytes(data[i:i + 2], "little") for i in range(2, len(data), 2)) & 0xffff
    data[:2] = checksum.to_bytes(2, "little")
    return data


def stamp(path, seconds):
    os.utime(path, ns=(seconds * 10**9, seconds * 10**9))


with tempfile.TemporaryDirectory() as tmp:
    r = subprocess.run([helper, tmp], env=env, capture_output=True, text=True, timeout=30)
    assert r.returncode == 0, r.stderr
    print(r.stdout.strip())

with tempfile.TemporaryDirectory() as cache:
    def run(*args):
        return subprocess.run([app, "--cache", cache, *args], env=env,
                              capture_output=True, text=True, timeout=30)

    for args in [("--latest",), ("--latest", "--state", "auto"),
                 ("--game", "seasons", "--latest", "--file", "1"),
                 ("--game", "seasons", "--state", "2", "--latest"),
                 ("--game", "seasons", "--latest", "--state", "auto")]:
        assert run(*args).returncode == 2, args

    for game in ("ages", "seasons"):
        for eu in (False, True):
            region = "Europe) (En,Fr,De,Es,It" if eu else "USA, Australia"
            rom = Path(rom_dir) / f"Legend of Zelda, The - Oracle of {game.title()} ({region}).gbc"
            boot = Path(tas_dir) / f"{game}-boot.state"
            if not rom.exists() or not boot.exists():
                print(f"skip: {rom.name} or boot state missing")
                sys.exit(77)
            r = run(str(rom), "--frames", "1")
            assert r.returncode == 0, r.stderr
            game_id = game + ("-eu" if eu else "")
            folder = Path(cache) / game_id
            sav = folder / "sram.sav"
            meta = folder / "sram.sav.dates"
            states = [folder / "state_auto", *(folder / f"state_{i}" for i in range(1, 5))]

            # Blank SRAM does not count; a failure must not write over anything.
            before = (sav.read_bytes(), sav.stat().st_mtime_ns)
            r = run("--game", game_id, "--latest", "--frames", "1")
            assert r.returncode == 1 and "no save files or save states" in r.stderr, r.stderr
            assert before == (sav.read_bytes(), sav.stat().st_mtime_ns)

            def reset():
                for path in states:
                    if path.is_dir(): path.rmdir()
                    else: path.unlink(missing_ok=True)
                meta.unlink(missing_ok=True)
                sav.write_bytes(bytes(0x2000))
                stamp(sav, 100)

            reset()
            # Every state slot can be the newest. Europe uses corrupt states to prove selection
            # without loading a USA snapshot into a different ROM. USA checks real restores.
            for i, newest in enumerate(states):
                reset()
                for path in states:
                    if eu: path.write_bytes(b"corrupt state")
                    else: shutil.copyfile(boot, path)
                    stamp(path, 200)
                stamp(newest, 300)
                r = run("--game", game_id, "--latest", "--frames", "1")
                if eu:
                    assert r.returncode == 1 and f"could not load save state {newest}" in r.stderr, r.stderr
                else:
                    expected = "automatic state" if i == 0 else f"slot {i}"
                    assert r.returncode == 0 and f"started from {expected}" in r.stderr, r.stderr

            reset()
            # A single valid normal save (including a backup-only slot) beats older states.
            file = saved_file(game == "ages")
            data = bytearray(0x2000)
            data[0x1aa0:0x1aa0 + len(file)] = file  # File 3, backup.
            sav.write_bytes(data)
            stamp(sav, 300)
            states[0].write_bytes(b"older invalid state")
            stamp(states[0], 200)
            r = run("--game", game_id, "--latest", "--frames", "1")
            assert r.returncode == 0 and "latest: starting save file 3" in r.stderr, r.stderr

            # Known per-slot dates select the second of several normal saves.
            data[0x10:0x10 + len(file)] = file
            sav.write_bytes(data)
            stamp(sav, 400)
            digest = hashlib.sha1(file).hexdigest()
            meta.write_text(f"0 {digest} 250000000000\n2 {digest} 300000000000\n")
            r = run("--game", game_id, "--latest", "--frames", "1")
            assert r.returncode == 0 and "latest: starting save file 3" in r.stderr, r.stderr

            # Legacy/imported multi-slot saves need a choice, not a guessed save file.
            meta.unlink()
            r = run("--game", game_id, "--latest", "--frames", "1")
            assert r.returncode == 0 and "opening file selection" in r.stderr, r.stderr

            # Issue #47: automatic resume must not restore stale SRAM after an unclean exit.
            # A marker outside the save slots must count even with older per-slot metadata.
            reset()
            # Let each ROM initialise its SRAM header (EU also stores language settings).
            r = run("--game", game_id, "--file", "1", "--frames", "601")
            assert r.returncode == 0, r.stderr
            data = bytearray(sav.read_bytes())
            data[0x10:0x10 + len(file)] = file
            data[0x1000:0x1000 + len(file)] = file  # Avoid an unrelated in-game backup repair.
            data[0x1ff0:0x1ff8] = b"MARQUEUR"
            sav.write_bytes(data)
            stamp(sav, 300)
            meta.write_text(f"0 {digest} 100000000000\n")
            if eu:
                states[0].write_bytes(b"older invalid state")
            else:
                shutil.copyfile(boot, states[0])
            stamp(states[0], 200)
            before = {p: (p.read_bytes(), p.stat().st_mtime_ns)
                      for p in (sav, meta, states[0])}
            # Run past the 600-frame periodic flush, then also exercise the exit flush.
            r = run("--game", game_id, "--state", "auto", "--frames", "601")
            assert r.returncode == 0 and "SRAM is newer" in r.stderr, r.stderr
            assert "started from automatic state" not in r.stderr, r.stderr
            for p, snapshot in before.items():
                after = (p.read_bytes(), p.stat().st_mtime_ns)
                changed = [i for i, (a, b) in enumerate(zip(snapshot[0], after[0])) if a != b]
                assert snapshot == after, (game_id, p.name, changed[:40], r.stderr)

            if game_id == "seasons-eu":
                # The reported launcher path, followed by a clean Quit to menu which
                # writes a replacement automatic state and flushes SRAM again.
                r = subprocess.run([app, "--cache", cache, "--frames", "601"],
                                   env=dict(env, ORACLES_TEST_KEYS=
                                            "10:^Home.0,150:Escape,170:Up,190:X"),
                                   capture_output=True, text=True, timeout=30)
                assert r.returncode == 0 and "SRAM is newer" in r.stderr, r.stderr
                assert "test tap: no" not in r.stderr, r.stderr
                for p in (sav, meta):
                    assert before[p] == (p.read_bytes(), p.stat().st_mtime_ns), r.stderr
                assert states[0].read_bytes() != before[states[0]][0], r.stderr

            # Ties favour auto, then the lowest manual state. Thumbnails/undo files are ignored.
            reset()
            for path in states:
                path.write_bytes(b"corrupt newest state")
                stamp(path, 300)
            for name in ("state_1.thumb", "state_4.undo"):
                (folder / name).write_bytes(b"not a candidate")
            for expected in (states[0], states[1]):
                snapshots = {p: (p.read_bytes(), p.stat().st_mtime_ns) for p in folder.iterdir() if p.is_file()}
                # Plain --game must search the active variant, not the other region.
                r = run("--game", game, "--latest", "--frames", "1")
                assert r.returncode == 1 and f"could not load save state {expected}" in r.stderr, r.stderr
                assert snapshots == {p: (p.read_bytes(), p.stat().st_mtime_ns) for p in snapshots}
                expected.unlink()
            # Direct ROM paths use the same resolution; directories are not state candidates.
            states[0].mkdir()
            r = run(str(rom), "--latest", "--frames", "1")
            assert r.returncode == 1 and f"could not load save state {states[2]}" in r.stderr, r.stderr

print("app latest: four ROM variants, saves, states, ties, stale resume and failure preservation passed")
