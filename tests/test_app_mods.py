"""Mods entry is hidden until the mods release, including from touch navigation."""
import os
import subprocess
import sys
import tempfile
with tempfile.TemporaryDirectory() as cache:
    r = subprocess.run([sys.argv[1], "--cache", cache], env=dict(os.environ,
        SDL_VIDEO_DRIVER="dummy", SDL_AUDIO_DRIVER="dummy",
        ORACLES_TEST_KEYS="10:Right,20:Down,30:Up,40:^Setting.19,50:Z,60:Z"),
        capture_output=True, text=True, timeout=20)
    assert r.returncode == 0, r.stderr
    assert "test tap: no Setting.19" in r.stderr, r.stderr
print("app mods: deferred entry is hidden")
