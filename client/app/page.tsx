/**
 * Dashboard shell: landing page with a summary and links into Events/Incidents. No data
 * fetching of its own yet.
 * Component: client/app
 * Status: layout implemented. TODO(student): replace the placeholder counts with real
 *         summary data (e.g. open incident count, total events) once the API is wired up.
 */

export default function DashboardPage() {
  return (
    <div className="space-y-6">
      <h1 className="text-xl font-semibold">Dashboard</h1>
      <p className="text-sm text-slate-400">
        Sentinel ingests security events, runs them through the C++ analysis engine, and
        surfaces incidents here. See <a className="underline" href="/events">Events</a> to
        submit or browse raw events, and <a className="underline" href="/incidents">Incidents</a>
        {" "}to review and triage what the analyzer has found.
      </p>
      <div className="grid grid-cols-2 gap-4">
        <div className="rounded-md border border-slate-800 p-4">
          <p className="text-xs uppercase tracking-wide text-slate-400">Open incidents</p>
          {/* TODO(student): fetch and display a real count. */}
          <p className="mt-1 text-2xl font-semibold">—</p>
        </div>
        <div className="rounded-md border border-slate-800 p-4">
          <p className="text-xs uppercase tracking-wide text-slate-400">Total events</p>
          {/* TODO(student): fetch and display a real count. */}
          <p className="mt-1 text-2xl font-semibold">—</p>
        </div>
      </div>
    </div>
  );
}
