/**
 * Typed fetch wrapper for the Sentinel API. Every call goes through `apiFetch`, which unwraps
 * the shared error envelope so a failed request always surfaces as a thrown ApiError the UI can
 * render (via ErrorBanner) instead of being silently swallowed or left as an unhandled
 * rejection.
 *
 * Component: client/lib
 * Status: complete (infrastructure, not assessed logic).
 */

import type { ApiErrorBody, Incident, IncidentStatus, SecurityEvent } from "./types";

const API_BASE_URL = process.env.NEXT_PUBLIC_API_BASE_URL ?? "http://localhost:3001";

/** Thrown by apiFetch for any non-2xx response. Always carries the server's error envelope. */
export class ApiError extends Error {
  readonly code: string;
  readonly details?: unknown;
  readonly status: number;

  constructor(status: number, body: ApiErrorBody) {
    super(body.error.message);
    this.name = "ApiError";
    this.status = status;
    this.code = body.error.code;
    this.details = body.error.details;
  }
}

/**
 * Perform a fetch against the API and return the parsed JSON body.
 *
 * On a non-2xx response, this always throws an ApiError constructed from the response body's
 * `{ error: { code, message, details? } }` envelope -- callers never need to separately check
 * `res.ok`. If the server returns a non-JSON error body (e.g. a proxy's HTML error page), that
 * case is re-thrown as a generic ApiError with code "UNKNOWN_ERROR" rather than surfacing a raw
 * JSON-parse exception, so the UI always has a `code`/`message` to render either way.
 */
export async function apiFetch<T>(path: string, init?: RequestInit): Promise<T> {
  const res = await fetch(`${API_BASE_URL}${path}`, {
    ...init,
    headers: {
      "Content-Type": "application/json",
      ...init?.headers,
    },
  });

  if (!res.ok) {
    let body: ApiErrorBody;
    try {
      body = (await res.json()) as ApiErrorBody;
    } catch {
      body = { error: { code: "UNKNOWN_ERROR", message: `Request failed with status ${res.status}` } };
    }
    throw new ApiError(res.status, body);
  }

  if (res.status === 204) {
    return undefined as T;
  }

  return (await res.json()) as T;
}

// --- Typed convenience wrappers ------------------------------------------------------------
// Data fetching/shaping beyond a direct pass-through stays a TODO(student) -- these are thin
// wrappers over apiFetch, not business logic.

export function listEvents(): Promise<SecurityEvent[]> {
  return apiFetch<SecurityEvent[]>("/v1/events");
}

export function submitEvent(event: SecurityEvent): Promise<SecurityEvent[]> {
  return apiFetch<SecurityEvent[]>("/v1/events", {
    method: "POST",
    body: JSON.stringify(event),
  });
}

export function listIncidents(): Promise<Incident[]> {
  return apiFetch<Incident[]>("/v1/incidents");
}

export function getIncident(id: string): Promise<Incident> {
  return apiFetch<Incident>(`/v1/incidents/${id}`);
}

export function analyzeIncidents(): Promise<Incident[]> {
  return apiFetch<Incident[]>("/v1/incidents/analyze", { method: "POST" });
}

export function updateIncidentStatus(id: string, status: IncidentStatus): Promise<Incident> {
  return apiFetch<Incident>(`/v1/incidents/${id}/status`, {
    method: "PATCH",
    body: JSON.stringify({ status }),
  });
}
