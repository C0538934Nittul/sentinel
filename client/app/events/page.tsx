/**
 * Events page: lists submitted security events and offers a form to submit one manually.
 * Component: client/app/events
 * Status: page shell and state wiring implemented; the actual fetch-on-mount call is a
 *         TODO(student) -- EventTable will show its loading state indefinitely until this is
 *         filled in.
 */

"use client";

import { useEffect, useState } from "react";
import type { ApiError } from "@/lib/api";
import type { SecurityEvent } from "@/lib/types";
import { EventTable } from "@/components/EventTable";
import { SubmitEventForm } from "@/components/SubmitEventForm";

export default function EventsPage() {
  const [events, setEvents] = useState<SecurityEvent[] | null>(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<ApiError | Error | null>(null);

  useEffect(() => {
    // TODO(student): call listEvents() from lib/api, setEvents/setError accordingly, then
    // setLoading(false). Consider re-fetching after SubmitEventForm succeeds (see its
    // TODO(student) note) so a newly submitted event shows up without a manual refresh.
  }, []);

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
          <SubmitEventForm />
        </div>
      </section>
    </div>
  );
}
