/**
 * Shared TypeScript types mirroring the incident-result shape in docs/data-contract.md.
 * Component: api/src/types
 * Status: complete (type declarations only, no logic to stub).
 */

export const INCIDENT_SEVERITIES = ["LOW", "MEDIUM", "HIGH"] as const;
export type IncidentSeverity = (typeof INCIDENT_SEVERITIES)[number];

export const INCIDENT_STATUSES = ["Open", "Investigating", "Closed"] as const;
export type IncidentStatus = (typeof INCIDENT_STATUSES)[number];

/** Shape produced by the C++ analyzer, before persistence assigns id/status. */
export interface AnalyzerIncident {
  ruleId: string;
  severity: IncidentSeverity;
  riskScore: number;
  detectedAt: string;
  supportingEventIds: string[];
  summary: string;
}

/** Shape returned by the API once an incident has been persisted. */
export interface Incident extends AnalyzerIncident {
  id: string;
  status: IncidentStatus;
}

/** Shape of the JSON the analyzer binary writes to stdout: { "incidents": [...] }. */
export interface AnalyzerOutput {
  incidents: AnalyzerIncident[];
}
