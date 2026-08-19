/**
 * Small colored badge for an incident's status.
 * Component: client/components
 * Status: complete.
 */

import type { IncidentStatus } from "@/lib/types";

const STATUS_STYLES: Record<IncidentStatus, string> = {
  Open: "bg-red-900/50 text-red-200 border-red-800",
  Investigating: "bg-amber-900/50 text-amber-200 border-amber-800",
  Closed: "bg-slate-800 text-slate-300 border-slate-700",
};

export function StatusBadge({ status }: { status: IncidentStatus }) {
  return (
    <span className={`rounded-full border px-2 py-0.5 text-xs font-medium ${STATUS_STYLES[status]}`}>
      {status}
    </span>
  );
}
