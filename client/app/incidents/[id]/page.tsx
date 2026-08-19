/**
 * Single incident detail page.
 * Component: client/app/incidents/[id]
 * Status: page shell and state wiring implemented; the fetch-on-mount call is TODO(student).
 */

"use client";

import { useEffect, useState } from "react";
import { useParams } from "next/navigation";
import type { ApiError } from "@/lib/api";
import type { Incident } from "@/lib/types";
import { IncidentDetail } from "@/components/IncidentDetail";

export default function IncidentDetailPage() {
  const params = useParams<{ id: string }>();
  const [incident, setIncident] = useState<Incident | null>(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<ApiError | Error | null>(null);

  useEffect(() => {
    // TODO(student): call getIncident(params.id) from lib/api, setIncident/setError
    // accordingly, then setLoading(false).
    void params.id;
  }, [params.id]);

  return <IncidentDetail incident={incident} loading={loading} error={error} />;
}
