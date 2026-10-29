"""Engine failure recovery and copying persisted reports, without ROM-derived fixtures."""
import os
from pathlib import Path
import subprocess
import sys
import tempfile

app = sys.argv[1]
env = dict(os.environ, SDL_VIDEO_DRIVER="dummy", SDL_AUDIO_DRIVER="dummy", ORACLES_TOUCH="1")
# Settings tab, focus list, wrap to Copy error log, copy, then leave and quit.
keys = "10:Right,20:Down,30:Up,35:Up,40:X,50:Z,60:Z"

def run(cache, *args, script=keys):
    return subprocess.run([app, "--cache", str(cache), *args],
                          env=dict(env, ORACLES_TEST_KEYS=script),
                          capture_output=True, text=True, timeout=20)

with tempfile.TemporaryDirectory() as tmp:
    cache = Path(tmp)
    r = run(cache)
    assert r.returncode == 0, r.stderr
    assert r.stdout.strip() == "CLIPBOARD:", r.stdout
    game = cache / "seasons-eu"
    game.mkdir()
    # A synthetic cache: all zeroes, except the header's game/region and cartridge type.
    rom = bytearray(2 * 1024 * 1024)
    rom[0x134:0x13d] = b"ZELDA DIN"
    rom[0x142] = ord("P")
    rom[0x147] = 0x1b
    rom[0x149] = 3
    (game / "rom.bin").write_bytes(rom)
    (game / "cyctab.bin").write_bytes(bytes(len(rom)))
    (game / "manifest.txt").write_text("oracles-assets 1\n")
    (game / "sram.sav").write_bytes(b"existing save")
    (game / "state_auto").write_bytes(b"existing resume state")
    r = run(cache, "--game", "seasons-eu",
            script="10:^Error.1,20:^Error.0,30:^Error.2,50:Right,60:Down,70:Up,75:Up,80:X,90:Z,100:Z")
    assert r.returncode == 0, r.stderr  # old behavior exited 1 before reaching Settings
    assert "test tap: no" not in r.stderr, r.stderr
    assert "OPEN URL: https://github.com/kirby-letsgo/oracles-decomp/issues/new" in r.stdout, r.stdout
    report = (game / "crash.log").read_text()
    for field in ("test engine failure", "seasons-eu", "version", "platform", "room"):
        assert field in report, report
        assert field in r.stdout, r.stdout
    assert (game / "sram.sav").read_bytes() == b"existing save"
    assert (game / "state_auto").read_bytes() == b"existing resume state"
    # A fresh process can copy the same report, including the inactive EU folder.
    r = run(cache, script="10:Right,20:Down,30:Up,35:Up,40:^Setting.18,50:Z,60:Z")
    assert r.returncode == 0, r.stderr
    assert "test tap: no" not in r.stderr, r.stderr
    assert report in r.stdout, r.stdout
    # Automated frame-limited runs must still fail promptly with a nonzero exit.
    r = run(cache, "--game", "seasons-eu", "--frames", "1")
    assert r.returncode == 1, r.stderr
print("app error: recovery, clipboard, restart persistence and save preservation passed")
