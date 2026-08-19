/**
 * Handler for GET /v1/health.
 * Component: api/src/controllers
 * Status: implemented (Phase 3 -- infrastructure, not assessed logic; the API's own bootstrap
 *         diagnostics don't touch detection-rule/correlation/scoring code).
 */

import { existsSync } from "node:fs";
import type { Request, Response } from "express";
import mongoose from "mongoose";
import { env } from "../config/env.js";

/**
 * GET /v1/health
 * Input: none.
 * Output: 200 { status: "ok", mongo: "connected" | "disconnected", analyzerBinaryFound: boolean,
 *               uptimeSeconds: number }
 * Error cases: none -- this endpoint does not throw; it reports degraded state in the body
 * rather than via a non-2xx status, so uptime monitors can distinguish "API is up but Mongo
 * isn't" from "API is unreachable."
 */
export function getHealth(_req: Request, res: Response): void {
  res.status(200).json({
    status: "ok",
    mongo: mongoose.connection.readyState === 1 ? "connected" : "disconnected",
    analyzerBinaryFound: existsSync(env.analyzerBinPath),
    uptimeSeconds: process.uptime(),
  });
}
