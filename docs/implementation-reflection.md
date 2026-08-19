# Implementation reflection

Outline only -- write this after the assessed logic (detection rules, correlation, scoring,
controller bodies) is implemented, since it's a reflection on decisions you actually made.

## 1. Design decisions and trade-offs

- Where did you deviate from the skeleton's assumptions (see `docs/ASSUMPTIONS.md`), and why?
- For each detection rule, what was the trickiest edge case to get right (e.g. events exactly
  at a window boundary, an account with no prior events, overlapping windows)?
- Did `ThreatAnalyzer::analyze` end up doing anything beyond "call every rule and concatenate,"
  such as deduplication or cross-rule correlation? Why or why not?

## 2. Optimization and data-structure considerations

- What data structure did you use to group events by source IP / account within a rule's
  `evaluate()`, and why (e.g. `std::unordered_map` vs. sorting + linear scan)?
- Is there a difference in approach between rules that only care about one source IP
  (`RepeatedFailureRule`) and ones that need to track distinct values
  (`MultiAccountProbeRule`)?
- If you profiled anything, what did you find? If you didn't, what would you expect to be the
  bottleneck at scale (many events, long analysis windows)?

## 3. Error-handling strategy and known limitations

- Walk through what happens when `EventReader` encounters one invalid event in an otherwise
  valid file -- does the whole file fail, or is the bad event skipped/reported? Why did you
  choose that?
- What inputs does the analyzer *not* handle gracefully yet? Be specific -- this is more
  credible than a vague "more testing would help."
- Same question for the API: what request shapes or failure modes are still unhandled?

## 4. What you'd redesign with more time

- If the assessed portion is genuinely finished, what's the first thing you'd refactor and why?
- Is there a feature the rubric didn't ask for that the system is obviously missing (e.g. rule
  hot-reloading, pagination, authentication)?
