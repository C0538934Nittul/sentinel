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
