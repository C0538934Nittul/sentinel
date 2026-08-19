/**
 * Business logic for security events: listing and ingesting.
 * Component: api/src/services
 * Status: First-draft implementation (Step 3, Phase 5) against MongoDB via Mongoose. Not the
 *         final assessed version -- review and rewrite as needed.
 */

import { SecurityEventModel } from "../models/SecurityEvent.js";
import { ConflictError } from "../middleware/errorHandler.js";
import type { EventType, SecurityEvent } from "../types/securityEvent.js";

export interface ListEventsFilter {
  sourceIp?: string | undefined;
  eventType?: EventType | undefined;
  limit?: number | undefined;
}

/**
 * List persisted events, optionally filtered.
 * @param filter Optional sourceIp/eventType/limit constraints.
 * @returns Events in descending timestamp order.
 */
export async function listEvents(filter: ListEventsFilter = {}): Promise<SecurityEvent[]> {
  const query: Record<string, unknown> = {};
  if (filter.sourceIp) query.sourceIp = filter.sourceIp;
  if (filter.eventType) query.eventType = filter.eventType;

  const documents = await SecurityEventModel.find(query)
    .sort({ timestamp: -1 })
    .limit(filter.limit ?? 100)
    .exec();

  return documents.map((doc) => doc.toJSON() as unknown as SecurityEvent);
}

/**
 * Persist one or more validated events.
 * @param events Events already validated against the SecurityEvent schema by the controller.
 * @returns The persisted events (with any server-assigned fields applied).
 * @throws ConflictError if any `eventId` already exists.
 */
export async function createEvents(events: SecurityEvent[]): Promise<SecurityEvent[]> {
  try {
    const created = await SecurityEventModel.insertMany(events, { ordered: true });
    return created.map((doc) => doc.toJSON() as unknown as SecurityEvent);
  } catch (err) {
    // Mongo duplicate-key error on the unique eventId index.
    if (typeof err === "object" && err !== null && "code" in err && (err as { code?: number }).code === 11000) {
      const duplicateEventIds = events.map((e) => e.eventId);
      throw new ConflictError("One or more eventId values already exist", { eventIds: duplicateEventIds });
    }
    throw err;
  }
}
