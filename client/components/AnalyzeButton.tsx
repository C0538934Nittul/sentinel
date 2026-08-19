/**
 * Button that triggers POST /v1/incidents/analyze and surfaces the result or error.
 * Component: client/components
 * Status: complete. app/incidents/page.tsx passes its own re-fetch function as onAnalyzed, so
 *         a successful run re-queries the server rather than trusting the analyze response's
 *         (currently always-empty, since ThreatAnalyzer::analyze is unimplemented) incident list.
 */

"use client";

import { useState } from "react";
import { analyzeIncidents, type ApiError } from "@/lib/api";
import type { Incident } from "@/lib/types";
import { ErrorBanner } from "./ErrorBanner";

interface AnalyzeButtonProps {
  onAnalyzed?: (incidents: Incident[]) => void;
}

export function AnalyzeButton({ onAnalyzed }: AnalyzeButtonProps) {
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<ApiError | Error | null>(null);

  async function handleClick() {
    setLoading(true);
    setError(null);
    try {
      const incidents = await analyzeIncidents();
      onAnalyzed?.(incidents);
    } catch (err) {
      setError(err instanceof Error ? err : new Error("Unknown error"));
    } finally {
      setLoading(false);
    }
  }

  return (
    <div className="space-y-2">
      <button
        type="button"
        onClick={handleClick}
        disabled={loading}
        className="rounded-md bg-blue-600 px-4 py-2 text-sm font-medium text-white hover:bg-blue-500 disabled:opacity-50"
      >
        {loading ? "Analyzing..." : "Run analysis"}
      </button>
      {error && <ErrorBanner error={error} />}
    </div>
  );
}
