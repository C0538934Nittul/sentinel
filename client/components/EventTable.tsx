/**
 * Renders a list of security events with loading/empty/error states. Presentational only --
 * the owning page (app/events/page.tsx) is responsible for fetching and passing state down.
 * Component: client/components
 * Status: layout/loading/empty/error states implemented. Row content assumes the shape from
 *         lib/types.ts; no data shaping happens here (that's the page's job, currently stubbed).
 */

import type { ApiError } from "@/lib/api";
import type { SecurityEvent } from "@/lib/types";
import { ErrorBanner } from "./ErrorBanner";

interface EventTableProps {
  events: SecurityEvent[] | null;
  loading: boolean;
  error: ApiError | Error | null;
}

export function EventTable({ events, loading, error }: EventTableProps) {
  if (error) {
    return <ErrorBanner error={error} />;
  }

  if (loading) {
    return <p className="text-sm text-slate-400">Loading events...</p>;
  }

  if (!events || events.length === 0) {
    return <p className="text-sm text-slate-400">No events yet.</p>;
  }

  return (
    <table className="w-full border-collapse text-sm">
      <thead>
        <tr className="border-b border-slate-800 text-left text-slate-400">
          <th className="py-2 pr-4">Timestamp</th>
          <th className="py-2 pr-4">Type</th>
          <th className="py-2 pr-4">Source IP</th>
          <th className="py-2 pr-4">Account</th>
          <th className="py-2 pr-4">Host</th>
        </tr>
      </thead>
      <tbody>
        {events.map((event) => (
          <tr key={event.eventId} className="border-b border-slate-900">
            <td className="py-2 pr-4 font-mono text-xs">{event.timestamp}</td>
            <td className="py-2 pr-4">{event.eventType}</td>
            <td className="py-2 pr-4 font-mono text-xs">{event.sourceIp}</td>
            <td className="py-2 pr-4">{event.account ?? "—"}</td>
            <td className="py-2 pr-4">{event.host}</td>
          </tr>
        ))}
      </tbody>
    </table>
  );
}
