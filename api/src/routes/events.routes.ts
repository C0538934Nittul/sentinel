/**
 * Route registration for /v1/events.
 * Component: api/src/routes
 * Status: complete (routing only; handlers are stubbed).
 */

import { Router } from "express";
import { createEvents, listEvents } from "../controllers/events.controller.js";

const router = Router();

router.get("/", listEvents);
router.post("/", createEvents);

export default router;
