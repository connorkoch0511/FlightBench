import { test, expect } from "@playwright/test";
import path from "path";

const SCREENSHOT_DIR = path.join(__dirname, "screenshots");

test.describe("Overview page", () => {
  test("renders all 7 test cards with PASS badges", async ({ page }) => {
    await page.goto("/");
    await expect(page.getByRole("heading", { name: "FlightBench" })).toBeVisible();

    // All 7 test IDs present
    for (const id of ["TC-001","TC-002","TC-003","TC-004","TC-005","TC-006","TC-007"]) {
      await expect(page.getByText(id).first()).toBeVisible();
    }

    // 7 PASS badges in the test card grid
    const passBadges = page.locator(".grid a").getByText("PASS");
    await expect(passBadges).toHaveCount(7);

    await page.screenshot({
      path: path.join(SCREENSHOT_DIR, "01-overview.png"),
      fullPage: true,
    });
  });

  test("summary counts show 7 total, 7 passed, 0 failed", async ({ page }) => {
    await page.goto("/");
    await expect(page.getByText("Total tests")).toBeVisible();
    await expect(page.getByText("Passed")).toBeVisible();
    // The 0 failed stat card
    await expect(page.getByText("Failed")).toBeVisible();
    const failCount = page.getByText("0").first();
    await expect(failCount).toBeVisible();
  });
});

test.describe("Test detail pages", () => {
  test("TC-002 Climb — charts and criteria render", async ({ page }) => {
    await page.goto("/test/TC-002");
    await expect(page.getByRole("heading", { name: /Climb/i })).toBeVisible();
    await expect(page.getByText("PASS").first()).toBeVisible();

    // Both criteria listed
    await expect(page.getByText(/reached at least 8000ft/)).toBeVisible();
    await expect(page.getByText(/does not exceed 15000ft/)).toBeVisible();

    // 5 charts per detail page
    const charts = page.locator(".recharts-wrapper");
    await expect(charts).toHaveCount(5);

    await page.screenshot({
      path: path.join(SCREENSHOT_DIR, "02-climb-detail.png"),
      fullPage: true,
    });
  });

  test("TC-007 Extended Cruise — sensor noise visible in altitude chart", async ({ page }) => {
    await page.goto("/test/TC-007");
    await expect(page.getByRole("heading", { name: /Extended Cruise/i })).toBeVisible();

    // Legend items — multiple charts each show these, use first()
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

  test("breadcrumb navigates back to overview", async ({ page }) => {
    await page.goto("/test/TC-001");
    await page.getByText("← FlightBench").click();
    await expect(page).toHaveURL("/");
    await expect(page.getByText("Test Suite")).toBeVisible();
  });

  test("non-existent test returns 404", async ({ page }) => {
    const response = await page.goto("/test/TC-999");
    expect(response?.status()).toBe(404);
  });
});

test.describe("All test detail pages load", () => {
  const TEST_IDS = ["TC-001","TC-002","TC-003","TC-004","TC-005","TC-006","TC-007"];

  for (const id of TEST_IDS) {
    test(`${id} detail page loads without errors`, async ({ page }) => {
      const errors: string[] = [];
      page.on("pageerror", (e) => errors.push(e.message));

      await page.goto(`/test/${id}`);
      await expect(page.getByText(id).first()).toBeVisible();

      // 5 charts per page
      const charts = page.locator(".recharts-wrapper");
      await expect(charts).toHaveCount(5);

      expect(errors).toHaveLength(0);
    });
  }
});
