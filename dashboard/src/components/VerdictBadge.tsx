import { Badge } from "@/components/ui/badge";
import type { TestMeta } from "@/lib/types";

export function VerdictBadge({ verdict }: { verdict: TestMeta["verdict"] }) {
  const styles = {
    PASS:    "bg-green-100 text-green-800 border-green-200",
    FAIL:    "bg-red-100 text-red-800 border-red-200",
    ERROR:   "bg-yellow-100 text-yellow-800 border-yellow-200",
    UNKNOWN: "bg-gray-100 text-gray-600 border-gray-200",
  } as const;

  return (
    <Badge variant="outline" className={styles[verdict]}>
      {verdict}
    </Badge>
  );
}
