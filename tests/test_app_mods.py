"""Official mod selection through the real launcher, with no ROM or network."""
import os
from pathlib import Path
import subprocess
import sys
import tempfile

app = sys.argv[1]
with tempfile.TemporaryDirectory(prefix="oracles-mods-ui-") as tmp:
    cache = Path(tmp)

    def run(action):
        keys = f"10:Right,20:Down,30:Up,40:X,50:{action},70:Z,80:Z,90:Z"
        env = dict(os.environ, SDL_VIDEO_DRIVER="dummy", SDL_AUDIO_DRIVER="dummy",
                   ORACLES_TEST_KEYS=keys)
        result = subprocess.run([app, "--cache", str(cache)], env=env,
                                capture_output=True, text=True, timeout=20)
        assert result.returncode == 0, result.stderr
        assert "test tap: no" not in result.stderr, result.stderr

    run("^OfficialMod.0")
    assert "essence_tracker=on" in (cache / "settings.ini").read_text()
    # A fresh process must read On before Left flips it back to Off.
    run("Left")
    assert "essence_tracker=off" in (cache / "settings.ini").read_text()
    assert not (cache / "mods" / "essence-tracker").exists()
print("app mods: touch, keyboard, restart persistence and no installation files passed")
