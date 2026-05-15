import { test, expect } from "@playwright/test";
import path from "path";

const SCREENSHOT_DIR = path.join(__dirname, "screenshots");

test.describe("Overview page", () => {
  test("renders all 11 test cards with PASS badges", async ({ page }) => {
    await page.goto("/");
    await expect(page.getByRole("heading", { name: "FlightBench" })).toBeVisible();

    // All nominal + fault test IDs present
    const allIds = [
      "TC-001","TC-002","TC-003","TC-004","TC-005","TC-006","TC-007",
      "TC-F01","TC-F02","TC-F03","TC-F04",
    ];
    for (const id of allIds) {
      await expect(page.getByText(id).first()).toBeVisible();
    }

    // All 11 PASS badges across both card grids
    const passBadges = page.locator("a").getByText("PASS");
    await expect(passBadges).toHaveCount(11);

    // Fault injection section heading is present
    await expect(page.getByRole("heading", { name: "Fault Injection Tests" })).toBeVisible();

    await page.screenshot({
      path: path.join(SCREENSHOT_DIR, "01-overview.png"),
      fullPage: true,
    });
  });

  test("summary counts show 11 total, 11 passed, 0 failed", async ({ page }) => {
    await page.goto("/");
    await expect(page.getByText("Total tests")).toBeVisible();
    await expect(page.getByText("Passed")).toBeVisible();
    await expect(page.getByText("Failed")).toBeVisible();
    // 11 total (first occurrence is the "Total tests" card)
    await expect(page.getByText("11").first()).toBeVisible();
    // 0 failed
    await expect(page.getByText("0").first()).toBeVisible();
  });
});

test.describe("Nominal test detail pages", () => {
  test("TC-002 Climb — charts and criteria render", async ({ page }) => {
    await page.goto("/test/TC-002");
    await expect(page.getByRole("heading", { name: /Climb/i })).toBeVisible();
    await expect(page.getByText("PASS").first()).toBeVisible();

    await expect(page.getByText(/reached at least 8000ft/)).toBeVisible();
    await expect(page.getByText(/does not exceed 15000ft/)).toBeVisible();

    const charts = page.locator(".recharts-wrapper");
    await expect(charts).toHaveCount(5);

    await page.screenshot({
      path: path.join(SCREENSHOT_DIR, "02-climb-detail.png"),
      fullPage: true,
    });
  });

  test("TC-007 Extended Cruise — sensor noise visible", async ({ page }) => {
    await page.goto("/test/TC-007");
    await expect(page.getByRole("heading", { name: /Extended Cruise/i })).toBeVisible();
    await expect(page.getByText("True Altitude").first()).toBeVisible();
    await expect(page.getByText("Sensor (noisy)").first()).toBeVisible();

    await page.screenshot({
      path: path.join(SCREENSHOT_DIR, "03-cruise-detail.png"),
      fullPage: true,
    });
  });

  test("TC-004 Turn Left — heading chart present", async ({ page }) => {
    await page.goto("/test/TC-004");
    await expect(page.getByRole("heading", { name: /Turn Left/i })).toBeVisible();
    await expect(page.getByText("True Heading").first()).toBeVisible();

    await page.screenshot({
      path: path.join(SCREENSHOT_DIR, "04-turn-left.png"),
      fullPage: true,
    });
  });
});

test.describe("Fault injection test pages", () => {
  test("TC-F01 Stuck Altimeter — fault callout and charts render", async ({ page }) => {
    await page.goto("/test/TC-F01");
    await expect(page.getByRole("heading", { name: /Stuck Altimeter/i })).toBeVisible();

    // Amber fault callout present
    await expect(page.getByText("Injected Fault")).toBeVisible();
    await expect(page.getByText(/STUCK fault/)).toBeVisible();

    await expect(page.getByText("PASS").first()).toBeVisible();
    const charts = page.locator(".recharts-wrapper");
    await expect(charts).toHaveCount(5);

    await page.screenshot({
      path: path.join(SCREENSHOT_DIR, "05-fault-stuck-altimeter.png"),
      fullPage: true,
    });
  });

  test("TC-F02 Airspeed Spike — pitot anomaly page loads", async ({ page }) => {
    await page.goto("/test/TC-F02");
    await expect(page.getByRole("heading", { name: /Airspeed Spike/i })).toBeVisible();
    await expect(page.getByText("Injected Fault")).toBeVisible();
    await expect(page.getByText(/SPIKE fault/)).toBeVisible();

    const charts = page.locator(".recharts-wrapper");
    await expect(charts).toHaveCount(5);

    await page.screenshot({
      path: path.join(SCREENSHOT_DIR, "06-fault-airspeed-spike.png"),
      fullPage: true,
    });
  });

  test("TC-F03 Heading Dropout — dropout description present", async ({ page }) => {
    await page.goto("/test/TC-F03");
    await expect(page.getByRole("heading", { name: /Heading.*Dropout/i })).toBeVisible();
    await expect(page.getByText("Injected Fault")).toBeVisible();
    await expect(page.getByText(/DROPOUT fault/)).toBeVisible();
    await expect(page.getByText("True Heading").first()).toBeVisible();
  });

  test("TC-F04 Altimeter Drift — drift description present", async ({ page }) => {
    await page.goto("/test/TC-F04");
    await expect(page.getByRole("heading", { name: /Altimeter Drift/i })).toBeVisible();
    await expect(page.getByText("Injected Fault")).toBeVisible();
    await expect(page.getByText(/DRIFT fault/)).toBeVisible();
  });
});

test.describe("Navigation", () => {
  test("breadcrumb navigates back to overview", async ({ page }) => {
    await page.goto("/test/TC-F01");
    await page.getByText("← FlightBench").click();
    await expect(page).toHaveURL("/");
    await expect(page.getByRole("heading", { name: "Fault Injection Tests" })).toBeVisible();
  });

  test("non-existent test returns 404", async ({ page }) => {
    const response = await page.goto("/test/TC-999");
    expect(response?.status()).toBe(404);
  });
});

test.describe("All pages load without JS errors", () => {
  const ALL_IDS = [
    "TC-001","TC-002","TC-003","TC-004","TC-005","TC-006","TC-007",
    "TC-F01","TC-F02","TC-F03","TC-F04",
  ];

  for (const id of ALL_IDS) {
    test(`${id} loads cleanly`, async ({ page }) => {
      const errors: string[] = [];
      page.on("pageerror", (e) => errors.push(e.message));

      await page.goto(`/test/${id}`);
      await expect(page.getByText(id).first()).toBeVisible();

      const charts = page.locator(".recharts-wrapper");
      await expect(charts).toHaveCount(5);

      expect(errors).toHaveLength(0);
    });
  }
});
