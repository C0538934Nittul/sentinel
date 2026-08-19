/**
 * Renders an API error envelope verbatim -- the one component every data-fetching page relies
 * on so failures are always visible rather than silently swallowed (see lib/api.ts's
 * ApiError).
 * Component: client/components
 * Status: complete.
 */

import type { ApiError } from "@/lib/api";

export function ErrorBanner({ error }: { error: ApiError | Error }) {
  const code = "code" in error ? error.code : "UNKNOWN_ERROR";
  return (
    <div role="alert" className="rounded-md border border-red-800 bg-red-950/50 p-4 text-sm text-red-200">
      <p className="font-mono text-xs uppercase tracking-wide text-red-400">{code}</p>
      <p className="mt-1">{error.message}</p>
    </div>
  );
}
