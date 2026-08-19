# AI assistance log

Running log of every file authored or substantially modified with AI assistance during "Step 3
-- get the skeleton to a green build," one line per file: what was done and what still needs
your review/verification. This feeds `docs/professional-practice.md` §6 (AI tool usage
disclosure) -- keep it accurate rather than reconstructing it later.

Scope reminder: `RepeatedFailureRule::evaluate`, `SuccessAfterFailuresRule::evaluate`,
`MultiAccountProbeRule::evaluate`, and `ThreatAnalyzer::analyze` are explicitly **not**
AI-authored logic in this pass -- they remain `TODO(student)` throughout every phase below.

## Phase 2 -- C++ analyzer green build

- `analyzer/src/RepeatedFailureRule.cpp`, `SuccessAfterFailuresRule.cpp`,
  `MultiAccountProbeRule.cpp` -- added `(void)window_; (void)threshold_;` to silence
  `-Wunused-private-field` (fields are unread until `evaluate()` exists). No logic changed.
  **Verify:** confirm you're fine with this convention, or remove it once `evaluate()` reads
  the fields for real (the warning will naturally disappear then).
- `analyzer/tests/test_EventReader.cpp` -- reworded a doc comment to avoid an embedded `/*`
  token that triggered `-Wcomment`. No test behavior changed. **Verify:** nothing, purely
  cosmetic.
- `analyzer/src/EventReader.cpp` -- `readFromFile` now does a real file-open check (throws
  `FileError` on a missing path); `readFromStream` stubbed to return `{}` instead of throwing.
  **Verify:** this is temporary scaffolding, not the real parsing logic -- Phase 5 replaces
  `readFromStream`'s body with real parsing/validation.
- `analyzer/src/ConfigReader.cpp` -- same pattern: real file-open check, stub body returns a
  safe default `AnalysisConfig` (300s window, zero rules) instead of throwing. **Verify:** the
  300s default matches `config/rules.json`'s actual value, but is currently hardcoded rather
  than read from the file -- Phase 5 fixes this.
- `analyzer/src/RuleFactory.cpp` -- `buildRules` returns `{}` instead of throwing. **Verify:**
  correct by construction today (config always has zero rules from the `ConfigReader` stub);
  will need Phase 5's real implementation once `ConfigReader` actually parses rules.
- `analyzer/src/ThreatAnalyzer.cpp` -- `analyze()` returns `{}` instead of throwing. **This one
  matters most:** confirm you're comfortable that this is a placeholder return value, not an
  implementation. The correlation/aggregation logic is entirely absent -- `TODO(student)` is
  still the operative marker, the return-value change is purely about letting the CLI exit 0 on
  an empty pipeline rather than crash.
- `docs/ASSUMPTIONS.md` -- added a "Stub return values" section explaining the throw-vs-empty
  change above. **Verify:** read it; it explains why the analyzer currently produces zero
  incidents on any input, which is expected and not a bug.
- `.gitignore` -- added `*.log` after a stray `sentinel-analyzer.log` from a manual test run
  showed up as an untracked file at the repo root.

## Phase 3 -- API typechecks and starts

