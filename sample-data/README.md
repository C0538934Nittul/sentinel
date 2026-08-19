# Sample data

Seven synthetic datasets for exercising (or deliberately not exercising) the three detection
rules. All IPs are from `203.0.113.0/24` (RFC 5737 TEST-NET-3, reserved for documentation).
All usernames, hostnames, and process names are fabricated. Every file is wrapped as
`{ "events": [...] }` per `docs/data-contract.md`.

Numbers below were confirmed by directly counting events per source/account within a sliding
window over each file (see verification method at the bottom).

## `normal-events.json`

**Triggers nothing.** 40 events: 26 spaced `AUTH_SUCCESS` logins (90s apart, rotating across 4
source IPs and 5 accounts), 5 isolated `AUTH_FAILURE` events (each on a different source IP,
minutes apart), and 9 non-auth events (`FILE_ACCESS`, `PROCESS_START`, `NETWORK_CONNECTION`).

- Max `AUTH_FAILURE` count from one source within any 120s window: **1** (threshold is 10).
- Max distinct accounts probed from one source within any 180s window: **1** (threshold is 5).

## `authentication-attack.json`

**Fires `REPEATED_AUTH_FAILURE`.** 12 `AUTH_FAILURE` events from source `203.0.113.50` against
account `jdoe`, spaced 8 seconds apart, spanning **88 seconds** total.

- Count within the rule's 120s window: **12** (threshold: ≥10). Crosses by 2.

## `success-after-failures.json`

**Fires `SUCCESS_AFTER_FAILURES`.** 6 `AUTH_FAILURE` events against account `asmith` from
source `203.0.113.60` (5 seconds apart, spanning 25s), followed by one `AUTH_SUCCESS` from the
same account/source **60 seconds** after the last failure.

- Failure count before the success: **6** (threshold: ≥5). Crosses by 1.
- Delta from last failure to success: **60s** (well inside the 300s / 5-minute analysis window).

## `multi-account-probe.json`

**Fires `MULTI_ACCOUNT_PROBE`.** 7 `AUTH_FAILURE` events from source `203.0.113.70`, each
against a distinct account (`acct01`–`acct07`), spaced 20 seconds apart, spanning **120
seconds** total.

- Distinct accounts targeted within the rule's 180s window: **7** (threshold: ≥5). Crosses by 2.
- Sanity check: only 7 total events from this source, so `REPEATED_AUTH_FAILURE`'s threshold of
  10 is never reached — this file exercises exactly one rule.

## `mixed-events.json`

**Fires two rules**, embedded in general noise. 69 events total:

- **38 baseline noise events** (`AUTH_SUCCESS` / `FILE_ACCESS`, rotating across 3 sources, 4
  accounts, 4 hosts, spaced 75s apart) — establishes an uneventful backdrop.
- **Pattern A — `REPEATED_AUTH_FAILURE`:** 13 `AUTH_FAILURE` events from source
  `203.0.113.80` against account `guest`, spaced 8s apart (96s span), starting partway through
  the file. Count in the 120s window: **13** (threshold: ≥10). Comfortably over the threshold
  and deliberately **not** an exact-boundary case — see `boundary-events.json` for that.
- **Pattern B — `MULTI_ACCOUNT_PROBE`:** 6 `AUTH_FAILURE` events from source `203.0.113.90`
  against 6 distinct accounts (`svc01`–`svc06`), spaced 25s apart (125s span), starting well
  after pattern A ends. Distinct accounts in the 180s window: **6** (threshold: ≥5).
- **12 trailing noise events** (`PROCESS_START` / `NETWORK_CONNECTION` / `AUTH_SUCCESS`) after
  both patterns, rounding the file out.

Patterns A and B are separated by roughly 1400 seconds so they cannot be mistaken for a single
combined cluster.

## `boundary-events.json`

**Exercises the exact edges of `REPEATED_AUTH_FAILURE`'s threshold (10) and window (120s)** —
three independent cases, each on its own source IP/account so they can't interact. Unlike every
other file here, this one is deliberately about *precision*, not comfortable margins.

| Case | Source | Count | Span | Max count in any 120s window | Result |
|---|---|---|---|---|---|
| 1 — exact count, within window | `203.0.113.120` | 10 | 108s | 10 | **Fires** (10 ≥ 10, span ≤ 120s) |
| 2 — one below threshold | `203.0.113.121` | 9 | 96s | 9 | **Does not fire** (9 < 10) |
| 3 — right count, window exceeded | `203.0.113.122` | 10 | 125s | 9 | **Does not fire** (no single 120s window contains all 10) |

Case 3 is the subtle one: the file has 10 `AUTH_FAILURE` events on that source, but they're
spread across 125 seconds, so the widest 120-second slice only ever catches 9 of them — the
window, not the count, is what excludes it. This was confirmed by actually computing the
sliding-window maximum over the timestamps, not by construction.

Cases 2 and 3 each isolate a single failing condition (count-only, window-only) against the
same threshold/window pair Case 1 satisfies, so a rule implementation that passes Case 1 but
also accidentally fires on Case 2 or Case 3 has a specific, diagnosable bug rather than a vague
"something's off."

## `malformed-events.json`

**Not used to fire a rule — exercises analyzer input validation instead.** 6 deliberately
broken entries, each isolating one failure mode:

| # | Defect |
|---|---|
| 1 | `timestamp` field missing entirely |
| 2 | `timestamp` present but unparseable (`"not-a-real-timestamp"`) |
| 3 | `eventType` is not one of the five known values (`"TELEPORT_DETECTED"`) |
| 4 | `sourceIp` is a JSON number instead of a string |
| 5 | Empty object (`{}`) — every required field missing |
| 6 | `host` is explicitly JSON `null` rather than a string |

The file itself is syntactically valid JSON (it will parse), so it should exercise
`ValidationError` paths in `SecurityEvent::fromJson` / `EventReader`, not `ParseError` — useful
for confirming per-event validation reports something more specific than "file didn't parse."

## Verification method

For each file, the max count of `AUTH_FAILURE` events from a single `sourceIp` within any
sliding window equal to the relevant rule's `windowSeconds` was computed directly from the
`timestamp` values (not assumed from how the file was generated), and likewise for the max
number of distinct `account` values targeted by one `sourceIp` within a window. The numbers
above are those computed results, not nominal targets.
