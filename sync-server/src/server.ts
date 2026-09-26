import { buildApp } from './app.js';
import { connect } from './db/index.js';

const url = process.env.DATABASE_URL;
if (!url) {
  console.error('DATABASE_URL is not set (see .env.example)');
  process.exit(1);
}

const { db, close } = await connect(url);
const app = await buildApp({ db, logger: true, trustProxy: process.env.TRUST_PROXY !== 'false' });
for (const signal of ['SIGINT', 'SIGTERM'] as const) {
  process.once(signal, () => {
    void app.close().then(close);
  });
}
await app.listen({ port: Number(process.env.PORT ?? 3000), host: process.env.HOST ?? '0.0.0.0' });
