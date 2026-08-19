/**
 * Typed application error hierarchy and the Express error-handling middleware that maps each
 * error type to an HTTP status and the shared { error: { code, message, details? } } envelope
 * (see docs/data-contract.md).
 *
 * Component: api/src/middleware
 * Status: complete (infrastructure, not assessed logic).
 */

import type { NextFunction, Request, Response } from "express";
import type { ApiErrorBody } from "../types/apiError.js";

/** Base class for all errors this API intentionally raises and knows how to render. */
export abstract class AppError extends Error {
  abstract readonly statusCode: number;
  abstract readonly code: string;
  readonly details?: unknown;

  protected constructor(message: string, details?: unknown) {
    super(message);
    this.name = new.target.name;
    this.details = details;
  }
}

/** 400 -- request body/query/params failed schema validation. */
export class ValidationError extends AppError {
  readonly statusCode = 400;
  readonly code = "VALIDATION_ERROR";
  constructor(message: string, details?: unknown) {
    super(message, details);
  }
}

/** 404 -- requested resource does not exist. */
export class NotFoundError extends AppError {
  readonly statusCode = 404;
  readonly code = "NOT_FOUND";
  constructor(message: string, details?: unknown) {
    super(message, details);
  }
}

/** 409 -- request conflicts with existing state (e.g. duplicate eventId). */
export class ConflictError extends AppError {
  readonly statusCode = 409;
  readonly code = "CONFLICT";
  constructor(message: string, details?: unknown) {
    super(message, details);
  }
}

/** 422 -- request was well-formed but semantically unprocessable. */
export class UnprocessableError extends AppError {
  readonly statusCode = 422;
  readonly code = "UNPROCESSABLE_ENTITY";
  constructor(message: string, details?: unknown) {
    super(message, details);
  }
}

/** 500 -- the analyzer process failed, timed out, or returned something unparseable. */
export class AnalyzerError extends AppError {
  readonly statusCode = 500;
  readonly code = "ANALYZER_ERROR";
  constructor(message: string, details?: unknown) {
    super(message, details);
  }
}

/** 500 -- catch-all for anything not already an AppError. */
export class InternalError extends AppError {
  readonly statusCode = 500;
  readonly code = "INTERNAL_ERROR";
  constructor(message = "Internal server error", details?: unknown) {
    super(message, details);
  }
}

/** Express 404 handler for routes that don't match any registered route. */
export function notFoundHandler(req: Request, res: Response): void {
  const body: ApiErrorBody = {
    error: { code: "NOT_FOUND", message: `No route for ${req.method} ${req.path}` },
  };
  res.status(404).json(body);
}

/**
 * Express error-handling middleware (must be registered last, after all routes). Maps
 * AppError subclasses to their declared status/code, and anything else to a generic 500 --
 * unrecognized errors never leak stack traces or internal details to the client.
 */
// eslint-disable-next-line @typescript-eslint/no-unused-vars
export function errorHandler(err: unknown, req: Request, res: Response, next: NextFunction): void {
  if (err instanceof AppError) {
    const body: ApiErrorBody = {
      error: { code: err.code, message: err.message, ...(err.details !== undefined ? { details: err.details } : {}) },
    };
    res.status(err.statusCode).json(body);
    return;
  }

  req.log?.error({ err }, "unhandled error");
  const body: ApiErrorBody = {
    error: { code: "INTERNAL_ERROR", message: "Internal server error" },
  };
  res.status(500).json(body);
}
