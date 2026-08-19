# Assumptions and choices log

This file tracks every naming, library, port, or structural decision made while scaffolding this
project that you (the author) might want to change. Nothing here is load-bearing — it's all easy
to revise, but revising it means touching multiple files, so it's collected in one place.

## Data contract (docs/data-contract.md)

- `eventId` format assumed as free-form string (example uses `evt-00001`); no enforced pattern
  at the contract level. Validation strictness is left for the analyzer/API to decide.
- Timestamps are ISO 8601 UTC with millisecond precision (`Z` suffix), matching what
  `Date.prototype.toISOString()` produces in JS and what a `std::chrono::system_clock` ↔ string
  round-trip in C++ can target.
- `details` is an open/untyped object (`Record<string, unknown>` / `nlohmann::json` object) —
  event-type-specific fields live here rather than as top-level optional fields, to keep the
  core schema stable as new event types are added.
- Incident `detectedAt` is separate from any individual event's `timestamp` — it's when the
  *analysis* ran, not when the underlying activity occurred.
- Chose `supportingEventIds: string[]` (references by `eventId`) over embedding full event
  objects in the incident, to avoid duplicating event data between the `events` and `incidents`
  collections.

## Rules config (config/rules.json)

- Analysis window default: `300` seconds (5 minutes), matching the `SUCCESS_AFTER_FAILURES`
  window described in the brief. Per-rule windows (120s, 90s not used since spec says 90s only
  appears in sample data — actual rule thresholds are 120s/300s/180s) override the global default.
- Added an `enabled: true` flag per rule so rules can be toggled off without editing C++ or
  removing config entries.
- Field names: `windowSeconds`, `threshold`, `severity`, `score` — chosen to read naturally in
  both JSON and as C++ struct members.

## Stub return values (Step 3, Phase 2 -- getting to a green build)

Before this pass, every stubbed method threw `std::logic_error("... not implemented")`. That
made the binary fail loudly on any real path, but also meant `sentinel-analyzer --events
sample-data/normal-events.json --config config/rules.json` couldn't exit 0 the way a smoke test
needs it to, since the call chain (`ConfigReader` -> `RuleFactory` -> `ThreatAnalyzer::analyze`)
runs through several stubs even on an empty/trivial input.

Changed: `EventReader::readFromFile`/`ConfigReader::readFromFile` now do a **real** file-open
check (throw `FileError` on a missing path -- this is infra, not assessed logic) but then return
a safe empty/default value instead of parsing; `EventReader::readFromStream`,
`RuleFactory::buildRules`, and `ThreatAnalyzer::analyze` return an empty result (`{}`) instead of
throwing. `SecurityEvent::fromJson`, `IncidentResult::toJson`, and the three rules' `evaluate()`
bodies are untouched (still throw `std::logic_error`) since nothing in the current smoke-test
path calls them yet.

This means the *current* end-to-end run is a no-op that proves the pipeline is wired correctly,
not that it does anything -- zero events in, zero rules built, zero incidents out, exit 0. Once
`EventReader`/`ConfigReader` gain real parsing (Phase 5), they'll actually populate `events` and
`config.rules`, at which point `RuleFactory` and `ThreatAnalyzer::analyze` need to move off the
empty-stub return too (`RuleFactory` gets implemented in Phase 5; `ThreatAnalyzer::analyze`
stays a stub by design -- see the "Do not implement these four" list -- so it will keep
returning `{}` even once rules exist, until you implement it yourself).

## C++ analyzer

- Used `nlohmann::json` (header-only, FetchContent) rather than a hand-rolled parser — standard
  choice for this kind of coursework, widely documented, easy to defend on video.
- Used Catch2 v3 (FetchContent) for tests + CTest integration.
- `DetectionRule` is an abstract base class (not `std::function`) — see the Doxygen comment in
  `DetectionRule.hpp` for the reasoning; this is a deliberate OOP demonstration choice for the
  rubric, not a technical requirement.
