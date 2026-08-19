/**
 * Handlers for /v1/events. Zod request schemas validate incoming requests; both handlers call
 * through to src/services/eventService.ts, which is now implemented against MongoDB (Step 3,
 * Phase 5 -- first draft, not the final assessed version).
 * Component: api/src/controllers
 */

import type { NextFunction, Request, Response } from "express";
import { z } from "zod";
import * as eventService from "../services/eventService.js";
import { ValidationError } from "../middleware/errorHandler.js";
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
export async function listEvents(req: Request, res: Response, next: NextFunction): Promise<void> {
  const parsed = listEventsQuerySchema.safeParse(req.query);
  if (!parsed.success) {
    next(new ValidationError("Invalid query parameters", parsed.error.flatten()));
    return;
  }

  try {
    const events = await eventService.listEvents(parsed.data);
    res.status(200).json(events);
  } catch (err) {
    next(err);
  }
}

/**
 * POST /v1/events
 * Input: request body validated by createEventsBodySchema (a single event or { events: [...] }).
 * Output: 201 SecurityEvent[] (the created events)
 * Error cases: 400 on schema validation failure, 409 if any eventId already exists.
 */
export async function createEvents(req: Request, res: Response, next: NextFunction): Promise<void> {
  const parsed = createEventsBodySchema.safeParse(req.body);
  if (!parsed.success) {
    next(new ValidationError("Invalid event payload", parsed.error.flatten()));
    return;
  }

  const events = "events" in parsed.data ? parsed.data.events : [parsed.data];

  try {
    const created = await eventService.createEvents(events);
    res.status(201).json(created);
  } catch (err) {
    next(err);
  }
}
