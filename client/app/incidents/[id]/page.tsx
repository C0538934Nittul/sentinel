/**
 * Single incident detail page.
 * Component: client/app/incidents/[id]
 * Status: First-draft implementation (Step 3, Phase 5) -- fetches on mount / when the id changes.
 */

"use client";

import { useEffect, useState } from "react";
import { useParams } from "next/navigation";
import { getIncident, type ApiError } from "@/lib/api";
import type { Incident } from "@/lib/types";
import { IncidentDetail } from "@/components/IncidentDetail";

export default function IncidentDetailPage() {
  const params = useParams<{ id: string }>();
  const [incident, setIncident] = useState<Incident | null>(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<ApiError | Error | null>(null);

  useEffect(() => {
    let cancelled = false;
    setLoading(true);
    setError(null);
    getIncident(params.id)
      .then((result) => {
        if (!cancelled) setIncident(result);
      })
      .catch((err) => {
        if (!cancelled) setError(err instanceof Error ? err : new Error("Unknown error"));
      })
      .finally(() => {
        if (!cancelled) setLoading(false);
      });
    return () => {
      cancelled = true;
    };
  }, [params.id]);

  return <IncidentDetail incident={incident} loading={loading} error={error} />;
}
