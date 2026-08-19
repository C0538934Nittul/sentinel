/**
 * Route registration for /v1/health.
 * Component: api/src/routes
 * Status: complete (routing is not assessed logic; the handler it points to is stubbed).
 */

import { Router } from "express";
import { getHealth } from "../controllers/health.controller.js";

const router = Router();

router.get("/", getHealth);

export default router;
