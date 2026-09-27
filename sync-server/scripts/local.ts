// The server on an in-memory database, for the app's end-to-end sync test (tests/test_sync_flow.py):
// prints its address once it listens. PORT=0 (the default) picks a free port.
import { PGlite } from '@electric-sql/pglite';
import { drizzle } from 'drizzle-orm/pglite';
import { migrate } from 'drizzle-orm/pglite/migrator';
import { buildApp } from '../src/app.js';
import { migrationsFolder } from '../src/db/index.js';
import * as schema from '../src/db/schema.js';

const db = drizzle(new PGlite(), { schema });
await migrate(db, { migrationsFolder });
const app = await buildApp({ db });
for (const signal of ['SIGINT', 'SIGTERM'] as const) {
  process.once(signal, () => {
    void app.close();
  });
}
console.log(await app.listen({ port: Number(process.env.PORT ?? 0), host: '127.0.0.1' }));
