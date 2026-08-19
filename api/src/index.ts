/**
 * Process entry point: connects to MongoDB (with retry), starts the HTTP server, and wires up
 * graceful shutdown on SIGINT/SIGTERM.
 *
 * Component: api/src
 * Status: complete (infrastructure, not assessed logic).
 */

import mongoose from "mongoose";
import pino from "pino";
import { createApp } from "./app.js";
import { env } from "./config/env.js";

const logger = pino({ level: env.logLevel });

const MONGO_RETRY_DELAY_MS = 2000;
const MONGO_MAX_RETRIES = 10;

async function connectToMongo(): Promise<void> {
  for (let attempt = 1; attempt <= MONGO_MAX_RETRIES; attempt += 1) {
    try {
      await mongoose.connect(env.mongodbUri);
      logger.info({ attempt }, "connected to MongoDB");
      return;
    } catch (err) {
      logger.warn({ attempt, err }, "MongoDB connection attempt failed, retrying");
      if (attempt === MONGO_MAX_RETRIES) {
        throw err;
      }
      await new Promise((resolve) => setTimeout(resolve, MONGO_RETRY_DELAY_MS));
    }
  }
}

async function main(): Promise<void> {
  await connectToMongo();

  const app = createApp();
  const server = app.listen(env.port, () => {
    logger.info({ port: env.port }, "sentinel-api listening");
  });

  const shutdown = (signal: string) => {
    logger.info({ signal }, "shutting down");
    server.close(() => {
      mongoose.connection
        .close()
        .then(() => {
          logger.info("MongoDB connection closed, exiting");
          process.exit(0);
        })
        .catch((err) => {
          logger.error({ err }, "error closing MongoDB connection");
          process.exit(1);
        });
    });
  };

  process.on("SIGINT", () => shutdown("SIGINT"));
  process.on("SIGTERM", () => shutdown("SIGTERM"));
}

main().catch((err) => {
  logger.error({ err }, "fatal startup error");
  process.exit(1);
});
