/**
 * Business logic for incidents: listing, lookup, status updates, deletion, and triggering
 * analysis runs.
 * Component: api/src/services
 * Status: STUBBED -- signatures and contracts only. Bodies are TODO(student).
 */

import type { Incident, IncidentStatus } from "../types/incident.js";

export interface ListIncidentsFilter {
  status?: IncidentStatus;
  limit?: number;
}

/**
 * List persisted incidents, optionally filtered by status.
 */
export async function listIncidents(filter: ListIncidentsFilter = {}): Promise<Incident[]> {
  // TODO(student): query IncidentModel using `filter`, most recent detectedAt first.
  void filter;
  throw new Error("incidentService.listIncidents not implemented");
}

/**
 * Fetch a single incident by id.
 * @throws NotFoundError if no incident with that id exists.
 */
export async function getIncidentById(id: string): Promise<Incident> {
  // TODO(student): look up by _id, throw NotFoundError if missing or if `id` is not a valid
  // ObjectId shape.
  void id;
  throw new Error("incidentService.getIncidentById not implemented");
}

/**
 * Run the analyzer against currently-stored events and persist any resulting incidents.
 * @returns The newly created incidents (not the full incident list).
 * @throws AnalyzerError if the analyzer process fails (propagated from analyzerService).
 */
export async function analyzeAndPersist(): Promise<Incident[]> {
  // TODO(student): load events via eventService.listEvents(), call
  // analyzerService.runAnalyzer(events), map each AnalyzerIncident to an IncidentModel
  // document (default status "Open"), persist, and return the created incidents.
  throw new Error("incidentService.analyzeAndPersist not implemented");
}

/**
 * Update an incident's status.
 * @throws NotFoundError if no incident with that id exists.
 * @throws ValidationError if `status` is not a recognized IncidentStatus.
 */
export async function updateIncidentStatus(id: string, status: IncidentStatus): Promise<Incident> {
  // TODO(student): update IncidentModel by id, returning the updated document. Throw
  // NotFoundError if nothing matched.
  void id;
  void status;
  throw new Error("incidentService.updateIncidentStatus not implemented");
}

/**
 * Delete an incident.
 * @throws NotFoundError if no incident with that id exists.
 */
export async function deleteIncident(id: string): Promise<void> {
  // TODO(student): delete by id via IncidentModel, throw NotFoundError if nothing matched.
  void id;
  throw new Error("incidentService.deleteIncident not implemented");
}
