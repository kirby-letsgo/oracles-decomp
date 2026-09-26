import {
  customType,
  integer,
  pgTable,
  serial,
  text,
  timestamp,
  uniqueIndex,
} from 'drizzle-orm/pg-core';

const bytea = customType<{ data: Buffer; driverData: Uint8Array }>({
  dataType: () => 'bytea',
  fromDriver: (value) => Buffer.from(value),
});

// An account is its sync code: 16 random digits, the only thing a device needs to find it. The
// serial id never leaves the server.
export const accounts = pgTable('accounts', {
  id: serial('id').primaryKey(),
  code: text('code').notNull().unique(),
  createdAt: timestamp('created_at', { withTimezone: true }).notNull().defaultNow(),
  lastSeenAt: timestamp('last_seen_at', { withTimezone: true }).notNull().defaultNow(),
});

// Every upload is a new version of (game, name); the newest is current, older ones are history.
export const files = pgTable(
  'files',
  {
    id: serial('id').primaryKey(),
    accountId: integer('account_id')
      .notNull()
      .references(() => accounts.id, { onDelete: 'cascade' }),
    game: text('game').notNull(),
    name: text('name').notNull(),
    version: integer('version').notNull(),
    data: bytea('data').notNull(),
    size: integer('size').notNull(),
    sha256: text('sha256').notNull(),
    device: text('device'),
    formatVersion: integer('format_version'),
    createdAt: timestamp('created_at', { withTimezone: true }).notNull().defaultNow(),
  },
  (t) => [
    uniqueIndex('files_account_game_name_version').on(t.accountId, t.game, t.name, t.version),
  ],
);
