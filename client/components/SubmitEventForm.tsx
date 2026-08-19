/**
 * Form for manually submitting a single security event via POST /v1/events.
 * Component: client/components
 * Status: First-draft implementation (Step 3, Phase 5) -- submits via lib/api.submitEvent,
 *         clears the form and calls the optional onSubmitted callback on success so the owning
 *         page can refresh its list. Validation is still just the HTML `required` attributes;
 *         server-side Zod validation is the real backstop (errors surface via ErrorBanner).
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

interface SubmitEventFormProps {
  /** Called after a successful submission, e.g. so the owning page can re-fetch its list. */
  onSubmitted?: () => void;
}

export function SubmitEventForm({ onSubmitted }: SubmitEventFormProps) {
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
      setForm(initialFormState);
      onSubmitted?.();
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
