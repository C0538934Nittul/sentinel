/**
 * Handler for GET /v1/health.
 * Component: api/src/controllers
 * Status: STUBBED body -- trivial, but left for the student to wire up mongoose.connection
 *         readyState into the response.
 */

import type { Request, Response } from "express";

/**
 * GET /v1/health
 * Input: none.
 * Output: 200 { status: "ok", mongo: "connected" | "disconnected", uptimeSeconds: number }
 * Error cases: none -- this endpoint should not throw; it reports degraded state in the body
 * rather than via a non-2xx status, so uptime monitors can distinguish "API is up but Mongo
 * isn't" from "API is unreachable."
 */
export function getHealth(_req: Request, res: Response): void {
  // TODO(student): report mongoose.connection.readyState and process.uptime().
  void res;
  throw new Error("health.controller.getHealth not implemented");
}
