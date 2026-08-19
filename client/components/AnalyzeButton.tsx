/**
 * Button that triggers POST /v1/incidents/analyze and surfaces the result or error.
 * Component: client/components
 * Status: button/loading/error UI implemented. The onAnalyzed callback wiring (how the parent
 *         page refreshes its incident list afterward) is left as a TODO(student) -- see the
 *         call site in app/incidents/page.tsx.
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
      // TODO(student): decide how the incidents list page should refresh after this --
      // re-fetch from the server, merge `incidents` into existing state, or navigate.
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
