/**
 * Root layout: shared HTML shell and top-level navigation.
 * Component: client/app
 * Status: complete (layout only, no data fetching here).
 */

import type { Metadata } from "next";
import type { ReactNode } from "react";
import "./globals.css";

export const metadata: Metadata = {
  title: "Sentinel",
  description: "Sentinel security operations dashboard (PLA coursework project).",
};

export default function RootLayout({ children }: { children: ReactNode }) {
  return (
    <html lang="en">
      <body className="min-h-screen bg-slate-950 text-slate-100">
        <header className="border-b border-slate-800 px-6 py-4">
          <nav className="mx-auto flex max-w-5xl items-center gap-6">
            <a href="/" className="text-lg font-semibold">
              Sentinel
            </a>
            <a href="/events" className="text-sm text-slate-300 hover:text-white">
              Events
            </a>
            <a href="/incidents" className="text-sm text-slate-300 hover:text-white">
              Incidents
            </a>
          </nav>
        </header>
        <main className="mx-auto max-w-5xl px-6 py-8">{children}</main>
      </body>
    </html>
  );
}
