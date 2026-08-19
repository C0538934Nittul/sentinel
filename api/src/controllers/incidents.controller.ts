/**
 * Handlers for /v1/incidents. Zod request schemas are written in full and now wired up to
 * validate incoming requests (Phase 3 -- boilerplate request parsing, not assessed logic). The
 * subsequent calls into src/services/incidentService.ts are still stubbed, so valid requests
 * currently surface a 500 ("not implemented"). Full CRUD wiring against MongoDB is Phase 5.
 * Component: api/src/controllers
 */

import type { NextFunction, Request, Response } from "express";
import { z } from "zod";
import * as incidentService from "../services/incidentService.js";
import { ValidationError } from "../middleware/errorHandler.js";
import { INCIDENT_STATUSES } from "../types/incident.js";

export const listIncidentsQuerySchema = z.object({
  status: z.enum(INCIDENT_STATUSES).optional(),
  limit: z.coerce.number().int().positive().max(1000).optional(),
});

export const incidentIdParamsSchema = z.object({
  id: z.string().min(1),
});

export const updateIncidentStatusBodySchema = z.object({
  status: z.enum(INCIDENT_STATUSES),
});

/**
 * GET /v1/incidents
 * Input: query params validated by listIncidentsQuerySchema.
 * Output: 200 Incident[]
 * Error cases: 400 on invalid query params.
 */
export async function listIncidents(req: Request, res: Response, next: NextFunction): Promise<void> {
  const parsed = listIncidentsQuerySchema.safeParse(req.query);
  if (!parsed.success) {
    next(new ValidationError("Invalid query parameters", parsed.error.flatten()));
    return;
  }

  try {
    // TODO(student): incidentService.listIncidents is still stubbed -- this call currently rejects.
    const incidents = await incidentService.listIncidents(parsed.data);
    res.status(200).json(incidents);
  } catch (err) {
    next(err);
  }
}

/**
 * GET /v1/incidents/:id
 * Input: params validated by incidentIdParamsSchema.
 * Output: 200 Incident
 * Error cases: 404 if no incident with that id exists.
 */
export async function getIncident(req: Request, res: Response, next: NextFunction): Promise<void> {
  const parsed = incidentIdParamsSchema.safeParse(req.params);
  if (!parsed.success) {
    next(new ValidationError("Invalid incident id", parsed.error.flatten()));
    return;
  }

  try {
    // TODO(student): incidentService.getIncidentById is still stubbed -- this call currently rejects.
    const incident = await incidentService.getIncidentById(parsed.data.id);
    res.status(200).json(incident);
  } catch (err) {
    next(err);
  }
}

/**
 * POST /v1/incidents/analyze
 * Input: none (operates on all currently-stored events).
 * Output: 201 Incident[] (newly created incidents from this run)
 * Error cases: 500 (ANALYZER_ERROR) if the analyzer process fails, times out, or returns
 * unparseable output.
 */
export async function analyzeIncidents(_req: Request, res: Response, next: NextFunction): Promise<void> {
  try {
    // TODO(student): incidentService.analyzeAndPersist is still stubbed -- this call currently rejects.
    const incidents = await incidentService.analyzeAndPersist();
    res.status(201).json(incidents);
  } catch (err) {
    next(err);
  }
}

/**
 * PATCH /v1/incidents/:id/status
 * Input: params validated by incidentIdParamsSchema, body validated by
 * updateIncidentStatusBodySchema.
 * Output: 200 Incident (updated)
 * Error cases: 400 on invalid status value, 404 if no incident with that id exists.
 */
export async function updateIncidentStatus(req: Request, res: Response, next: NextFunction): Promise<void> {
  const paramsParsed = incidentIdParamsSchema.safeParse(req.params);
  if (!paramsParsed.success) {
    next(new ValidationError("Invalid incident id", paramsParsed.error.flatten()));
    return;
  }
  const bodyParsed = updateIncidentStatusBodySchema.safeParse(req.body);
  if (!bodyParsed.success) {
    next(new ValidationError("Invalid status value", bodyParsed.error.flatten()));
    return;
  }

  try {
    // TODO(student): incidentService.updateIncidentStatus is still stubbed -- this call currently rejects.
    const incident = await incidentService.updateIncidentStatus(paramsParsed.data.id, bodyParsed.data.status);
    res.status(200).json(incident);
  } catch (err) {
    next(err);
  }
}

/**
 * DELETE /v1/incidents/:id
 * Input: params validated by incidentIdParamsSchema.
 * Output: 204 No Content
 * Error cases: 404 if no incident with that id exists.
 */
export async function deleteIncident(req: Request, res: Response, next: NextFunction): Promise<void> {
  const parsed = incidentIdParamsSchema.safeParse(req.params);
  if (!parsed.success) {
    next(new ValidationError("Invalid incident id", parsed.error.flatten()));
    return;
  }

  try {
    // TODO(student): incidentService.deleteIncident is still stubbed -- this call currently rejects.
    await incidentService.deleteIncident(parsed.data.id);
    res.status(204).send();
  } catch (err) {
    next(err);
  }
}
