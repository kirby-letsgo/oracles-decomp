import { PGlite } from '@electric-sql/pglite';
import { drizzle } from 'drizzle-orm/pglite';
import { migrate } from 'drizzle-orm/pglite/migrator';
import type { FastifyInstance } from 'fastify';
import { afterEach, beforeEach, describe, expect, it } from 'vitest';
import { buildApp } from '../src/app.js';
import { migrationsFolder } from '../src/db/index.js';
import * as schema from '../src/db/schema.js';
import { KEEP_VERSIONS } from '../src/files.js';

let app: FastifyInstance;

beforeEach(async () => {
  const db = drizzle(new PGlite(), { schema });
  await migrate(db, { migrationsFolder });
  app = await buildApp({ db, accountsPerHour: 3 });
});

afterEach(async () => {
  await app.close();
});

async function newAccount(): Promise<string> {
  const res = await app.inject({ method: 'POST', url: '/accounts' });
  expect(res.statusCode).toBe(201);
  return res.json<{ code: string }>().code;
}

function upload(code: string, game: string, name: string, data: Buffer, headers = {}) {
  return app.inject({
    method: 'PUT',
    url: `/accounts/${code}/files/${game}/${name}`,
    headers: { 'content-type': 'application/octet-stream', ...headers },
    payload: data,
  });
}

describe('accounts', () => {
  it('creates a 16-digit code and finds it with or without dashes', async () => {
    const code = await newAccount();
    expect(code).toMatch(/^\d{4}-\d{4}-\d{4}-\d{4}$/);
    const plain = code.replaceAll('-', '');
    const res = await app.inject({ method: 'GET', url: `/accounts/${plain}` });
    expect(res.statusCode).toBe(200);
    expect(res.json()).toMatchObject({ code, files: [] });
  });

  it('answers 404 for a code nobody created', async () => {
    const res = await app.inject({ method: 'GET', url: '/accounts/0000-0000-0000-0000' });
    expect(res.statusCode).toBe(404);
  });

  it('limits account creation per address', async () => {
    for (let i = 0; i < 3; i++) await newAccount();
    const res = await app.inject({ method: 'POST', url: '/accounts' });
    expect(res.statusCode).toBe(429);
  });
});

describe('files', () => {
  it('stores a save, skips identical uploads and serves the bytes back', async () => {
    const code = await newAccount();
    const sram = Buffer.alloc(8192, 7);
    const first = await upload(code, 'seasons', 'sram.sav', sram, { 'x-device': 'pixel 8' });
    expect(first.statusCode).toBe(201);
    expect(first.json()).toMatchObject({ version: 1 });
    const again = await upload(code, 'seasons', 'sram.sav', sram);
    expect(again.statusCode).toBe(200);
    expect(again.json()).toMatchObject({ version: 1 });

    const res = await app.inject({
      method: 'GET',
      url: `/accounts/${code}/files/seasons/sram.sav`,
    });
    expect(res.statusCode).toBe(200);
    expect(res.headers['x-version']).toBe('1');
    expect(res.rawPayload.equals(sram)).toBe(true);

    const list = await app.inject({ method: 'GET', url: `/accounts/${code}` });
    expect(list.json().files).toMatchObject([
      { game: 'seasons', name: 'sram.sav', version: 1, size: 8192, device: 'pixel 8' },
    ]);
  });

  it('only accepts the synced files', async () => {
    const code = await newAccount();
    for (const [game, name] of [
      ['seasons', 'rom.bin'],
      ['seasons', 'cyctab.bin'],
      ['zelda', 'sram.sav'],
      ['shared', 'sram.sav'],
    ]) {
      const res = await upload(code, game!, name!, Buffer.from('x'));
      expect(res.statusCode).toBe(400);
    }
    expect(
      (await upload(code, 'shared', 'settings.ini', Buffer.from('volume=9\n'))).statusCode,
    ).toBe(201);
    expect((await upload(code, 'ages', 'state_2.thumb', Buffer.alloc(10))).statusCode).toBe(201);
  });

  it('needs a format version on save states and returns it', async () => {
    const code = await newAccount();
    const state = Buffer.alloc(1000, 1);
    expect((await upload(code, 'ages', 'state_auto', state)).statusCode).toBe(400);
    const res = await upload(code, 'ages', 'state_auto', state, { 'x-format-version': '3' });
    expect(res.statusCode).toBe(201);
    const got = await app.inject({ method: 'GET', url: `/accounts/${code}/files/ages/state_auto` });
    expect(got.headers['x-format-version']).toBe('3');
  });

  it('refuses an upload based on an older version', async () => {
    const code = await newAccount();
    await upload(code, 'ages', 'sram.sav', Buffer.from('a'));
    await upload(code, 'ages', 'sram.sav', Buffer.from('b'), { 'x-base-version': '1' });
    const stale = await upload(code, 'ages', 'sram.sav', Buffer.from('c'), {
      'x-base-version': '1',
    });
    expect(stale.statusCode).toBe(409);
    expect(stale.json()).toMatchObject({ version: 2 });
  });

  it('keeps the last versions as history and serves any of them', async () => {
    const code = await newAccount();
    for (let i = 1; i <= KEEP_VERSIONS + 2; i++) {
      await upload(code, 'seasons', 'sram.sav', Buffer.from(`save ${i}`));
    }
    const history = await app.inject({
      method: 'GET',
      url: `/accounts/${code}/files/seasons/sram.sav/history`,
    });
    const versions = history.json<{ version: number }[]>().map((v) => v.version);
    expect(versions).toHaveLength(KEEP_VERSIONS);
    expect(versions[0]).toBe(KEEP_VERSIONS + 2);
    const old = await app.inject({
      method: 'GET',
      url: `/accounts/${code}/files/seasons/sram.sav?version=${KEEP_VERSIONS}`,
    });
    expect(old.rawPayload.toString()).toBe(`save ${KEEP_VERSIONS}`);
    const pruned = await app.inject({
      method: 'GET',
      url: `/accounts/${code}/files/seasons/sram.sav?version=1`,
    });
    expect(pruned.statusCode).toBe(404);
  });
});
