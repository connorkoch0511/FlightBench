"use client";

import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  Legend,
  ResponsiveContainer,
} from "recharts";
import type { TelemetryPoint } from "@/lib/types";

interface Series {
  key: keyof TelemetryPoint;
  label: string;
  color: string;
  dashed?: boolean;
}

interface Props {
  data: TelemetryPoint[];
  series: Series[];
  title: string;
  unit: string;
  yDomain?: [number | "auto", number | "auto"];
}

export function TelemetryChart({ data, series, title, unit, yDomain }: Props) {
  return (
    <div className="flex flex-col gap-2">
      <p className="text-sm font-medium text-muted-foreground">{title}</p>
      <ResponsiveContainer width="100%" height={220}>
        <LineChart data={data} margin={{ top: 4, right: 16, bottom: 0, left: 0 }}>
          <CartesianGrid strokeDasharray="3 3" stroke="#e5e7eb" />
          <XAxis
            dataKey="t"
            tick={{ fontSize: 11 }}
            tickFormatter={(v) => `${v}s`}
            label={{ value: "Time (s)", position: "insideBottomRight", offset: -4, fontSize: 11 }}
          />
          <YAxis
            tick={{ fontSize: 11 }}
            domain={yDomain ?? ["auto", "auto"]}
            tickFormatter={(v) => `${v}`}
            width={62}
            label={{ value: unit, angle: -90, position: "insideLeft", fontSize: 11 }}
          />
          <Tooltip
            formatter={(val, name) => [`${Number(val).toFixed(1)} ${unit}`, name as string]}
            labelFormatter={(l) => `t = ${l}s`}
            contentStyle={{ fontSize: 12 }}
          />
          <Legend wrapperStyle={{ fontSize: 12 }} />
          {series.map((s) => (
            <Line
              key={s.key}
              type="monotone"
              dataKey={s.key}
              name={s.label}
              stroke={s.color}
              strokeWidth={s.dashed ? 1.5 : 2}
              strokeDasharray={s.dashed ? "4 3" : undefined}
              dot={false}
              isAnimationActive={false}
            />
          ))}
        </LineChart>
      </ResponsiveContainer>
    </div>
  );
}
