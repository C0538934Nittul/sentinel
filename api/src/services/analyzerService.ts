/**
 * Integration seam between the API and the C++ analyzer binary. Spawns the compiled
 * sentinel-analyzer executable, pipes event JSON to it over stdin, and parses its stdout.
 *
 * Component: api/src/services
 * Status: complete (infrastructure, not assessed logic -- the analysis itself happens inside
 *         the C++ binary, which is where the assessed logic lives).
 */

import { execFile, type ExecFileException } from "node:child_process";
import { env } from "../config/env.js";
import { AnalyzerError } from "../middleware/errorHandler.js";
import type { AnalyzerOutput } from "../types/incident.js";
import type { SecurityEvent } from "../types/securityEvent.js";

const ANALYZER_TIMEOUT_MS = 10_000;
const ANALYZER_MAX_BUFFER_BYTES = 10 * 1024 * 1024; // 10 MB of stdout/stderr

// Exit codes defined by analyzer/src/main.cpp -- keep this in sync with that file.
const ANALYZER_EXIT_BAD_ARGS = 2;
const ANALYZER_EXIT_FILE_ERROR = 3;
const ANALYZER_EXIT_PARSE_VALIDATION_ERROR = 4;

interface AnalyzerErrorPayload {
  error?: { code?: string; message?: string };
}

/**
 * Run the analyzer against a set of events and return its parsed output.
 *
 * ## Why execFile, not exec or a shell string
 *
 * `child_process.exec` (and `spawn(..., { shell: true })`) run the command through `/bin/sh`,
 * which means any part of the command line built from a variable is subject to shell
 * interpolation -- a filename or argument containing characters like `; | & $( )` could inject
 * arbitrary shell commands. `execFile` invokes the binary directly via `execve`-family syscalls
 * with an argv array, with no shell in between: arguments are passed as discrete strings and
 * are never re-parsed or re-interpreted, so there is no injection surface even if a caller
 * somehow influenced an argument value. Here the arguments are all fixed/internally-controlled
 * (a config path and CLI flags), but the same discipline is what makes this pattern safe if the
 * argument set ever grows to include anything derived from user input. This is the specific
 * security control documented in docs/professional-practice.md.
 */
export function runAnalyzer(events: SecurityEvent[]): Promise<AnalyzerOutput> {
  return new Promise((resolve, reject) => {
    const child = execFile(
      env.analyzerBinPath,
      ["--stdin", "--config", env.rulesConfigPath],
      { timeout: ANALYZER_TIMEOUT_MS, maxBuffer: ANALYZER_MAX_BUFFER_BYTES },
      (error, stdout, stderr) => {
        if (error) {
          reject(mapAnalyzerFailure(error, stderr));
          return;
        }

        // TODO(student): decide whether/how to validate `stdout` against AnalyzerOutput's
        // shape (e.g. with a Zod schema) before trusting it, rather than a bare JSON.parse +
        // type assertion. Left as a stub since the analyzer's actual output schema depends on
        // IncidentResult::toJson(), which is not implemented yet.
        try {
          const parsed = JSON.parse(stdout) as AnalyzerOutput;
          resolve(parsed);
        } catch {
          reject(new AnalyzerError("Analyzer produced invalid JSON output", { stdout, stderr }));
        }
      },
    );

    child.stdin?.write(JSON.stringify({ events }));
    child.stdin?.end();
  });
}

function mapAnalyzerFailure(error: ExecFileException, stderr: string): AnalyzerError {
  const parsedStderr = tryParseStderr(stderr);
  const message = parsedStderr?.error?.message ?? error.message;
  const exitCode = typeof error.code === "number" ? error.code : undefined;

  switch (exitCode) {
    case ANALYZER_EXIT_BAD_ARGS:
      return new AnalyzerError(`Analyzer rejected its own arguments: ${message}`, { exitCode });
    case ANALYZER_EXIT_FILE_ERROR:
      return new AnalyzerError(`Analyzer file error: ${message}`, { exitCode });
    case ANALYZER_EXIT_PARSE_VALIDATION_ERROR:
      return new AnalyzerError(`Analyzer input validation failed: ${message}`, { exitCode });
    default:
      if (error.signal) {
        return new AnalyzerError(`Analyzer terminated by signal ${error.signal} (possible timeout)`, {
          signal: error.signal,
        });
      }
      return new AnalyzerError(`Analyzer failed unexpectedly: ${message}`, { exitCode, stderr });
  }
}

function tryParseStderr(stderr: string): AnalyzerErrorPayload | undefined {
  try {
    return JSON.parse(stderr) as AnalyzerErrorPayload;
  } catch {
    return undefined;
  }
}
