#!/usr/bin/env python3
"""Two devices (two app folders) syncing through the real sync server, driven headless through
oracles-native's launcher:
  A saves slot 1 and syncs; B syncs and gets it; A changes it and syncs; B changes it too, and its
  "keep which?" screen keeps HERE (B's goes to the server); A changes it again and keeps OTHER (B's
  replaces A's, and A's is kept as .bak). Each time the slot's thumbnail follows. Last, A saves
  while the server is unreachable, the server comes back, and the retry uploads it without another
  launch.

The server is no longer part of this repository (it lives in save-sync-server), so it runs here
from a pinned container image: the one that serves an in-memory database and starts empty, so a
run leaves nothing behind. Pin it by digest, never a moving tag, or a server change could quietly
alter what this test means.

usage: test_sync_flow.py ORACLES_NATIVE ROM    (exit 77 when the ROM or the image is missing)"""
import os, socket, subprocess, sys, tempfile, threading, time, urllib.error, urllib.request

# TODO: pin by digest (ghcr.io/kirby-letsgo/save-sync-server@sha256:...) once one is published.
# ORACLES_SYNC_IMAGE points at another one, to try a server built locally (pnpm image:test).
SERVER_IMAGE = (
    os.environ.get('ORACLES_SYNC_IMAGE') or 'ghcr.io/kirby-letsgo/save-sync-server:0.1.0-test'
)

app, rom = (os.path.abspath(p) for p in sys.argv[1:3])
if not os.path.exists(rom): print('skip: ROM not present'); sys.exit(77)


def docker(*args, **kw):
    return subprocess.run(['docker', *args], capture_output=True, text=True, **kw)


def both_streams(done):
    return (done.stdout + done.stderr).strip()


def last_line(done):
    return both_streams(done).splitlines()[-1:] or ['no output from docker']


def skip(why):
    print(f'skip: {why}')
    sys.exit(77)


try:
    if docker('info').returncode != 0: skip('the docker daemon is not running')
except FileNotFoundError:
    skip('docker is not installed')
if docker('image', 'inspect', SERVER_IMAGE).returncode != 0:
    print(f'pulling {SERVER_IMAGE}')
    pull = docker('pull', '--quiet', SERVER_IMAGE)
    if pull.returncode != 0:
        skip(f'the sync server image is not available: {last_line(pull)[0]}')

env = dict(os.environ, SDL_VIDEO_DRIVER='dummy', SDL_AUDIO_DRIVER='dummy')
THUMB = 160 * 144 * 3

