/**
 * Dashboard shell: landing page with a summary and links into Events/Incidents.
 * Component: client/app
 * Status: First-draft implementation (Step 3, Phase 5) -- fetches real counts on mount.
 */

"use client";

import { useEffect, useState } from "react";
import { listEvents, listIncidents } from "@/lib/api";

export default function DashboardPage() {
  const [openIncidentCount, setOpenIncidentCount] = useState<number | null>(null);
  const [totalEventCount, setTotalEventCount] = useState<number | null>(null);

  useEffect(() => {
    let cancelled = false;

    listIncidents()
      .then((incidents) => {
        if (!cancelled) {
          setOpenIncidentCount(incidents.filter((i) => i.status === "Open").length);
        }
      })
      .catch(() => {
        // Dashboard summary tiles fail silently to "--" rather than blocking the page with a
        // full-page ErrorBanner -- the Events/Incidents pages are the ones responsible for
        // surfacing API errors in detail.
      });

    listEvents()
      .then((events) => {
        if (!cancelled) setTotalEventCount(events.length);
      })
      .catch(() => {});

    return () => {
      cancelled = true;
    };
  }, []);

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
          <p className="mt-1 text-2xl font-semibold">{openIncidentCount ?? "—"}</p>
        </div>
        <div className="rounded-md border border-slate-800 p-4">
          <p className="text-xs uppercase tracking-wide text-slate-400">Total events</p>
          <p className="mt-1 text-2xl font-semibold">{totalEventCount ?? "—"}</p>
        </div>
      </div>
    </div>
  );
}
