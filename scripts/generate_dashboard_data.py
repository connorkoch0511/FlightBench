#!/usr/bin/env python3
"""
Convert FlightBench telemetry CSVs + test results into JSON for the dashboard.
Run from the repo root: python3 scripts/generate_dashboard_data.py
"""
import csv
import json
import os
import re
import sys
from pathlib import Path

TELEMETRY_DIR  = Path("telemetry")
REPORT_FILE    = Path("reports/test_report.txt")
OUTPUT_DIR     = Path("dashboard/src/data")
MAX_POINTS     = 300  # downsample long runs for the chart

SCENARIO_NAMES = {
    "TC-001": "Straight and Level",
    "TC-002": "Climb",
    "TC-003": "Descent",
    "TC-004": "Turn Left",
    "TC-005": "Turn Right",
    "TC-006": "Combined Climb + Turn",
    "TC-007": "Extended Cruise (FL350)",
}

DESCRIPTIONS = {
    "TC-001": "60-second cruise at 10,000 ft. Altitude must hold within ±50 ft.",
    "TC-002": "120-second climb from 5,000 ft. Must reach at least 8,000 ft.",
    "TC-003": "90-second descent from 10,000 ft. Must lose at least 1,000 ft.",
    "TC-004": "30-second standard-rate left turn from heading 090. Must turn ≥30°.",
    "TC-005": "30-second standard-rate right turn from heading 000. Must turn ≥30°.",
    "TC-006": "60-second combined climb and left turn. Must gain 1,000 ft and turn 15°.",
    "TC-007": "10-minute cruise at FL350. Altitude and heading must remain stable.",
}


def parse_report(path: Path) -> dict:
    """Extract per-test verdicts from the text report."""
    results = {}
    if not path.exists():
        return results
    current_id = None
    for line in path.read_text().splitlines():
        m = re.search(r'\[(PASS|FAIL|ERROR)\]\s+(TC-\d+)', line)
        if m:
            verdict, tc_id = m.group(1), m.group(2)
            current_id = tc_id
            results[tc_id] = {"verdict": verdict, "criteria": []}
        elif current_id and re.match(r'\s+(PASS|FAIL)', line):
            m2 = re.match(r'\s+(PASS|FAIL)\s+(.+)', line)
            if m2:
                results[current_id]["criteria"].append({
                    "verdict": m2.group(1),
                    "description": m2.group(2).strip()
                })
    return results


def load_csv(path: Path, max_points: int) -> list:
    rows = []
    with open(path, newline="") as f:
        reader = csv.DictReader(f)
        all_rows = list(reader)

    total = len(all_rows)
    step = max(1, total // max_points)
    sampled = all_rows[::step]

    for r in sampled:
        rows.append({
            "t":        round(float(r["time_s"]), 2),
            "alt":      round(float(r["true_altitude_ft"]), 1),
            "spd":      round(float(r["true_airspeed_kts"]), 2),
            "hdg":      round(float(r["true_heading_deg"]), 2),
            "vs":       round(float(r["true_vs_fpm"]), 1),
            "pitch":    round(float(r["pitch_deg"]), 2),
            "roll":     round(float(r["roll_deg"]), 2),
            "s_alt":    round(float(r["sensor_altitude_ft"]), 1),
            "s_spd":    round(float(r["sensor_airspeed_kts"]), 2),
            "s_hdg":    round(float(r["sensor_heading_deg"]), 2),
        })
    return rows


def main():
    if not TELEMETRY_DIR.exists():
        print("ERROR: telemetry/ not found. Run ./build/flightbench first.")
        sys.exit(1)

    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    report = parse_report(REPORT_FILE)

    all_tests = []
    for csv_path in sorted(TELEMETRY_DIR.glob("TC-*.csv")):
        tc_id = csv_path.stem
        print(f"Processing {tc_id} ({csv_path.stat().st_size // 1024} KB)...")

        rows = load_csv(csv_path, MAX_POINTS)
        result = report.get(tc_id, {"verdict": "UNKNOWN", "criteria": []})

        test_data = {
            "id":          tc_id,
            "name":        SCENARIO_NAMES.get(tc_id, tc_id),
            "description": DESCRIPTIONS.get(tc_id, ""),
            "verdict":     result["verdict"],
            "criteria":    result["criteria"],
            "points":      len(rows),
            "telemetry":   rows,
        }

        out_path = OUTPUT_DIR / f"{tc_id}.json"
        out_path.write_text(json.dumps(test_data, separators=(",", ":")))
        print(f"  -> {out_path} ({len(rows)} points)")

        all_tests.append({k: v for k, v in test_data.items() if k != "telemetry"})

    index_path = OUTPUT_DIR / "index.json"
    index_path.write_text(json.dumps(all_tests, indent=2))
    print(f"\nIndex written to {index_path} ({len(all_tests)} tests)")


if __name__ == "__main__":
    main()
