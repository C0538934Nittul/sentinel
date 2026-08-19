/**
 * Renders a single incident's full detail with loading/empty/error states. Presentational only
 * -- the owning page (app/incidents/[id]/page.tsx) fetches and passes state down.
 * Component: client/components
 * Status: layout/loading/empty/error states implemented.
 */

import type { ApiError } from "@/lib/api";
import type { Incident } from "@/lib/types";
import { ErrorBanner } from "./ErrorBanner";
import { StatusBadge } from "./StatusBadge";

interface IncidentDetailProps {
  incident: Incident | null;
  loading: boolean;
  error: ApiError | Error | null;
}

export function IncidentDetail({ incident, loading, error }: IncidentDetailProps) {
  if (error) {
    return <ErrorBanner error={error} />;
  }

  if (loading) {
    return <p className="text-sm text-slate-400">Loading incident...</p>;
  }

  if (!incident) {
    return <p className="text-sm text-slate-400">Incident not found.</p>;
  }

  return (
    <div className="space-y-4">
      <div className="flex items-center justify-between">
        <h1 className="text-xl font-semibold">{incident.ruleId}</h1>
        <StatusBadge status={incident.status} />
      </div>
      <p className="text-sm text-slate-300">{incident.summary}</p>
      <dl className="grid grid-cols-2 gap-4 text-sm">
        <div>
          <dt className="text-slate-400">Severity</dt>
          <dd>{incident.severity}</dd>
        </div>
        <div>
          <dt className="text-slate-400">Risk score</dt>
          <dd>{incident.riskScore}</dd>
        </div>
        <div>
          <dt className="text-slate-400">Detected at</dt>
          <dd className="font-mono text-xs">{incident.detectedAt}</dd>
        </div>
        <div>
          <dt className="text-slate-400">Supporting events</dt>
          <dd>{incident.supportingEventIds.length}</dd>
        </div>
      </dl>
      <div>
        <h2 className="text-sm font-medium text-slate-300">Supporting event IDs</h2>
        <ul className="mt-1 space-y-1 font-mono text-xs text-slate-400">
          {incident.supportingEventIds.map((eventId) => (
            <li key={eventId}>{eventId}</li>
          ))}
        </ul>
      </div>
    </div>
  );
}