- `api/src/app.ts` -- switched `pino-http`'s import from default to named (`import {
  pinoHttp }`) to work around a NodeNext/CJS-interop typecheck error (TS2349) in its
  `.d.ts`. No behavior change.
- `api/src/services/analyzerService.ts` -- replaced a hand-rolled error type with Node's own
  `ExecFileException` (from `node:child_process`), fixing several typecheck errors caused by
  `exactOptionalPropertyTypes` narrowing the hand-rolled type incorrectly. No logic changed.
- `api/src/controllers/health.controller.ts` -- **implemented** (not left stubbed): reports
  `mongoose.connection.readyState` and whether the analyzer binary exists at
  `env.analyzerBinPath` via `fs.existsSync`. Treated as infra rather than assessed logic since
  Phase 3's own acceptance criteria required a working health check. **Verify:** the
  `mongo`/`analyzerBinaryFound` field names and the 200-with-degraded-body-instead-of-non-2xx
  design choice.
- `api/src/controllers/events.controller.ts`, `incidents.controller.ts` -- wired the
  already-written Zod schemas to actually validate `req.query`/`req.params`/`req.body` and
  `next(new ValidationError(...))` on failure; converted handlers to `async` with
  `try/await/catch(next)` around the (still-stubbed) service calls to avoid unhandled promise
  rejections. **The actual service calls remain `TODO(student)`/stubbed** -- only the
  request-validation seam is real. **Verify:** this is boilerplate parsing, not business logic,
  but confirm you're comfortable with where I judged that line to be.
- `api/src/types/securityEvent.ts`, `api/src/services/eventService.ts`,
  `api/src/services/incidentService.ts` -- widened optional field types from `field?: T` to
  `field?: T | undefined` to satisfy `exactOptionalPropertyTypes: true` against Zod's
  `.optional()` output. Pure type-level fix, no runtime behavior change.
- `api/.env` created from `.env.example` with the values that match the Phase 2 build output
  (`ANALYZER_BIN_PATH=analyzer/build/debug/sentinel-analyzer`). Not committed (gitignored).

## Phase 4 -- Client typechecks and renders

- `client/package.json` -- pinned `eslint` from `^9.9.0` down to `^8.57.0` to resolve a real
  `npm install` ERESOLVE conflict against `eslint-config-next@14.2.5`'s peer dependency range.
  Not a suppression -- `npm install --legacy-peer-deps`/`--force` was deliberately avoided; see
  `docs/ASSUMPTIONS.md`. No other client code changed -- typecheck, build, and all four routes
  (`/`, `/events`, `/incidents`, `/incidents/[id]`) worked on the first real attempt once
  dependencies installed.

## Phase 5 -- First-draft implementations (C++)

Everything below is a **first draft** per your own framing -- written to compile, pass its
tests, and be readable, not presented as the final assessed answer. `RepeatedFailureRule`,
`SuccessAfterFailuresRule`, `MultiAccountProbeRule` (`evaluate()`) and `ThreatAnalyzer::analyze`
were **not** touched -- still `TODO(student)`, still throwing/returning `{}` as left in Phase 2.

- `analyzer/src/SecurityEvent.cpp` -- implemented `fromJson`, `toString(EventType)`,
  `eventTypeFromString`. Timestamp parsing uses a strict regex
  (`^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}\.\d{3}Z$`) plus `std::chrono::year_month_day`/
  `sys_days` for the calendar math -- deliberately avoided libc++'s `<chrono>` stream
  parsing/formatting extensions, whose availability varies by platform. **Verify:** the
  regex-then-chrono approach, and whether you want looser timestamp acceptance (e.g. no
  trailing `Z`, non-UTC offsets) -- currently rejected.
- `analyzer/include/sentinel/SecurityEvent.hpp` -- added `operator<` (by timestamp, tie-broken
  by `eventId`) since it wasn't declared in Step 2 and the rule `evaluate()` methods will want
  chronologically-sorted input. **Verify:** whether you'd rather sort in `EventReader` instead
  of relying on caller-side sorting via this operator.
- `analyzer/src/EventReader.cpp` -- implemented `readFromFile`/`readFromStream` with
  **skip-and-log** semantics (bad events dropped + logged to stderr + summary count; file as a
  whole still succeeds). Full reasoning and the fail-fast alternative are in
  `docs/ASSUMPTIONS.md`. **Verify:** this is a real design decision, not a default -- read that
  section before you build on it.
- `analyzer/src/ConfigReader.cpp` -- implemented `readFromFile`: parses `analysisWindowSeconds`
  and each `rules[]` entry, validates required fields and severity strings, throws `ConfigError`
  with a message naming the offending rule id.
- `analyzer/src/RuleFactory.cpp` -- implemented `buildRules`: maps each enabled `RuleConfig` to
  its concrete rule class by `id` string, throws `ConfigError` for an unrecognized id.
- `analyzer/src/IncidentResult.cpp` -- implemented `toJson()` matching
  `docs/data-contract.md`'s incident-result shape exactly (field names, ISO 8601 millisecond
  timestamp format).
- `analyzer/src/Logger.cpp`, `include/sentinel/Logger.hpp` -- added stderr echoing alongside
  the existing file output (was file-only in Step 2).
- `analyzer/tests/test_SecurityEvent.cpp`, `test_IncidentResult.cpp`, `test_EventReader.cpp`,
  `test_ConfigReader.cpp`, `test_RuleFactory.cpp`, `test_Logger.cpp` -- filled in with real
  assertions against the above. Self-contained (temp files / in-memory JSON), not dependent on
  `sample-data/` being reachable from `ctest`'s working directory.
  `test_RepeatedFailureRule.cpp`, `test_SuccessAfterFailuresRule.cpp`,
  `test_MultiAccountProbeRule.cpp`, `test_ThreatAnalyzer.cpp` were **left empty** as instructed
  -- note `ThreatAnalyzer::addRule`/`ruleCount` do have real, testable (non-assessed) behavior
  from Step 2 that I chose not to test here, erring toward leaving the whole file alone since
  `ThreatAnalyzer::analyze` is explicitly on the "don't implement" list; say if you'd like those
  ownership-only tests added.

## Phase 5 -- First-draft implementations (API)

- `api/src/services/eventService.ts` -- implemented `listEvents` (Mongoose `.find()` with
  optional `sourceIp`/`eventType` filters, sorted newest-first, default limit 100) and
  `createEvents` (`insertMany`, catching Mongo's duplicate-key error code 11000 and re-throwing
  as `ConflictError` with the offending `eventId`s).
- `api/src/services/incidentService.ts` -- implemented all five functions. `analyzeAndPersist`
  loads events via `eventService.listEvents({ limit: 100_000 })` (see **Verify** note below),
  calls the existing `analyzerService.runAnalyzer`, and persists whatever incidents come back
  (currently always zero, since `ThreatAnalyzer::analyze` is still a stub -- this is expected).
  `getIncidentById`/`updateIncidentStatus`/`deleteIncident` validate the id is a well-formed
  Mongo ObjectId before querying (`isValidObjectId`), throwing `NotFoundError` either way rather
  than letting a malformed id reach Mongoose as a cast error.
- `api/src/controllers/events.controller.ts`, `incidents.controller.ts` -- removed the
  now-stale "still stubbed" `TODO(student)` comments left over from Phase 3, since the service
  calls they annotate now actually work. No other controller logic changed.
- `api/src/models/SecurityEvent.ts` -- **bug fix**: removed a duplicate index definition on
  `eventId` (both `unique: true` on the field and an explicit `.index({eventId:1},{unique:
  true})` call declared the same index; Mongoose warned about it at startup). Kept the
  field-level `unique: true`, removed the redundant explicit call.
- **Verify:** `analyzeAndPersist`'s `{ limit: 100_000 }` is a placeholder way to say "all
  events" without a dedicated "fetch everything" service function -- fine for coursework scale,
  but worth a real pagination/streaming approach if this were closer to production.
- **End-to-end verified live** (not just typechecked): POST'd all 12 events from
  `sample-data/authentication-attack.json`, confirmed GET returns them, confirmed a duplicate
  `eventId` POST returns 409, confirmed `POST /v1/incidents/analyze` actually spawns
  `sentinel-analyzer` (its log file was written with the full pipeline trace: config loaded, 12
  events loaded, 3 rules registered, 0 incidents -- correct given `analyze()` is stubbed), and
  confirmed GET-by-id/PATCH-status/DELETE all work correctly against a manually-inserted test
  incident, including their 404 paths. Test data cleared from MongoDB afterward.

## Phase 5 -- First-draft implementations (client)

- `client/app/page.tsx` -- fetches open-incident and total-event counts on mount via
  `listIncidents()`/`listEvents()`; fails silently to "—" per-tile rather than showing a
  full-page error (dashboard summary tiles, not the primary error-surfacing UI).
- `client/app/events/page.tsx` -- fetches events on mount via `listEvents()`; passes a
  `fetchEvents` callback into `SubmitEventForm` as `onSubmitted` so a successful manual
  submission refreshes the table.
- `client/app/incidents/page.tsx` -- fetches incidents on mount via `listIncidents()`; passes
  `fetchIncidents` into `AnalyzeButton` as `onAnalyzed` so a completed analysis run re-queries
  the server (rather than trusting the analyze response's incident list directly, which is
  currently always empty since `ThreatAnalyzer::analyze` is unimplemented).
- `client/app/incidents/[id]/page.tsx` -- fetches the single incident via `getIncident(id)` on
  mount / id change, with a `cancelled` flag to avoid a state update after unmount.
- `client/components/SubmitEventForm.tsx` -- added an `onSubmitted` prop; clears the form and
  calls it after a successful submission.
- `client/components/AnalyzeButton.tsx` -- removed a stale `TODO(student)` comment now that the
  refresh strategy is decided (delegate to the parent page's re-fetch, not local state merging).
- **Verify:** no browser automation tool was available in this session (the Chrome extension
  wasn't connected), so client-side React rendering/hydration was **not** visually confirmed.
  What *was* confirmed: clean `tsc --noEmit` and `next build`, and that the exact HTTP responses
  these components fetch (`GET /v1/events`, `GET /v1/incidents`, with an
  `Origin: http://localhost:3000` header matching what a real browser sends) return data in the
  shape `lib/types.ts` expects, with CORS headers present. Please load the app in an actual
  browser and confirm the dashboard/events/incidents pages populate as expected before relying
  on this being fully verified.
