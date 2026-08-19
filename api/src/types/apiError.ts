/**
 * Shared TypeScript type for the API error envelope in docs/data-contract.md.
 * Component: api/src/types
 * Status: complete.
 */

export interface ApiErrorBody {
  error: {
    code: string;
    message: string;
    details?: unknown;
  };
}
