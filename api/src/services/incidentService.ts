/**
 * Business logic for incidents: listing, lookup, status updates, deletion, and triggering
 * analysis runs.
 * Component: api/src/services
 * Status: First-draft implementation (Step 3, Phase 5) against MongoDB via Mongoose. Not the
 *         final assessed version -- review and rewrite as needed.
 */

import { isValidObjectId } from "mongoose";
import { IncidentModel } from "../models/Incident.js";
import { NotFoundError } from "../middleware/errorHandler.js";
import { runAnalyzer } from "./analyzerService.js";
import * as eventService from "./eventService.js";
import type { Incident, IncidentStatus } from "../types/incident.js";

export interface ListIncidentsFilter {
  status?: IncidentStatus | undefined;
  limit?: number | undefined;
}

/**
 * List persisted incidents, optionally filtered by status.
 */
export async function listIncidents(filter: ListIncidentsFilter = {}): Promise<Incident[]> {
  const query: Record<string, unknown> = {};
  if (filter.status) query.status = filter.status;

  const documents = await IncidentModel.find(query)
    .sort({ detectedAt: -1 })
    .limit(filter.limit ?? 100)
    .exec();

  return documents.map((doc) => doc.toJSON() as unknown as Incident);
}

/**
 * Fetch a single incident by id.
 * @throws NotFoundError if no incident with that id exists.
 */
export async function getIncidentById(id: string): Promise<Incident> {
  if (!isValidObjectId(id)) {
    throw new NotFoundError(`No incident with id ${id}`);
  }
  const document = await IncidentModel.findById(id).exec();
  if (!document) {
    throw new NotFoundError(`No incident with id ${id}`);
  }
  return document.toJSON() as unknown as Incident;
}

/**
 * Run the analyzer against currently-stored events and persist any resulting incidents.
 * @returns The newly created incidents (not the full incident list).
 * @throws AnalyzerError if the analyzer process fails (propagated from analyzerService).
 */
export async function analyzeAndPersist(): Promise<Incident[]> {
  // "All" events for analysis, not the default page-sized 100 the GET /v1/events endpoint
  // uses -- see docs/ASSUMPTIONS.md.
  const events = await eventService.listEvents({ limit: 100_000 });

  const analyzerOutput = await runAnalyzer(events);

  if (analyzerOutput.incidents.length === 0) {
    return [];
  }

  const created = await IncidentModel.insertMany(
    analyzerOutput.incidents.map((incident) => ({
      ruleId: incident.ruleId,
      severity: incident.severity,
      riskScore: incident.riskScore,
      detectedAt: incident.detectedAt,
      supportingEventIds: incident.supportingEventIds,
      summary: incident.summary,
      status: "Open" as const,
    })),
  );

  return created.map((doc) => doc.toJSON() as unknown as Incident);
}

/**
 * Update an incident's status.
 * @throws NotFoundError if no incident with that id exists.
 * @throws ValidationError if `status` is not a recognized IncidentStatus.
 */
export async function updateIncidentStatus(id: string, status: IncidentStatus): Promise<Incident> {
  if (!isValidObjectId(id)) {
    throw new NotFoundError(`No incident with id ${id}`);
  }
  const document = await IncidentModel.findByIdAndUpdate(id, { status }, { new: true }).exec();
  if (!document) {
    throw new NotFoundError(`No incident with id ${id}`);
  }
  return document.toJSON() as unknown as Incident;
}

/**
 * Delete an incident.
 * @throws NotFoundError if no incident with that id exists.
 */
export async function deleteIncident(id: string): Promise<void> {
  if (!isValidObjectId(id)) {
    throw new NotFoundError(`No incident with id ${id}`);
  }
  const document = await IncidentModel.findByIdAndDelete(id).exec();
  if (!document) {
    throw new NotFoundError(`No incident with id ${id}`);
  }
}
