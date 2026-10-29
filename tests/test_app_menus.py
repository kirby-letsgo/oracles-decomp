#!/usr/bin/env python3
"""Drives oracles-native's menus headless (SDL dummy drivers, ORACLES_TEST_KEYS) through a fresh
cache: install the ROM, boot into file 1, pause, save to slot 1, resume, pause, load slot 1, open
Settings from the pause menu, lower the volume and bind A to the S key, quit to the launcher (which writes the auto
state), quit the app; then save slot 1 again with the touch overlay and a tap on the slot's row; then
play by taps alone: pause, Save state, slot 2, Quit to menu, and start the game from slot 2 on the main menu;
then, with a newer release out (served from here), the download link from the main menu's notice and
Settings > Version, and an AppImage replacing itself (or, downloaded damaged, opening the link).
Haptic ticks (ORACLES_TEST_HAPTICS logs them): one per control pressed and menu tap, none for keys,
none once settings.ini turns them off.
usage: test_app_menus.py ORACLES_NATIVE ROM   (exit 77 when the ROM is missing)"""
import hashlib, http.server, json, os, subprocess, sys, tempfile, threading

app, rom = sys.argv[1], sys.argv[2]
if not os.path.exists(rom): print('skip: ROM not present'); sys.exit(77)
env = dict(os.environ, SDL_VIDEO_DRIVER='dummy', SDL_AUDIO_DRIVER='dummy')
with tempfile.TemporaryDirectory() as cache:
    r = subprocess.run([app, rom, '--cache', cache, '--frames', '1'], env=env, capture_output=True, text=True, timeout=120)
    assert r.returncode == 0, r.stderr
    keys = ('200:Escape,210:Down,220:X,230:X,240:Z,300:Escape,310:Down,320:Down,330:X,340:X,'
            '360:Escape,370:Up,380:Up,390:X,400:Left,' + ','.join(f'{t}:Down' for t in range(401, 413)) + ',413:X,'
            '414:Down,416:Down,418:Down,420:Down,422:X,424:S,426:Z,428:Z,430:Z,'
            '460:Escape,470:Up,480:X,520:Z')
    r = subprocess.run([app, '--game', 'seasons', '--cache', cache], env=dict(env, ORACLES_TEST_KEYS=keys, ORACLES_TEST_HAPTICS='1'), capture_output=True, text=True, timeout=120)
    assert r.returncode == 0, r.stderr
    assert 'haptic' not in r.stderr, f'keys gave a haptic tick; stderr: {r.stderr}'
    d = os.path.join(cache, 'seasons')
    for f in ('state_1', 'state_1.thumb', 'state_auto', 'state_auto.thumb', 'sram.sav'):
        assert os.path.exists(os.path.join(d, f)), f'missing {f}; stderr: {r.stderr}'
    assert os.path.getsize(os.path.join(d, 'state_1.thumb')) == 160 * 144 * 3
    ini = open(os.path.join(cache, 'settings.ini')).read()
    assert 'volume=9' in ini, ini
    assert 'fast_forward=3' in ini, ini
    assert 'keys=82,-1;81,-1;80,-1;79,-1;22,27;' in ini, ini
    assert ini.count(';') >= 24, ini                   # every action, Swap and Item X/Y included
    # the touch overlay: Pause, down to Save State, A, then tap slot 1's row; quit with the keyboard
    os.remove(os.path.join(d, 'state_1'))
    keys = '200:#PAUSE,210:#DOWN,220:#A,240:^Slot.0,300:Escape,360:Escape,370:Up,380:X,420:Z'
    r = subprocess.run([app, '--game', 'seasons', '--cache', cache], env=dict(env, ORACLES_TOUCH='1', ORACLES_TEST_KEYS=keys), capture_output=True, text=True, timeout=120)
    assert r.returncode == 0, r.stderr
    assert os.path.exists(os.path.join(d, 'state_1')), f'touch did not save slot 1; stderr: {r.stderr}'
    # taps only: pause with the overlay, then rows: Save state, slot 2, Quit to menu; on the main
    # menu, slot 2's row starts the game from that state; quit with the keyboard from the pause menu
    # B once the game is running: the main menu was the last menu drawn, and its rows must not go on
    # claiming fingers meant for the controls under where they were
    keys = ('200:#PAUSE,230:^Pause.1,260:^Slot.1,300:^Pause.4,340:^Home.2,390:#B,'
            '420:Escape,440:Up,460:X,500:Z')
    r = subprocess.run([app, '--game', 'seasons', '--cache', cache], env=dict(env, ORACLES_TOUCH='1', ORACLES_TEST_KEYS=keys, ORACLES_TEST_HAPTICS='1'), capture_output=True, text=True, timeout=120)
    assert r.returncode == 0, r.stderr
    assert 'test tap: no' not in r.stderr, r.stderr
    ticks = r.stderr.count('haptic\n')
    assert ticks == 6, f'{ticks} haptic ticks for Pause, four taps and B in game; stderr: {r.stderr}' 
    assert os.path.exists(os.path.join(d, 'state_2')), f'tapping did not save slot 2; stderr: {r.stderr}'
    assert 'started from slot 2' in r.stderr, f'the main menu did not start slot 2; stderr: {r.stderr}'
    # Undo: saving into empty slot 3 and undoing empties it again; saving over slot 2 and undoing
    # brings back the state that was there
    before = open(os.path.join(d, 'state_2'), 'rb').read()
    keys = ('200:#PAUSE,230:^Pause.1,260:^Slot.2,290:^Undo,'
            '330:^Pause.1,360:^Slot.1,390:^Undo,430:^Pause.4,470:Z')
    r = subprocess.run([app, '--game', 'seasons', '--cache', cache], env=dict(env, ORACLES_TOUCH='1', ORACLES_TEST_KEYS=keys), capture_output=True, text=True, timeout=120)
    assert r.returncode == 0, r.stderr
    assert 'test tap: no' not in r.stderr, r.stderr
    assert not os.path.exists(os.path.join(d, 'state_3')), f'undo left slot 3 saved; stderr: {r.stderr}'
    assert open(os.path.join(d, 'state_2'), 'rb').read() == before, 'undo did not bring slot 2 back'
    # Vibration off: the same touches give no tick
    with open(os.path.join(cache, 'settings.ini'), 'a') as f: f.write('haptics=off\n')
    keys = '200:#PAUSE,230:^Pause.0,300:#PAUSE,330:^Pause.4,370:Z'
    r = subprocess.run([app, '--game', 'seasons', '--cache', cache], env=dict(env, ORACLES_TOUCH='1', ORACLES_TEST_KEYS=keys, ORACLES_TEST_HAPTICS='1'), capture_output=True, text=True, timeout=120)
    assert r.returncode == 0, r.stderr
    assert 'test tap: no' not in r.stderr, r.stderr
    assert 'haptic' not in r.stderr, f'ticks with haptics=off; stderr: {r.stderr}'
    assert 'haptics=off' in open(os.path.join(cache, 'settings.ini')).read()
    # a newer release, served from here (ORACLES_TEST_RELEASE stands in for GitHub's API): the main
    # menu's notice and the Version row both give this system's file, printed instead of opened
    srv = os.path.join(cache, 'srv')
    os.mkdir(srv)
    class Quiet(http.server.SimpleHTTPRequestHandler):
        def __init__(self, *a, **k): super().__init__(*a, directory=srv, **k)
        def log_message(self, *a): pass
    server = http.server.ThreadingHTTPServer(('127.0.0.1', 0), Quiet)
    threading.Thread(target=server.serve_forever, daemon=True).start()
    base = f'http://127.0.0.1:{server.server_address[1]}/'
    new = b'\x7fELF the next version'
    files = {'Oracles-macOS.dmg': b'dmg', 'Oracles-windows-x64.zip': b'zip', 'Oracles-linux-x86_64.AppImage': new, 'Oracles-android.apk': b'apk'}
    def release(name, digest_of):
        assets = [{'name': n, 'digest': 'sha256:' + hashlib.sha256(digest_of(n, b)).hexdigest(), 'browser_download_url': base + n} for n, b in files.items()]
        with open(os.path.join(srv, name), 'w') as f: json.dump({'tag_name': 'v9.0.0', 'name': 'Oracles 9.0.0', 'assets': assets}, f)
        return base + name
    for n, b in files.items():
        with open(os.path.join(srv, n), 'wb') as f: f.write(b)
    good, bad = release('latest.json', lambda n, b: b), release('bad.json', lambda n, b: b + b'!')
    keys = '100:^Update,130:Right,140:Down,150:Up,153:Up,155:Up,160:X,190:Z,200:Z'
    r = subprocess.run([app, '--cache', cache], env=dict(env, ORACLES_TOUCH='1', ORACLES_TEST_KEYS=keys, ORACLES_TEST_RELEASE=good), capture_output=True, text=True, timeout=120)
    assert r.returncode == 0, r.stderr
    assert 'test tap: no' not in r.stderr, r.stderr
    assert r.stderr.count('open ' + base + 'Oracles-') == 2, f'expected the notice and the Version row to open the download; stderr: {r.stderr}'
    # an AppImage replaces itself with the release's and restarts on it; a damaged download leaves
    # it as it was and opens the link instead
    appimage = os.path.join(cache, 'Oracles.AppImage')
    for latest, keys in ((bad, '100:^Update,200:Z'), (good, '100:^Update,400:Z')):
        with open(appimage, 'wb') as f: f.write(b'old')
        r = subprocess.run([app, '--cache', cache], env=dict(env, ORACLES_TOUCH='1', ORACLES_TEST_KEYS=keys, ORACLES_TEST_RELEASE=latest, APPIMAGE=appimage), capture_output=True, text=True, timeout=120)
        assert r.returncode == 0, r.stderr
        assert 'test tap: no' not in r.stderr, r.stderr
        if latest == bad:
            assert 'update failed: download damaged' in r.stderr and 'open ' + base + 'Oracles-linux-x86_64.AppImage' in r.stderr, r.stderr
            assert open(appimage, 'rb').read() == b'old', 'a damaged download replaced the AppImage'
        else:
            assert 'restart ' + appimage in r.stderr, f'no restart on the new AppImage; stderr: {r.stderr}'
            assert open(appimage, 'rb').read() == new and os.access(appimage, os.X_OK), 'the AppImage was not replaced'
    server.shutdown()
print('ok app_menus')
