"""Direct state launch: strict arguments, real restores and non-destructive failure."""
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
app, rom, boot = sys.argv[1:]
env = dict(os.environ, SDL_VIDEO_DRIVER="dummy", SDL_AUDIO_DRIVER="dummy")
with tempfile.TemporaryDirectory() as cache:
    def run(*args):
        return subprocess.run([app, "--cache", cache, *args], env=env,
                              capture_output=True, text=True, timeout=30)
    for args in [("--state",), ("--state", "auto"),
                 ("--game", "seasons", "--state", "auto", "--file", "1")]:
        assert run(*args).returncode == 2
    for value in ("0", "5", "1x", "-1", "", "AUTO"):
        assert run("--game", "seasons", "--state", value).returncode == 2
    if not Path(rom).exists() or not Path(boot).exists():
        print("skip: ROM or boot state missing"); sys.exit(77)
    r = run(rom, "--frames", "1")
    assert r.returncode == 0, r.stderr
    folder = Path(cache) / "seasons"
    for state in ("auto", "1", "2", "3", "4"):
        path = folder / ("state_auto" if state == "auto" else "state_" + state)
        before = (folder / "sram.sav").read_bytes()
        for corrupt in (False, True):
            if corrupt: path.write_bytes(b"broken state")
            r = run("--game", "seasons", "--state", state, "--frames", "1")
            assert r.returncode == 1 and "could not load save state" in r.stderr, r.stderr
            assert (folder / "sram.sav").read_bytes() == before
            if corrupt: assert path.read_bytes() == b"broken state"
        shutil.copyfile(boot, path)
        r = run("--game", "seasons", "--state", state, "--frames", "1")
        assert r.returncode == 0 and "started from" in r.stderr, r.stderr
        assert "ran 1 frames" in r.stderr, r.stderr
    r = run(rom, "--state", "auto", "--frames", "1")
    assert r.returncode == 0 and "started from automatic state" in r.stderr, r.stderr
print("app state: arguments, automatic and four manual slots, failure preservation passed")
