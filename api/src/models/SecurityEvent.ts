/**
 * Mongoose model for persisted security events. Schema mirrors docs/data-contract.md exactly --
 * if you change a field there, change it here too.
 *
 * Component: api/src/models
 * Status: schema fully specified (validation, indexes, toJSON transform). There is no service
 *         logic in this file to stub -- see src/services/eventService.ts for the stubbed
 *         business logic that uses this model.
 */

import { Schema, model, type InferSchemaType } from "mongoose";
import { EVENT_TYPES } from "../types/securityEvent.js";

const securityEventSchema = new Schema(
  {
    eventId: { type: String, required: true, unique: true, trim: true },
    timestamp: { type: Date, required: true },
    eventType: { type: String, required: true, enum: EVENT_TYPES },
    sourceIp: {
      type: String,
      required: true,
      // Basic IPv4 dotted-quad format check -- see docs/ASSUMPTIONS.md for why format-only.
      match: /^(\d{1,3}\.){3}\d{1,3}$/,
    },
    account: { type: String, required: false, trim: true },
    host: { type: String, required: true, trim: true },
    details: { type: Schema.Types.Mixed, required: false, default: {} },
  },
  {
    timestamps: false,
    toJSON: {
      virtuals: true,
      transform: (_doc, ret: Record<string, unknown>) => {
        ret.id = ret._id;
        delete ret._id;
        delete ret.__v;
        return ret;
      },
    },
  },
);

// eventId must be globally unique -- this is the primary correlation key referenced by
// IncidentResult.supportingEventIds, so a duplicate would silently corrupt that linkage.
securityEventSchema.index({ eventId: 1 }, { unique: true });

// Compound index supporting the analyzer/API's most common query shape: "events from this
// source, most recent first" -- used both when assembling analysis windows and when paging
// the events list in the client.
securityEventSchema.index({ sourceIp: 1, timestamp: -1 });

export type SecurityEventDocument = InferSchemaType<typeof securityEventSchema>;

export const SecurityEventModel = model("SecurityEvent", securityEventSchema);
