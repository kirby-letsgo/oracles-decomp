#!/usr/bin/env python3
"""Drives oracles-native's menus headless (SDL dummy drivers, ORACLES_TEST_KEYS) through a fresh
cache: install the ROM, boot into file 1, pause, save to slot 1, resume, pause, load slot 1, open
Settings from the pause menu, lower the volume and bind A to the S key, quit to the launcher (which writes the auto
state), quit the app; then save slot 1 again through the touch overlay.
usage: test_app_menus.py ORACLES_NATIVE ROM   (exit 77 when the ROM is missing)"""
import os, subprocess, sys, tempfile

app, rom = sys.argv[1], sys.argv[2]
if not os.path.exists(rom): print('skip: ROM not present'); sys.exit(77)
env = dict(os.environ, SDL_VIDEO_DRIVER='dummy', SDL_AUDIO_DRIVER='dummy')
with tempfile.TemporaryDirectory() as cache:
    r = subprocess.run([app, rom, '--cache', cache, '--frames', '1'], env=env, capture_output=True, text=True, timeout=120)
    assert r.returncode == 0, r.stderr
    keys = ('200:Escape,210:Down,220:X,230:X,240:Z,300:Escape,310:Down,320:Down,330:X,340:X,'
            '360:Escape,370:Up,380:Up,390:X,400:Left,401:Down,402:Down,403:Down,404:Down,405:Down,407:Down,408:Down,409:Down,410:X,'
            '412:Down,414:Down,416:Down,418:Down,420:X,422:S,424:Z,426:Z,428:Z,'
            '460:Escape,470:Up,480:X,520:Z')
    r = subprocess.run([app, '--game', 'seasons', '--cache', cache], env=dict(env, ORACLES_TEST_KEYS=keys), capture_output=True, text=True, timeout=120)
    assert r.returncode == 0, r.stderr
    d = os.path.join(cache, 'seasons')
    for f in ('state_1', 'state_1.thumb', 'state_auto', 'state_auto.thumb', 'sram.sav'):
        assert os.path.exists(os.path.join(d, f)), f'missing {f}; stderr: {r.stderr}'
    assert os.path.getsize(os.path.join(d, 'state_1.thumb')) == 160 * 144 * 3
    ini = open(os.path.join(cache, 'settings.ini')).read()
    assert 'volume=9' in ini, ini
    assert 'keys=82,-1;81,-1;80,-1;79,-1;22,27;' in ini, ini
    assert ini.count(';') >= 24, ini                   # every action, Swap and Item X/Y included
    # the touch overlay: Pause, down to Save State, A, A (slot 1), then quit with the keyboard
    os.remove(os.path.join(d, 'state_1'))
    keys = '200:#PAUSE,210:#DOWN,220:#A,230:#A,300:Escape,360:Escape,370:Up,380:X,420:Z'
    r = subprocess.run([app, '--game', 'seasons', '--cache', cache], env=dict(env, ORACLES_TOUCH='1', ORACLES_TEST_KEYS=keys), capture_output=True, text=True, timeout=120)
    assert r.returncode == 0, r.stderr
    assert os.path.exists(os.path.join(d, 'state_1')), f'touch did not save slot 1; stderr: {r.stderr}'
print('ok app_menus')
