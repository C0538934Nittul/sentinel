/**
 * Shared TypeScript types mirroring docs/data-contract.md. Kept independent from api/src/types
 * (no cross-package import) since the client and API are deployed/built separately -- see
 * docs/ASSUMPTIONS.md if you'd rather extract a shared package later.
 * Component: client/lib
 * Status: complete.
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
  timestamp: string;
  eventType: EventType;
  sourceIp: string;
  account?: string;
  host: string;
  details?: Record<string, unknown>;
}

export const INCIDENT_SEVERITIES = ["LOW", "MEDIUM", "HIGH"] as const;
export type IncidentSeverity = (typeof INCIDENT_SEVERITIES)[number];

export const INCIDENT_STATUSES = ["Open", "Investigating", "Closed"] as const;
export type IncidentStatus = (typeof INCIDENT_STATUSES)[number];

export interface Incident {
  id: string;
  ruleId: string;
  severity: IncidentSeverity;
  riskScore: number;
  detectedAt: string;
  supportingEventIds: string[];
  summary: string;
  status: IncidentStatus;
}

export interface ApiErrorBody {
  error: {
    code: string;
    message: string;
    details?: unknown;
  };
}
