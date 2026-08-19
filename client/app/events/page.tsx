/**
 * Events page: lists submitted security events and offers a form to submit one manually.
 * Component: client/app/events
 * Status: First-draft implementation (Step 3, Phase 5) -- fetches on mount and re-fetches
 *         after a successful manual submission.
 */

"use client";

import { useCallback, useEffect, useState } from "react";
import { listEvents, type ApiError } from "@/lib/api";
import type { SecurityEvent } from "@/lib/types";
import { EventTable } from "@/components/EventTable";
import { SubmitEventForm } from "@/components/SubmitEventForm";

export default function EventsPage() {
  const [events, setEvents] = useState<SecurityEvent[] | null>(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<ApiError | Error | null>(null);

  const fetchEvents = useCallback(async () => {
    setLoading(true);
    setError(null);
    try {
      setEvents(await listEvents());
    } catch (err) {
      setError(err instanceof Error ? err : new Error("Unknown error"));
    } finally {
      setLoading(false);
    }
  }, []);

  useEffect(() => {
    fetchEvents();
  }, [fetchEvents]);

  return (
    <div className="space-y-8">
      <section>
        <h1 className="text-xl font-semibold">Events</h1>
        <div className="mt-4">
          <EventTable events={events} loading={loading} error={error} />
        </div>
      </section>
      <section>
        <h2 className="text-lg font-medium">Submit an event</h2>
        <div className="mt-4">
          <SubmitEventForm onSubmitted={fetchEvents} />
        </div>
      </section>
    </div>
  );
}
