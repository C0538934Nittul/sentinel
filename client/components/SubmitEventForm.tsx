/**
 * Form for manually submitting a single security event via POST /v1/events.
 * Component: client/components
 * Status: form UI and field state implemented. Submission calls the real lib/api.submitEvent,
 *         but form-level validation (beyond HTML `required`) and success handling (e.g.
 *         clearing the form, refreshing an event list) are left as TODO(student).
 */

"use client";

import { useState, type FormEvent } from "react";
import { submitEvent, type ApiError } from "@/lib/api";
import { EVENT_TYPES, type EventType, type SecurityEvent } from "@/lib/types";
import { ErrorBanner } from "./ErrorBanner";

const initialFormState = {
  eventId: "",
  eventType: EVENT_TYPES[0] as EventType,
  sourceIp: "",
  account: "",
  host: "",
};

export function SubmitEventForm() {
  const [form, setForm] = useState(initialFormState);
  const [submitting, setSubmitting] = useState(false);
  const [error, setError] = useState<ApiError | Error | null>(null);

  async function handleSubmit(e: FormEvent<HTMLFormElement>) {
    e.preventDefault();
    setSubmitting(true);
    setError(null);
    try {
      const event: SecurityEvent = {
        eventId: form.eventId,
        timestamp: new Date().toISOString(),
        eventType: form.eventType,
        sourceIp: form.sourceIp,
        host: form.host,
        ...(form.account ? { account: form.account } : {}),
      };
      await submitEvent(event);
      // TODO(student): clear the form and/or refresh whatever list is showing this event.
    } catch (err) {
      setError(err instanceof Error ? err : new Error("Unknown error"));
    } finally {
      setSubmitting(false);
    }
  }

  return (
    <form onSubmit={handleSubmit} className="space-y-3">
      <div className="grid grid-cols-2 gap-3">
        <label className="text-sm">
          Event ID
          <input
            required
            value={form.eventId}
            onChange={(e) => setForm({ ...form, eventId: e.target.value })}
            className="mt-1 w-full rounded-md border border-slate-700 bg-slate-900 px-2 py-1"
          />
        </label>
        <label className="text-sm">
          Event type
          <select
            value={form.eventType}
            onChange={(e) => setForm({ ...form, eventType: e.target.value as EventType })}
            className="mt-1 w-full rounded-md border border-slate-700 bg-slate-900 px-2 py-1"
          >
            {EVENT_TYPES.map((type) => (
              <option key={type} value={type}>
                {type}
              </option>
            ))}
          </select>
        </label>
        <label className="text-sm">
          Source IP
          <input
            required
            value={form.sourceIp}
            onChange={(e) => setForm({ ...form, sourceIp: e.target.value })}
            placeholder="203.0.113.42"
            className="mt-1 w-full rounded-md border border-slate-700 bg-slate-900 px-2 py-1"
          />
        </label>
        <label className="text-sm">
          Account (optional)
          <input
            value={form.account}
            onChange={(e) => setForm({ ...form, account: e.target.value })}
            className="mt-1 w-full rounded-md border border-slate-700 bg-slate-900 px-2 py-1"
          />
        </label>
        <label className="text-sm">
          Host
          <input
            required
            value={form.host}
            onChange={(e) => setForm({ ...form, host: e.target.value })}
            className="mt-1 w-full rounded-md border border-slate-700 bg-slate-900 px-2 py-1"
          />
        </label>
      </div>
      <button
        type="submit"
        disabled={submitting}
        className="rounded-md bg-blue-600 px-4 py-2 text-sm font-medium text-white hover:bg-blue-500 disabled:opacity-50"
      >
        {submitting ? "Submitting..." : "Submit event"}
      </button>
      {error && <ErrorBanner error={error} />}
    </form>
  );
}
