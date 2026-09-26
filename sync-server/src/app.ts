import rateLimit from '@fastify/rate-limit';
import { and, desc, eq, lt } from 'drizzle-orm';
import Fastify, { type FastifyInstance } from 'fastify';
import { createHash } from 'node:crypto';
import { z } from 'zod';
import { formatCode, newCode, normalizeCode } from './codes.js';
import type { Db } from './db/index.js';
import { accounts, files } from './db/schema.js';
import { KEEP_VERSIONS, MAX_FILE_BYTES, allowedFile, needsFormatVersion } from './files.js';

export interface AppOptions {
  db: Db;
  logger?: boolean;
  accountsPerHour?: number;
}

const fileParams = z.object({ code: z.string(), game: z.string(), name: z.string() });
const uploadHeaders = z.object({
  'x-device': z.string().max(64).optional(),
  'x-format-version': z.coerce.number().int().nonnegative().optional(),
  'x-base-version': z.coerce.number().int().nonnegative().optional(),
});

const fileMeta = {
  game: files.game,
  name: files.name,
  version: files.version,
  size: files.size,
  sha256: files.sha256,
  device: files.device,
  formatVersion: files.formatVersion,
  updatedAt: files.createdAt,
};

export async function buildApp({
  db,
  logger = false,
  accountsPerHour = 10,
}: AppOptions): Promise<FastifyInstance> {
  const app = Fastify({ logger, bodyLimit: MAX_FILE_BYTES });
  await app.register(rateLimit, { global: false });
  app.addContentTypeParser('application/octet-stream', { parseAs: 'buffer' }, (_req, body, done) =>
    done(null, body),
  );
  app.setErrorHandler((error, _req, reply) => {
    if (error instanceof z.ZodError) return reply.code(400).send({ error: 'bad request' });
    return reply.send(error);
  });

  // The account behind a code (any spacing or dashes), touching its last-seen time.
  async function findAccount(input: string) {
    const code = normalizeCode(input);
    if (!code) return null;
    const [row] = await db
      .update(accounts)
      .set({ lastSeenAt: new Date() })
      .where(eq(accounts.code, code))
      .returning({ id: accounts.id, code: accounts.code, createdAt: accounts.createdAt });
    return row ?? null;
  }

  app.get('/health', async () => ({ ok: true }));

  app.post(
    '/accounts',
    { config: { rateLimit: { max: accountsPerHour, timeWindow: '1 hour' } } },
    async (_req, reply) => {
      for (let attempt = 0; attempt < 5; attempt++) {
        const [row] = await db
          .insert(accounts)
          .values({ code: newCode() })
          .onConflictDoNothing()
          .returning({ code: accounts.code });
        if (row) return reply.code(201).send({ code: formatCode(row.code) });
      }
      return reply.code(503).send({ error: 'could not allocate a code' });
    },
  );

  app.get('/accounts/:code', async (req, reply) => {
    const { code } = z.object({ code: z.string() }).parse(req.params);
    const account = await findAccount(code);
    if (!account) return reply.code(404).send({ error: 'no such account' });
    const current = await db
      .selectDistinctOn([files.game, files.name], fileMeta)
      .from(files)
      .where(eq(files.accountId, account.id))
      .orderBy(files.game, files.name, desc(files.version));
    return { code: formatCode(account.code), createdAt: account.createdAt, files: current };
  });

  app.put('/accounts/:code/files/:game/:name', async (req, reply) => {
    const { code, game, name } = fileParams.parse(req.params);
    if (!allowedFile(game, name)) return reply.code(400).send({ error: 'this file is not synced' });
    if (!Buffer.isBuffer(req.body)) {
      return reply.code(415).send({ error: 'send the file as application/octet-stream' });
    }
    const headers = uploadHeaders.parse(req.headers);
    if (needsFormatVersion(name) && headers['x-format-version'] === undefined) {
      return reply.code(400).send({ error: 'save states need x-format-version' });
    }
    const account = await findAccount(code);
    if (!account) return reply.code(404).send({ error: 'no such account' });

    const data = req.body;
    const sha256 = createHash('sha256').update(data).digest('hex');
    const same = and(eq(files.accountId, account.id), eq(files.game, game), eq(files.name, name));
    const result = await db.transaction(async (tx) => {
      const [latest] = await tx
        .select({ version: files.version, sha256: files.sha256 })
        .from(files)
        .where(same)
        .orderBy(desc(files.version))
        .limit(1);
      const current = latest?.version ?? 0;
      const base = headers['x-base-version'];
      if (base !== undefined && base !== current) return { conflict: current };
      if (latest?.sha256 === sha256) return { version: current, created: false };
      const version = current + 1;
      await tx.insert(files).values({
        accountId: account.id,
        game,
        name,
        version,
        data,
        size: data.length,
        sha256,
        device: headers['x-device'] ?? null,
        formatVersion: headers['x-format-version'] ?? null,
      });
      await tx.delete(files).where(and(same, lt(files.version, version - KEEP_VERSIONS + 1)));
      return { version, created: true };
    });
    if ('conflict' in result) {
      return reply
        .code(409)
        .send({ error: 'the file changed on another device', version: result.conflict });
    }
    return reply.code(result.created ? 201 : 200).send({ version: result.version, sha256 });
  });

  app.get('/accounts/:code/files/:game/:name', async (req, reply) => {
    const { code, game, name } = fileParams.parse(req.params);
    const { version } = z
      .object({ version: z.coerce.number().int().positive().optional() })
      .parse(req.query);
    const account = await findAccount(code);
    if (!account) return reply.code(404).send({ error: 'no such account' });
    const where = and(eq(files.accountId, account.id), eq(files.game, game), eq(files.name, name));
    const [row] = await db
      .select({
        data: files.data,
        version: files.version,
        sha256: files.sha256,
        formatVersion: files.formatVersion,
      })
      .from(files)
      .where(version === undefined ? where : and(where, eq(files.version, version)))
      .orderBy(desc(files.version))
      .limit(1);
    if (!row) return reply.code(404).send({ error: 'no such file' });
    reply.header('content-type', 'application/octet-stream');
    reply.header('x-version', String(row.version));
    reply.header('x-sha256', row.sha256);
    if (row.formatVersion !== null) reply.header('x-format-version', String(row.formatVersion));
    return reply.send(row.data);
  });

  app.get('/accounts/:code/files/:game/:name/history', async (req, reply) => {
    const { code, game, name } = fileParams.parse(req.params);
    const account = await findAccount(code);
    if (!account) return reply.code(404).send({ error: 'no such account' });
    return db
      .select(fileMeta)
      .from(files)
      .where(and(eq(files.accountId, account.id), eq(files.game, game), eq(files.name, name)))
      .orderBy(desc(files.version));
  });

  return app;
}
