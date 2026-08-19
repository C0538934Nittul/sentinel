/**
 * Environment configuration, validated with Zod so the process fails fast (and loudly) on a
 * bad or missing .env rather than surfacing a confusing error deep in a request handler.
 *
 * Component: api/src/config
 * Status: complete (infrastructure, not assessed logic).
 */

import path from "node:path";
import { fileURLToPath } from "node:url";
import { z } from "zod";
import "dotenv/config";

// api/src/config/env.ts -> repo root is three levels up (api/src/config -> api/src -> api -> repo root).
const REPO_ROOT = path.resolve(path.dirname(fileURLToPath(import.meta.url)), "..", "..", "..");

const envSchema = z.object({
  PORT: z.coerce.number().int().positive().default(3001),
  MONGODB_URI: z.string().min(1, "MONGODB_URI is required"),
  CLIENT_ORIGIN: z.string().url(),
  ANALYZER_BIN_PATH: z.string().min(1, "ANALYZER_BIN_PATH is required"),
  RULES_CONFIG_PATH: z.string().min(1, "RULES_CONFIG_PATH is required"),
  LOG_LEVEL: z.enum(["debug", "info", "warn", "error"]).default("info"),
});

function loadEnv() {
  const parsed = envSchema.safeParse(process.env);
  if (!parsed.success) {
    // eslint-disable-next-line no-console
    console.error("Invalid environment configuration:", parsed.error.flatten().fieldErrors);
    process.exit(1);
  }
  return parsed.data;
}

const rawEnv = loadEnv();

/**
 * Validated environment configuration. `analyzerBinPath` and `rulesConfigPath` are resolved to
 * absolute paths relative to the repository root at startup -- never hardcode an absolute path
 * from a specific developer's machine anywhere in this codebase.
 */
export const env = {
  port: rawEnv.PORT,
  mongodbUri: rawEnv.MONGODB_URI,
  clientOrigin: rawEnv.CLIENT_ORIGIN,
  analyzerBinPath: path.resolve(REPO_ROOT, rawEnv.ANALYZER_BIN_PATH),
  rulesConfigPath: path.resolve(REPO_ROOT, rawEnv.RULES_CONFIG_PATH),
  logLevel: rawEnv.LOG_LEVEL,
  repoRoot: REPO_ROOT,
} as const;
