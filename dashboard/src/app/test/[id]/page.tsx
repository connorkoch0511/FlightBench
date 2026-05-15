import Link from "next/link";
import { notFound } from "next/navigation";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { VerdictBadge } from "@/components/VerdictBadge";
import { TelemetryChart } from "@/components/TelemetryChart";
import type { TestData } from "@/lib/types";

const TEST_IDS = ["TC-001","TC-002","TC-003","TC-004","TC-005","TC-006","TC-007"];

export function generateStaticParams() {
  return TEST_IDS.map((id) => ({ id }));
}

async function getTestData(id: string): Promise<TestData> {
  const { default: data } = await import(`@/data/${id}.json`);
  return data as TestData;
}

export default async function TestPage({ params }: { params: Promise<{ id: string }> }) {
  const { id } = await params;
  if (!TEST_IDS.includes(id)) notFound();

  const test = await getTestData(id);

  const altSeries = [
    { key: "alt"  as const, label: "True Altitude",   color: "#2563eb" },
    { key: "s_alt"as const, label: "Sensor (noisy)",  color: "#93c5fd", dashed: true },
  ];
  const spdSeries = [
    { key: "spd"  as const, label: "True Airspeed",   color: "#16a34a" },
    { key: "s_spd"as const, label: "Sensor (noisy)",  color: "#86efac", dashed: true },
  ];
  const hdgSeries = [
    { key: "hdg"  as const, label: "True Heading",    color: "#9333ea" },
    { key: "s_hdg"as const, label: "Sensor (noisy)",  color: "#d8b4fe", dashed: true },
  ];
  const vsSeries = [
    { key: "vs"   as const, label: "Vertical Speed",  color: "#ea580c" },
  ];
  const attitudeSeries = [
    { key: "pitch"as const, label: "Pitch",           color: "#0891b2" },
    { key: "roll" as const, label: "Roll",            color: "#0d9488" },
  ];

  return (
    <div className="min-h-screen bg-zinc-50">
      <header className="border-b bg-white px-6 py-4">
        <div className="mx-auto max-w-5xl flex items-center gap-3">
          <Link href="/" className="text-sm text-muted-foreground hover:text-foreground">
            ← FlightBench
          </Link>
          <span className="text-muted-foreground">/</span>
          <span className="text-sm font-mono">{test.id}</span>
        </div>
      </header>

      <main className="mx-auto max-w-5xl px-6 py-8 space-y-6">
        {/* Header */}
        <div className="flex items-start justify-between gap-4">
          <div>
            <h1 className="text-xl font-semibold">{test.name}</h1>
            <p className="mt-1 text-sm text-muted-foreground max-w-xl">
              {test.description}
            </p>
          </div>
          <VerdictBadge verdict={test.verdict} />
        </div>

        {/* Criteria */}
        <Card>
          <CardHeader className="pb-2">
            <CardTitle className="text-sm">Pass / Fail Criteria</CardTitle>
          </CardHeader>
          <CardContent>
            <ul className="space-y-1.5">
              {test.criteria.map((c, i) => (
                <li key={i} className="flex items-start gap-2 text-sm">
                  <span className={c.verdict === "PASS" ? "text-green-600 font-medium" : "text-red-600 font-medium"}>
                    {c.verdict}
                  </span>
                  <span className="text-muted-foreground">{c.description}</span>
                </li>
              ))}
            </ul>
          </CardContent>
        </Card>

        {/* Charts */}
        <div className="grid gap-4 md:grid-cols-2">
          <Card>
            <CardContent className="pt-6">
              <TelemetryChart
                data={test.telemetry}
                series={altSeries}
                title="Altitude"
                unit="ft"
              />
            </CardContent>
          </Card>

          <Card>
            <CardContent className="pt-6">
              <TelemetryChart
                data={test.telemetry}
                series={spdSeries}
                title="Airspeed"
                unit="kts"
              />
            </CardContent>
          </Card>

          <Card>
            <CardContent className="pt-6">
              <TelemetryChart
                data={test.telemetry}
                series={hdgSeries}
                title="Heading"
                unit="deg"
                yDomain={[0, 360]}
              />
            </CardContent>
          </Card>

          <Card>
            <CardContent className="pt-6">
              <TelemetryChart
                data={test.telemetry}
                series={vsSeries}
                title="Vertical Speed"
                unit="fpm"
              />
            </CardContent>
          </Card>

          <Card className="md:col-span-2">
            <CardContent className="pt-6">
              <TelemetryChart
                data={test.telemetry}
                series={attitudeSeries}
                title="Attitude (Pitch & Roll)"
                unit="deg"
                yDomain={[-40, 40]}
              />
            </CardContent>
          </Card>
        </div>

        <p className="text-xs text-muted-foreground text-right">
          {test.points} data points · solid lines = ground truth · dashed = noisy sensor reading
        </p>
      </main>
    </div>
  );
}
