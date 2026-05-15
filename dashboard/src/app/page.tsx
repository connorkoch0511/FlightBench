import Link from "next/link";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { VerdictBadge } from "@/components/VerdictBadge";
import type { TestMeta } from "@/lib/types";
import indexData from "@/data/index.json";

const tests = indexData as TestMeta[];

const scenarioIcon: Record<string, string> = {
  "TC-001": "✈",
  "TC-002": "↑",
  "TC-003": "↓",
  "TC-004": "↺",
  "TC-005": "↻",
  "TC-006": "↑↺",
  "TC-007": "✈",
};

export default function Home() {
  const passed  = tests.filter((t) => t.verdict === "PASS").length;
  const failed  = tests.filter((t) => t.verdict === "FAIL").length;

  return (
    <div className="min-h-screen bg-zinc-50">
      <header className="border-b bg-white px-6 py-4">
        <div className="mx-auto max-w-5xl">
          <div className="flex items-center justify-between">
            <div>
              <h1 className="text-xl font-semibold tracking-tight">FlightBench</h1>
              <p className="text-sm text-muted-foreground">
                Hardware-in-the-Loop Simulation Test Harness
              </p>
            </div>
            <a
              href="https://github.com/connorkoch0511/FlightBench"
              target="_blank"
              rel="noopener noreferrer"
              className="text-sm text-muted-foreground underline underline-offset-4 hover:text-foreground"
            >
              GitHub →
            </a>
          </div>
        </div>
      </header>

      <main className="mx-auto max-w-5xl px-6 py-8 space-y-8">
        {/* Summary bar */}
        <div className="grid grid-cols-3 gap-4">
          <Card>
            <CardContent className="pt-6">
              <p className="text-3xl font-bold">{tests.length}</p>
              <p className="text-sm text-muted-foreground">Total tests</p>
            </CardContent>
          </Card>
          <Card>
            <CardContent className="pt-6">
              <p className="text-3xl font-bold text-green-600">{passed}</p>
              <p className="text-sm text-muted-foreground">Passed</p>
            </CardContent>
          </Card>
          <Card>
            <CardContent className="pt-6">
              <p className="text-3xl font-bold text-red-600">{failed}</p>
              <p className="text-sm text-muted-foreground">Failed</p>
            </CardContent>
          </Card>
        </div>

        {/* About */}
        <section className="space-y-2">
          <h2 className="text-sm font-semibold uppercase tracking-wide text-muted-foreground">
            About
          </h2>
          <p className="text-sm leading-relaxed text-muted-foreground max-w-2xl">
            FlightBench is a C++17 simulation and verification framework modeled after
            aerospace System Integration Lab (SIL) and Hardware-in-the-Loop (HIL) test
            environments. Each test case sets initial flight conditions, runs a time-stepped
            physics engine across a defined maneuver, and evaluates per-step invariants and
            end-state criteria. Sensor models add realistic noise (Gaussian altitude error,
            pitot scale factor, compass drift). The charts below show ground-truth vs. noisy
            sensor readings at each timestep.
          </p>
        </section>

        {/* Test case grid */}
        <section className="space-y-3">
          <h2 className="text-sm font-semibold uppercase tracking-wide text-muted-foreground">
            Test Suite
          </h2>
          <div className="grid gap-3 sm:grid-cols-2 lg:grid-cols-3">
            {tests.map((t) => (
              <Link key={t.id} href={`/test/${t.id}`}>
                <Card className="h-full cursor-pointer transition-shadow hover:shadow-md">
                  <CardHeader className="pb-2">
                    <div className="flex items-start justify-between gap-2">
                      <div className="flex items-center gap-2">
                        <span className="text-lg">{scenarioIcon[t.id]}</span>
                        <span className="text-xs font-mono text-muted-foreground">
                          {t.id}
                        </span>
                      </div>
                      <VerdictBadge verdict={t.verdict} />
                    </div>
                    <CardTitle className="text-sm leading-snug">{t.name}</CardTitle>
                  </CardHeader>
                  <CardContent>
                    <p className="text-xs text-muted-foreground leading-relaxed">
                      {t.description}
                    </p>
                    <p className="mt-2 text-xs text-muted-foreground">
                      {t.criteria.length} criteria · {t.points} data points
                    </p>
                  </CardContent>
                </Card>
              </Link>
            ))}
          </div>
        </section>

        {/* Stack */}
        <section className="space-y-2 border-t pt-6">
          <h2 className="text-sm font-semibold uppercase tracking-wide text-muted-foreground">
            Stack
          </h2>
          <div className="flex flex-wrap gap-2">
            {["C++17", "CMake", "PID Control", "Sensor Noise Models", "Next.js", "Recharts", "Vercel"].map((t) => (
              <span
                key={t}
                className="rounded-md border bg-white px-2.5 py-0.5 text-xs font-medium"
              >
                {t}
              </span>
            ))}
          </div>
        </section>
      </main>
    </div>
  );
}
