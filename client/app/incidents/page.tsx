/**
 * Incidents page: lists incidents and offers a button to trigger a new analysis run.
 * Component: client/app/incidents
 * Status: First-draft implementation (Step 3, Phase 5) -- fetches on mount and re-fetches
 *         after a successful analysis run.
 */

"use client";

import { useCallback, useEffect, useState } from "react";
import { listIncidents, type ApiError } from "@/lib/api";
import type { Incident } from "@/lib/types";
import { AnalyzeButton } from "@/components/AnalyzeButton";
import { IncidentList } from "@/components/IncidentList";

export default function IncidentsPage() {
  const [incidents, setIncidents] = useState<Incident[] | null>(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<ApiError | Error | null>(null);

  const fetchIncidents = useCallback(async () => {
    setLoading(true);
    setError(null);
    try {
      setIncidents(await listIncidents());
    } catch (err) {
      setError(err instanceof Error ? err : new Error("Unknown error"));
    } finally {
      setLoading(false);
    }
  }, []);

  useEffect(() => {
    fetchIncidents();
  }, [fetchIncidents]);

  return (
    <div className="space-y-6">
      <div className="flex items-center justify-between">
        <h1 className="text-xl font-semibold">Incidents</h1>
        <AnalyzeButton onAnalyzed={fetchIncidents} />
      </div>
      <IncidentList incidents={incidents} loading={loading} error={error} />
    </div>
  );
}
