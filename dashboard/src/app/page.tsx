import Link from "next/link";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { VerdictBadge } from "@/components/VerdictBadge";
import type { TestMeta } from "@/lib/types";
import indexData from "@/data/index.json";

const tests = indexData as TestMeta[];
const nominal = tests.filter((t) => !t.fault);
const faults  = tests.filter((t) => t.fault);

const scenarioIcon: Record<string, string> = {
  "TC-001": "✈", "TC-002": "↑", "TC-003": "↓",
  "TC-004": "↺", "TC-005": "↻", "TC-006": "↑↺", "TC-007": "✈",
  "TC-F01": "⚠", "TC-F02": "⚠", "TC-F03": "⚠", "TC-F04": "⚠",
};

function TestCard({ t }: { t: TestMeta }) {
  return (
    <Link href={`/test/${t.id}`}>
      <Card className="h-full cursor-pointer transition-shadow hover:shadow-md">
        <CardHeader className="pb-2">
          <div className="flex items-start justify-between gap-2">
            <div className="flex items-center gap-2">
              <span className="text-lg">{scenarioIcon[t.id]}</span>
              <span className="text-xs font-mono text-muted-foreground">{t.id}</span>
            </div>
            <VerdictBadge verdict={t.verdict} />
          </div>
          <CardTitle className="text-sm leading-snug">{t.name}</CardTitle>
        </CardHeader>
        <CardContent>
          <p className="text-xs text-muted-foreground leading-relaxed">{t.description}</p>
          {t.fault_description && (
            <p className="mt-2 text-xs text-amber-700 bg-amber-50 rounded px-2 py-1 leading-relaxed">
              {t.fault_description}
            </p>
          )}
          <p className="mt-2 text-xs text-muted-foreground">
            {t.criteria.length} criteria · {t.points} data points
          </p>
        </CardContent>
      </Card>
    </Link>
  );
}

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
            end-state criteria. Fault injection tests wrap sensors with configurable failure
            modes — stuck values, spikes, dropouts, and drift — to verify the system
            degrades gracefully under realistic sensor anomalies.
          </p>
        </section>

        {/* Nominal test suite */}
        <section className="space-y-3">
          <h2 className="text-sm font-semibold uppercase tracking-wide text-muted-foreground">
            Nominal Test Suite
          </h2>
          <div className="grid gap-3 sm:grid-cols-2 lg:grid-cols-3">
            {nominal.map((t) => <TestCard key={t.id} t={t} />)}
          </div>
        </section>

        {/* Fault injection suite */}
        <section className="space-y-3">
          <div className="flex items-center gap-2">
            <h2 className="text-sm font-semibold uppercase tracking-wide text-muted-foreground">
              Fault Injection Tests
            </h2>
            <span className="rounded-full bg-amber-100 px-2 py-0.5 text-xs font-medium text-amber-800">
              {faults.length} faults
            </span>
          </div>
          <p className="text-xs text-muted-foreground max-w-2xl">
            Each test wraps a sensor with a configurable fault active during a defined time
            window. Criteria verify the system behaves correctly despite faulty sensor input.
            Charts show the divergence between ground truth and sensor readings.
          </p>
          <div className="grid gap-3 sm:grid-cols-2">
            {faults.map((t) => <TestCard key={t.id} t={t} />)}
          </div>
        </section>

        {/* Stack */}
        <section className="space-y-2 border-t pt-6">
          <h2 className="text-sm font-semibold uppercase tracking-wide text-muted-foreground">
            Stack
          </h2>
          <div className="flex flex-wrap gap-2">
            {["C++17", "CMake", "PID Control", "Sensor Noise Models", "Fault Injection",
              "Next.js", "Recharts", "Playwright", "Vercel"].map((tag) => (
              <span key={tag} className="rounded-md border bg-white px-2.5 py-0.5 text-xs font-medium">
                {tag}
              </span>
            ))}
          </div>
        </section>
      </main>
    </div>
  );
}
