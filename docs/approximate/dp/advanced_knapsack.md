---
title: Advanced Knapsack Approximations (高度なナップサック近似)
documentation_of: ../../../src/approximate/dp/advanced_knapsack.hpp
---

Heuristics and bounded-state DPs for nonnegative integral 0/1 knapsack inputs.
非負整数 0/1 ナップサック向けのヒューリスティックと状態数制限 DP である。

## API summary / API 一覧

| API | Method / 方法 | Time / 時間計算量 | Extra space / 追加空間 |
|---|---|---:|---:|
| `core_problem_knapsack` | exact enumeration near the ratio break / 比率順 break 周辺の core を全探索 | \(O(n\log n+c2^c)\) | \(O(n)\) |
| `weight_scaling_knapsack_dp` | upward weight rounding + capacity DP / 重量切上げ後の容量 DP | \(O(nC')\) | \(O(C'+nC'/64)\) |
| `surrogate_relaxation_knapsack` | weighted combination of \(m\) constraints / \(m\) 制約の重み付き結合 | \(O(mn+n\log n)\) | \(O(mn)\) including normalized weights |
| `lagrangian_relaxation_knapsack` | positive reduced-profit order / 正の reduced profit 順 | \(O(n\log n)\) | \(O(n)\) |
| `dantzig_bound_knapsack` | fractional upper bound + modified greedy / 分数上界と修正貪欲解 | \(O(n\log n)\) | \(O(n)\) |

Here \(c\le24\) is `core_size`, and
\(C'\le\)`maximum_scaled_capacity`.
ここで \(c\le24\)、\(C'\) は指定した scaled capacity 上限以下である。

## Feasibility and guarantees / 実行可能性と保証

`core_problem_knapsack` never returns a solution worse than
`modified_greedy_knapsack`; it therefore retains that routine's \(1/2\)
guarantee. Its core enumeration may improve the solution but is not globally
exact unless the relevant items are all inside the core.
`core_problem_knapsack` は修正貪欲解を fallback とするため \(1/2\) 保証を維持する。core 外を含む大域的厳密解は保証しない。

`weight_scaling_knapsack_dp` rounds weights upward, so its reconstructed solution
is feasible in the original instance. Weight scaling alone has no general
value-approximation ratio; use `value_scaling_knapsack_fptas` when an FPTAS
guarantee is required.
重量切上げにより元問題での実行可能性を保つが、一般の価値近似比は持たない。FPTAS が必要なら既存の `value_scaling_knapsack_fptas` を使う。

Surrogate and Lagrangian routines always test the original capacity constraints
before selecting an item, but are primal heuristics without a general
approximation ratio. `dantzig_bound_knapsack::fractional_upper_bound` is the LP
fractional-knapsack upper bound and is at least the integral optimum, subject to
floating-point rounding.
surrogate/Lagrangian は元制約を確認して実行可能解を返すが一般近似比はない。Dantzig の分数値は浮動小数誤差を除き整数最適値の上界である。

## 注意点

Negative inputs, dimension mismatches, nonfinite parameters, excessive core
size, and arithmetic overflow are rejected with standard exceptions.
負入力、次元不一致、非有限 parameter、過大な core、算術 overflow には標準例外を送出する。
