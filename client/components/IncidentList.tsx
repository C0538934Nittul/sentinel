/**
 * Renders a list of incidents with loading/empty/error states. Presentational only -- the
 * owning page (app/incidents/page.tsx) fetches and passes state down.
 * Component: client/components
 * Status: layout/loading/empty/error states implemented.
 */

import type { ApiError } from "@/lib/api";
import type { Incident } from "@/lib/types";
import { ErrorBanner } from "./ErrorBanner";
import { StatusBadge } from "./StatusBadge";

interface IncidentListProps {
  incidents: Incident[] | null;
  loading: boolean;
  error: ApiError | Error | null;
}

export function IncidentList({ incidents, loading, error }: IncidentListProps) {
  if (error) {
    return <ErrorBanner error={error} />;
  }

  if (loading) {
    return <p className="text-sm text-slate-400">Loading incidents...</p>;
  }

  if (!incidents || incidents.length === 0) {
    return <p className="text-sm text-slate-400">No incidents yet. Run an analysis to generate some.</p>;
  }

  return (
    <ul className="divide-y divide-slate-900">
      {incidents.map((incident) => (
        <li key={incident.id} className="flex items-center justify-between py-3">
          <div>
            <a href={`/incidents/${incident.id}`} className="font-medium hover:underline">
              {incident.ruleId}
            </a>
            <p className="text-xs text-slate-400">{incident.summary}</p>
          </div>
          <div className="flex items-center gap-3">
            <span className="text-xs text-slate-400">score {incident.riskScore}</span>
            <StatusBadge status={incident.status} />
          </div>
        </li>
      ))}
    </ul>
  );
}