- `ThreatAnalyzer` owns rules via `std::vector<std::unique_ptr<DetectionRule>>` — exclusive
  ownership model, documented in the header. Change this only if you have a specific reason to
  share rule instances across analyzers.
- Exit codes (`0/2/3/4/5`) are an assumption of mine, not given in the brief — chosen to give
  the API a clean way to distinguish "bad CLI usage" from "bad input file" from "unexpected
  crash." Adjust freely; just keep `analyzerService.ts`'s exit-code mapping in sync if you do.
- Log output goes to a file via `Logger`, not stdout — stdout is reserved for the JSON result so
  the API can pipe it directly.

## Sample data

- Time deltas and account/source counts are chosen to sit comfortably *above* each threshold
  (not exactly at the boundary) so the intent of each file is unambiguous when you're first
  wiring up the rules. You may want boundary-exact fixtures later for edge-case testing —
  consider adding `*-boundary.json` variants once the rules are implemented.
- All IPs are from `203.0.113.0/24` (RFC 5737 TEST-NET-3). All usernames/hostnames are fabricated
  (`jdoe`, `asmith`, `workstation-*`, etc.) — no relation to real people or real systems.

## API (api/)

- Port `3001` for the API, `3000` for the client — Next.js's own dev-server default is 3000, so
  the API took the next number up.
- MongoDB database name: `sentinel`, local URI `mongodb://localhost:27017/sentinel`.
- `ANALYZER_BIN_PATH` and `RULES_CONFIG_PATH` are resolved **relative to the repo root** at
  runtime (via a computed path in `env.ts`), not hardcoded — see the "no absolute paths" rule.
  Default assumed layout: `analyzer/build/sentinel-analyzer` for the binary once built, and
  `config/rules.json` for the rules file.
- Chose `execFile` (not `exec`/`spawn` with `shell: true`) specifically to avoid shell
  interpolation — documented inline in `analyzerService.ts` since it's a named security control.
- Chose Zod for request validation and env validation — one library for both, less to explain.
- Mongoose schema: unique index on `eventId`, compound index on `{ sourceIp: 1, timestamp: -1 }`
  for time-windowed source lookups, index on incident `status`. `toJSON` transform renames
  `_id` → `id` and strips `__v` so the API's wire format doesn't leak Mongoose internals.
- Incident status enum: `Open`, `Investigating`, `Closed` — no `Ignored`/`FalsePositive` state
  added; add one later if your rubric wants a "dismissed" workflow.

## Client (client/) -- Step 3, Phase 4 correction

- `eslint` was pinned to `^9.9.0` in the original skeleton but `eslint-config-next@14.2.5`
  only supports ESLint 7/8 as a peer dependency -- `npm install` failed with an ERESOLVE
  conflict. Fixed by pinning `eslint` to `^8.57.0` (matching what Next.js 14's own project
  template uses), not by installing with `--legacy-peer-deps`/`--force`. If you later upgrade
  to Next.js 15 (which supports ESLint 9), revisit this pin.

## Client (client/)

- `NEXT_PUBLIC_API_BASE_URL` read at build/runtime via `process.env`, default
  `http://localhost:3001`.
- App Router (not Pages Router), TailwindCSS for styling, no component library — kept dependency
  surface small since visual polish isn't assessed.
- `lib/api.ts`'s error-unwrapping helper assumes every non-2xx response follows the
  `{ error: { code, message, details? } }` envelope; if a route ever returns a raw Express error
  page instead of JSON, that helper's parsing will need a fallback (currently just a TODO note in
  the file, not implemented).

## Tooling

- `.vscode/launch.json`'s LLDB config assumes the analyzer binary lands at
  `analyzer/build/debug/sentinel-analyzer` — adjust the path if your CMake preset output
  directory differs once you actually build.
- `scripts/*.sh` use `#!/usr/bin/env bash` and are marked executable; they assume Homebrew paths
  (`/opt/homebrew` on Apple Silicon) when checking for installed tools, with a fallback check for
  `/usr/local` (Intel Homebrew).
