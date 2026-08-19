/**
 * Express application assembly: security middleware, logging, route mounting, and error
 * handling. Does not start listening -- see src/index.ts for that (kept separate so tests can
 * import the app without binding a port).
 *
 * Component: api/src
 * Status: complete (infrastructure, not assessed logic).
 */

import cors from "cors";
import express, { type Express } from "express";
import rateLimit from "express-rate-limit";
import helmet from "helmet";
// Named import, not default -- pino-http's .d.ts uses ESM `export default` syntax over a
// plain-CJS package with no "exports" field, which trips up NodeNext's default-import interop
// (TS2349: "not callable"). The named export `pinoHttp` it also provides sidesteps that.
import { pinoHttp } from "pino-http";
import { env } from "./config/env.js";
import { errorHandler, notFoundHandler } from "./middleware/errorHandler.js";
import eventsRoutes from "./routes/events.routes.js";
import healthRoutes from "./routes/health.routes.js";
import incidentsRoutes from "./routes/incidents.routes.js";

export function createApp(): Express {
  const app = express();

  // --- Documented web-service security control (see docs/professional-practice.md) ---
  // Three layers, each covering a different class of risk:
  //   1. helmet: sets a battery of protective HTTP response headers (X-Content-Type-Options,
  //      X-Frame-Options, a conservative Content-Security-Policy default, etc.) to reduce
  //      exposure to MIME-sniffing, clickjacking, and related browser-side attacks.
  //   2. cors, restricted to CLIENT_ORIGIN: the API is not meant to be called from arbitrary
  //      origins -- only the Sentinel dashboard should be able to make browser-based requests.
  //   3. Zod input validation (see controllers/*): every request body/params/query is parsed
  //      against an explicit schema before touching any business logic, so malformed or
  //      malicious input is rejected at the boundary rather than trusted downstream.
  app.use(helmet());
  app.use(cors({ origin: env.clientOrigin }));

  app.use(express.json({ limit: "1mb" }));

  app.use(
    rateLimit({
      windowMs: 60_000,
      limit: 120,
      standardHeaders: true,
      legacyHeaders: false,
    }),
  );

  app.use(pinoHttp({ level: env.logLevel }));

  app.use("/v1/health", healthRoutes);
  app.use("/v1/events", eventsRoutes);
  app.use("/v1/incidents", incidentsRoutes);

  app.use(notFoundHandler);
  app.use(errorHandler);

  return app;
}
