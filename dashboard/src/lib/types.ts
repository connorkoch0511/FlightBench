export interface TelemetryPoint {
  t: number;
  alt: number;
  spd: number;
  hdg: number;
  vs: number;
  pitch: number;
  roll: number;
  s_alt: number;
  s_spd: number;
  s_hdg: number;
}

export interface Criterion {
  verdict: "PASS" | "FAIL";
  description: string;
}

export interface TestMeta {
  id: string;
  name: string;
  description: string;
  verdict: "PASS" | "FAIL" | "ERROR" | "UNKNOWN";
  criteria: Criterion[];
  points: number;
}

export interface TestData extends TestMeta {
  telemetry: TelemetryPoint[];
}
