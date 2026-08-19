/**
 * Mongoose model for persisted incidents. Schema mirrors the incident-result shape in
 * docs/data-contract.md, plus the API-owned `status` field.
 *
 * Component: api/src/models
 * Status: schema fully specified. No service logic to stub here -- see
 *         src/services/incidentService.ts.
 */

import { Schema, model, type InferSchemaType } from "mongoose";
import { INCIDENT_SEVERITIES, INCIDENT_STATUSES } from "../types/incident.js";

const incidentSchema = new Schema(
  {
    ruleId: { type: String, required: true, trim: true },
    severity: { type: String, required: true, enum: INCIDENT_SEVERITIES },
    riskScore: { type: Number, required: true, min: 0, max: 100 },
    detectedAt: { type: Date, required: true },
    supportingEventIds: { type: [String], required: true, default: [] },
    summary: { type: String, required: true, trim: true },
    status: { type: String, required: true, enum: INCIDENT_STATUSES, default: "Open" },
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

// Incident list/dashboard views filter by status ("show me Open incidents") far more often
// than they look up a single incident, so status gets its own index.
incidentSchema.index({ status: 1 });
incidentSchema.index({ detectedAt: -1 });

export type IncidentDocument = InferSchemaType<typeof incidentSchema>;

export const IncidentModel = model("Incident", incidentSchema);
