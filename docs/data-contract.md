# Data contract

This document is the single source of truth for the JSON shapes shared across the C++ analyzer,
the Mongoose schemas, and the Next.js client. If you change a field name, update it here first,
then propagate to `analyzer/include/sentinel/SecurityEvent.hpp`, `api/src/models/*.ts`,
`api/src/types/`, and `client/lib/types.ts`.

See `docs/ASSUMPTIONS.md` for the reasoning behind specific choices below.

## Security event

A single observed security-relevant event, as produced by whatever ingests raw log data (out of
scope for this project — events are supplied as files or POSTed directly).

```json
{
  "eventId": "evt-00001",
  "timestamp": "2026-03-01T10:00:00.000Z",
  "eventType": "AUTH_FAILURE",
  "sourceIp": "203.0.113.42",
  "account": "jdoe",
  "host": "workstation-12",
  "details": { "method": "ssh" }
}
```

| Field | Type | Required | Notes |
|---|---|---|---|
| `eventId` | string | yes | Unique identifier. Unique index enforced at the persistence layer. |
| `timestamp` | string (ISO 8601, UTC, millisecond precision) | yes | Must parse to a valid instant. |
| `eventType` | enum (see below) | yes | |
| `sourceIp` | string (IPv4 dotted-quad) | yes | |
| `account` | string | no | Absent for events with no associated account (e.g. some `NETWORK_CONNECTION` events). |
| `host` | string | yes | Hostname of the machine that generated or observed the event. |
| `details` | object | no | Event-type-specific data. Open schema — see per-type conventions below. |

### Event types

| `eventType` | Meaning | Typical `details` fields |
|---|---|---|
| `AUTH_SUCCESS` | Successful authentication | `method` (`"ssh"`, `"web"`, `"rdp"`, ...) |
| `AUTH_FAILURE` | Failed authentication attempt | `method`, optionally `reason` |
| `FILE_ACCESS` | File read/write/delete | `path`, `operation` (`"read"`\|`"write"`\|`"delete"`) |
| `PROCESS_START` | New process launched | `processName`, `pid` |
| `NETWORK_CONNECTION` | Outbound/inbound network connection observed | `destinationIp`, `destinationPort`, `protocol` |

### File wrapper

Event files (used as analyzer input and sample data) wrap the array:

```json
{
  "events": [
    { "eventId": "evt-00001", "timestamp": "...", "eventType": "AUTH_FAILURE", "...": "..." }
  ]
}
```

## Incident result

Produced by the analyzer when a detection rule fires; persisted via the API.

```json
{
  "incidentId": "inc-00001",
  "ruleId": "REPEATED_AUTH_FAILURE",
  "severity": "HIGH",
  "riskScore": 80,
  "detectedAt": "2026-03-01T10:02:00.000Z",
  "supportingEventIds": ["evt-00010", "evt-00011", "evt-00012"],
  "summary": "10 failed auth attempts from 203.0.113.42 within 120s",
  "status": "Open"
}
```

| Field | Type | Notes |
|---|---|---|
| `incidentId` | string | Assigned at persistence time (Mongo `_id`, exposed as `id` on the wire — see below). Analyzer output does not need to invent this. |
| `ruleId` | string | One of the rule IDs in `config/rules.json`. |
| `severity` | enum: `LOW` \| `MEDIUM` \| `HIGH` | Copied from the rule's configured severity at detection time. |
| `riskScore` | number | Copied from the rule's configured score at detection time. |
| `detectedAt` | string (ISO 8601 UTC) | When the analysis run produced this incident — not an event timestamp. |
| `supportingEventIds` | string[] | `eventId`s of the events that caused the rule to fire. |
| `summary` | string | Human-readable one-line description. |
| `status` | enum: `Open` \| `Investigating` \| `Closed` | Defaults to `Open`. Managed by the API, not the analyzer. |

### Persisted vs. wire shape

Mongoose documents use `_id` internally; the API's `toJSON` transform renames it to `id` and
strips the `__v` version key, so every JSON response uses `id`, never `_id` or `__v`.

## API error envelope

Every non-2xx API response uses this shape:

```json
{
  "error": {
    "code": "VALIDATION_ERROR",
    "message": "eventId is required",
    "details": { "field": "eventId" }
  }
}
```

| Field | Type | Notes |
|---|---|---|
| `error.code` | string | Stable machine-readable identifier (e.g. `VALIDATION_ERROR`, `NOT_FOUND`, `CONFLICT`, `ANALYZER_ERROR`, `INTERNAL_ERROR`). |
| `error.message` | string | Human-readable, safe to display. |
| `error.details` | object | Optional. Extra structured context (validation field paths, analyzer stderr excerpt, etc.). |
