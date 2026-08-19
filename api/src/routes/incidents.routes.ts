/**
 * Route registration for /v1/incidents.
 * Component: api/src/routes
 * Status: complete (routing only; handlers are stubbed).
 *
 * Note: the static "/analyze" route must be registered before the "/:id" route, otherwise
 * Express would match "analyze" as an :id parameter.
 */

import { Router } from "express";
import {
  analyzeIncidents,
  deleteIncident,
  getIncident,
  listIncidents,
  updateIncidentStatus,
} from "../controllers/incidents.controller.js";

const router = Router();

router.get("/", listIncidents);
router.post("/analyze", analyzeIncidents);
router.get("/:id", getIncident);
router.patch("/:id/status", updateIncidentStatus);
router.delete("/:id", deleteIncident);

export default router;
