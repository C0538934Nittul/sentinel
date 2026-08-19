# Professional practice

Outline only -- prompting questions under each heading, not prose. Two sections below have a
hard minimum (marked): don't skip those.

## 1. Tools, libraries, and build tools

- List what you actually used (CMake, nlohmann/json, Catch2, Express, Mongoose, Zod, Next.js,
  Tailwind, ...) and, for each non-obvious choice, one sentence on why.
- What version-control workflow did you use (branches, commit granularity, `development` vs.
  `main`)?

## 2. Coding standards and organization

- What naming/formatting conventions did you follow in C++ vs. TypeScript, and are they
  consistent within each language?
- How does the repository layout (see root `README.md`'s "Repository layout" section) reflect a
  separation of concerns?
- Did you use a linter/formatter? What did it catch, if anything?

## 3. Testing approach and workflow

- What's actually covered by the Catch2 test checklist in `analyzer/tests/` once you've filled
  the bodies in -- and what, honestly, isn't covered?
- Did you do any manual testing beyond automated tests (e.g. `api/tests/requests.http`,
  running the analyzer directly against `sample-data/*.json`)?
- What would your testing strategy be if this were a production system instead of coursework?

## 4. Licensing and IP compliance

- What license (if any) applies to this repository, and why?
- List every third-party dependency with a non-permissive or unusual license, if any (a quick
  pass through `package.json`/CMake `FetchContent` targets is enough -- nlohmann/json, Catch2,
  Express, Mongoose, Zod, Next.js are all permissively licensed, but confirm rather than assume).

## 5. Security, privacy, data integrity, and responsible use

**Minimum: at least two distinct ethical/legal/social implications of processing
security-event data, even though this project only uses synthetic data.**

- What would change about this system's design if it ingested *real* security logs instead of
  synthetic fixtures (PII in `account`/`host` fields, retention limits, access control, breach
  notification obligations)?
- Two implications to get you started (expand on these or replace with your own, but don't
  leave fewer than two):
  1. Security-event data often contains personally identifiable information (usernames,
     hostnames, IP addresses) even when it's "just log data" -- what obligations would that
     create under data-protection frameworks you're aware of?
  2. A SOC tool that produces false positives/negatives has real consequences (alert fatigue,
     missed incidents) -- what's the ethical weight of a detection rule's threshold choice?
- What's documented in the root `README.md`'s "Scope note" and why does it matter for a tool
  like this even in a coursework context?

## 6. AI tool usage disclosure

**Required: disclose what AI assistance was used, what kind, and how you verified/adapted it.**

- Which parts of this repository were scaffolded with AI assistance (per the project's own
  README and `docs/ASSUMPTIONS.md`, that's the plumbing/infrastructure -- CMake config, Express
  middleware, the C++ class *declarations*, sample data, documentation scaffolding) versus
  written by you unassisted (the assessed logic: detection rule bodies, correlation, scoring,
  controller bodies)?
- What did you personally verify, change, or reject from the AI-assisted portions, and how?
- Be specific enough that an evaluator could tell the difference between "I used AI to generate
  this and submitted it unread" and "I used AI for scaffolding and did the assessed work
  myself" -- the second is what this section needs to demonstrate.
