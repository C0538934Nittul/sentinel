/**
 * Shared TypeScript types mirroring the security-event shape in docs/data-contract.md.
 * Component: api/src/types
 * Status: complete (type declarations only, no logic to stub).
 */

export const EVENT_TYPES = [
  "AUTH_SUCCESS",
  "AUTH_FAILURE",
  "FILE_ACCESS",
  "PROCESS_START",
  "NETWORK_CONNECTION",
] as const;

export type EventType = (typeof EVENT_TYPES)[number];

export interface SecurityEvent {
  eventId: string;
  timestamp: string; // ISO 8601 UTC, millisecond precision
  eventType: EventType;
  sourceIp: string;
  account?: string;
  host: string;
  details?: Record<string, unknown>;
}

export interface SecurityEventFile {
  events: SecurityEvent[];
}
