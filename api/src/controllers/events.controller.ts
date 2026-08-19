/**
 * Handlers for /v1/events. Zod request schemas are written in full; handler bodies are
 * stubbed (business logic lives in src/services/eventService.ts, also stubbed).
 * Component: api/src/controllers
 */

import type { NextFunction, Request, Response } from "express";
import { z } from "zod";
import { EVENT_TYPES } from "../types/securityEvent.js";

const securityEventSchema = z.object({
  eventId: z.string().min(1),
  timestamp: z.string().datetime({ message: "timestamp must be ISO 8601 UTC" }),
  eventType: z.enum(EVENT_TYPES),
  sourceIp: z.string().regex(/^(\d{1,3}\.){3}\d{1,3}$/, "sourceIp must be an IPv4 dotted-quad"),
  account: z.string().min(1).optional(),
  host: z.string().min(1),
  details: z.record(z.string(), z.unknown()).optional(),
});

export const listEventsQuerySchema = z.object({
  sourceIp: z.string().optional(),
  eventType: z.enum(EVENT_TYPES).optional(),
  limit: z.coerce.number().int().positive().max(1000).optional(),
});

export const createEventsBodySchema = z.union([
  securityEventSchema,
  z.object({ events: z.array(securityEventSchema).min(1) }),
]);

/**
 * GET /v1/events
 * Input: query params validated by listEventsQuerySchema.
 * Output: 200 SecurityEvent[]
 * Error cases: 400 if query params fail validation.
 */
export function listEvents(req: Request, res: Response, next: NextFunction): void {
  // TODO(student): parse req.query with listEventsQuerySchema, call
  // eventService.listEvents(filter), respond 200 with the array. Pass validation failures and
  // unexpected errors to next(err) for errorHandler to render.
  void req;
  void res;
  void next;
  throw new Error("events.controller.listEvents not implemented");
}

/**
 * POST /v1/events
 * Input: request body validated by createEventsBodySchema (a single event or { events: [...] }).
 * Output: 201 SecurityEvent[] (the created events)
 * Error cases: 400 on schema validation failure, 409 if any eventId already exists.
 */
export function createEvents(req: Request, res: Response, next: NextFunction): void {
  // TODO(student): parse req.body with createEventsBodySchema, normalize to an array, call
  // eventService.createEvents(events), respond 201 with the created events.
  void req;
  void res;
  void next;
  throw new Error("events.controller.createEvents not implemented");
}