started = docker('run', '--rm', '-d', '-p', '127.0.0.1::3000', SERVER_IMAGE)
assert started.returncode == 0, started.stderr
container = started.stdout.strip()
try:
    mapped = docker('port', container, '3000/tcp')
    assert mapped.returncode == 0, mapped.stderr
    url = 'http://' + mapped.stdout.strip().splitlines()[0]

    def request(method, path, data=None):
        with urllib.request.urlopen(urllib.request.Request(url + path, data=data, method=method), timeout=10) as r:
            return r.read()

    for attempt in range(60):                  # the server migrates its in-memory database first
        try:
            if request('GET', '/health'): break
        except (urllib.error.URLError, OSError):
            time.sleep(1)
    else:
        logs = both_streams(docker('logs', container))
        raise AssertionError(f'the server never answered /health; its logs:\n{logs}')

    code = request('POST', '/accounts').decode().split('"code":"')[1].split('"')[0].replace('-', '')

    def on_server(name):
        return request('GET', f'/accounts/{code}/files/seasons/{name}')

    # the server behind a port that refuses connections until online() (the device is offline)
    probe = socket.socket()
    probe.bind(('127.0.0.1', 0))
    offline_port = probe.getsockname()[1]
    probe.close()

    def online():
        listener = socket.create_server(('127.0.0.1', offline_port))
        host, port = url[len('http://'):].split(':')

        def pump(src, dst):
            try:
                while data := src.recv(65536): dst.sendall(data)
            except OSError: pass
            finally:
                for s in (src, dst):
                    try: s.shutdown(socket.SHUT_RDWR)
                    except OSError: pass

        def serve():
            while True:
                client, _ = listener.accept()
                upstream = socket.create_connection((host, int(port)))
                threading.Thread(target=pump, args=(client, upstream), daemon=True).start()
                threading.Thread(target=pump, args=(upstream, client), daemon=True).start()
        threading.Thread(target=serve, daemon=True).start()

    with tempfile.TemporaryDirectory() as a, tempfile.TemporaryDirectory() as b:
        def install(cache, server_url):
            r = subprocess.run([app, rom, '--cache', cache, '--frames', '1'], env=env, capture_output=True, text=True, timeout=120)
            assert r.returncode == 0, r.stderr
            with open(os.path.join(cache, 'sync.ini'), 'w') as f: f.write(f'url={server_url}\ncode={code}\n')

        def save(cache, data, shade):
            with open(os.path.join(cache, 'seasons', 'state_1'), 'wb') as f: f.write(data)
            with open(os.path.join(cache, 'seasons', 'state_1.thumb'), 'wb') as f: f.write(bytes([shade]) * THUMB)
            time.sleep(1.1)                    # a later mtime than the last sync wrote

        def local(cache, name='state_1'):
            with open(os.path.join(cache, 'seasons', name), 'rb') as f: return f.read()

        def launcher(cache, keys, during=None):
            p = subprocess.Popen([app, '--cache', cache], env=dict(env, ORACLES_TEST_KEYS=keys), stderr=subprocess.PIPE, text=True)
            if during: during()
            try:
                _, err = p.communicate(timeout=45)
            except subprocess.TimeoutExpired:
                p.kill()
                _, err = p.communicate()
                raise AssertionError(f'the app never got to the keys {keys}; stderr: {err}')
            assert p.returncode == 0, err
            return err

        install(a, url)
        install(b, url)
        synced_then_quit = 'synced+5:Z'

        save(a, b'A1' * 1000, 10)
        launcher(a, synced_then_quit)
        assert on_server('state_1') == b'A1' * 1000
        assert on_server('state_1.thumb') == bytes([10]) * THUMB

        launcher(b, synced_then_quit)
        assert local(b) == b'A1' * 1000
        assert local(b, 'state_1.thumb') == bytes([10]) * THUMB

        save(a, b'A2' * 1000, 20)
        launcher(a, synced_then_quit)
        assert on_server('state_1') == b'A2' * 1000

        save(b, b'B2' * 1000, 30)                                 # B never saw A2: keep which?
        launcher(b, 'conflict+10:Left,conflict+20:X,conflict+40:Z')
        assert on_server('state_1') == b'B2' * 1000, 'HERE did not upload B\'s slot'
        assert on_server('state_1.thumb') == bytes([30]) * THUMB
        assert local(b) == b'B2' * 1000

        save(a, b'A3' * 1000, 40)                                 # A never saw B2: keep which?
        launcher(a, 'conflict+10:Right,conflict+20:X,conflict+40:Z')
        assert local(a) == b'B2' * 1000, 'OTHER did not bring B\'s slot'
        assert local(a, 'state_1.thumb') == bytes([30]) * THUMB
        assert on_server('state_1') == b'B2' * 1000
        assert local(a, 'state_1.bak') == b'A3' * 1000, 'OTHER did not keep A\'s slot as .bak'
        assert local(a, 'state_1.thumb.bak') == bytes([40]) * THUMB

        with open(os.path.join(a, 'sync.ini'), 'w') as f: f.write(f'url=http://127.0.0.1:{offline_port}\ncode={code}\n')
        save(a, b'A4' * 1000, 50)
        err = launcher(a, synced_then_quit, during=lambda: (time.sleep(2), online()))
        assert 'sync failed' in err, err
        assert on_server('state_1') == b'A4' * 1000, 'the retry did not upload the slot saved offline'
        assert on_server('state_1.thumb') == bytes([50]) * THUMB
finally:
    docker('rm', '-f', container)
print('ok sync_flow')
