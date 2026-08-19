# Technical design

Outline only -- fill in each section yourself once the implementation is far enough along to
describe accurately. Prompting questions are there to focus the writing, not to be answered
one-by-one as a Q&A.

## 1. Architecture overview

- What are the three components, and what is each one responsible for (and explicitly *not*
  responsible for)?
- Why does the client never talk to MongoDB or the analyzer directly? What would break if it did?
- Where does a request go, end to end, from a browser click to a database write and back?

## 2. C++ class relationships and ownership model

- How do `ThreatAnalyzer`, `DetectionRule`, and the three concrete rule subclasses relate to
  each other? Draw or describe the class diagram.
- Walk through the ownership model documented in `ThreatAnalyzer.hpp`: who allocates a rule, who
  owns it, when is it destroyed, and why `unique_ptr` rather than a raw pointer or `shared_ptr`?
- Where else in the analyzer does ownership matter (e.g. `SecurityEvent`'s immutability, the
  `nlohmann::json` member)?

## 3. Event-to-incident data flow

- Trace a single event from ingestion (file or POST) through to appearing as part of an
  incident in the dashboard. What transforms at each boundary?
- How does the analysis window (`config/rules.json`'s `analysisWindowSeconds`) bound what
  `ThreatAnalyzer::analyze` actually sees?

## 4. Memory management and performance strategy

- Where does the analyzer allocate on the heap, and why (or why not, elsewhere)?
- What's the algorithmic complexity of each detection rule's `evaluate()` as you implemented
  it, and could it be improved?
- Are events copied, moved, or referenced as they flow through `EventReader` ->
  `ThreatAnalyzer` -> rules? Justify the choice.

## 5. File handling and validation strategy

- What's the validation boundary between "malformed JSON" (ParseError) and "valid JSON, invalid
  event" (ValidationError)? Why split it that way?
- How does `malformed-events.json` exercise this boundary? (See `sample-data/README.md`.)

## 6. REST / SOA design rationale and framework selection

- Why Express + TypeScript over alternatives you considered?
- Why is `/v1` versioned? What would a `/v2` migration look like?
- Why does `POST /v1/incidents/analyze` return only the *newly created* incidents rather than
  the full incident list?

## 7. OpenAPI summary

- Point to `docs/openapi.yaml` and summarize the endpoint surface at a glance -- this section
  should be a short pointer/summary, not a duplicate of the spec.

## 8. Database schema and API <-> C++ integration

- Why the two Mongoose indexes chosen in `SecurityEvent.ts` (`eventId` unique, compound
  `{ sourceIp, timestamp }`)? What query do they each serve?
- Walk through `analyzerService.runAnalyzer`: how does it hand off to the C++ process and get a
  result back? What happens on timeout, non-zero exit, or malformed output?

## 9. Local build and deployment architecture

- What has to be running, in what order, for the full system to work locally? (See
  `scripts/dev.sh` for the raw steps -- this section is the narrative version.)
- What's explicitly out of scope (e.g. containerization, cloud deployment) and why?
