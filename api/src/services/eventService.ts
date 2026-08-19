/**
 * Business logic for security events: listing and ingesting.
 * Component: api/src/services
 * Status: STUBBED -- signatures and contracts only. Bodies are TODO(student).
 */

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
  // TODO(student): query SecurityEventModel using `filter`, map documents to the
  // SecurityEvent wire shape (toJSON transform already handles _id -> id / __v stripping).
  void filter;
  throw new Error("eventService.listEvents not implemented");
}

/**
 * Persist one or more validated events.
 * @param events Events already validated against the SecurityEvent schema by the controller.
 * @returns The persisted events (with any server-assigned fields applied).
 * @throws ConflictError if any `eventId` already exists.
 */
export async function createEvents(events: SecurityEvent[]): Promise<SecurityEvent[]> {
  // TODO(student): insert `events` via SecurityEventModel, catching the Mongo duplicate-key
  // error (code 11000) on the unique eventId index and re-throwing as ConflictError with the
  // offending eventId(s) in `details`.
  void events;
  throw new Error("eventService.createEvents not implemented");
}
