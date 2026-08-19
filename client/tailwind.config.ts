/**
 * Tailwind configuration. Component: client. Status: complete (default content globs --
 * visual polish is out of scope for this PLA, see docs/professional-practice.md).
 */
import type { Config } from "tailwindcss";

const config: Config = {
  content: ["./app/**/*.{ts,tsx}", "./components/**/*.{ts,tsx}"],
  theme: {
    extend: {},
  },
  plugins: [],
};

export default config;
