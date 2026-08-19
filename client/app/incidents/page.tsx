/**
 * Incidents page: lists incidents and offers a button to trigger a new analysis run.
 * Component: client/app/incidents
 * Status: page shell and state wiring implemented; the fetch-on-mount call and the
 *         AnalyzeButton refresh callback are TODO(student).
 */

"use client";

import { useEffect, useState } from "react";
import type { ApiError } from "@/lib/api";
import type { Incident } from "@/lib/types";
import { AnalyzeButton } from "@/components/AnalyzeButton";
import { IncidentList } from "@/components/IncidentList";

export default function IncidentsPage() {
  const [incidents, setIncidents] = useState<Incident[] | null>(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<ApiError | Error | null>(null);

  useEffect(() => {
    // TODO(student): call listIncidents() from lib/api, setIncidents/setError accordingly,
    // then setLoading(false).
  }, []);

  function handleAnalyzed(newIncidents: Incident[]) {
    // TODO(student): merge newIncidents into `incidents`, or re-fetch the full list -- your
    // call. void keeps this stub from erroring on the unused parameter.
    void newIncidents;
  }

  return (
    <div className="space-y-6">
      <div className="flex items-center justify-between">
        <h1 className="text-xl font-semibold">Incidents</h1>
        <AnalyzeButton onAnalyzed={handleAnalyzed} />
      </div>
      <IncidentList incidents={incidents} loading={loading} error={error} />
    </div>
  );
}
