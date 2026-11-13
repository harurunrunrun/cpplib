---
title: Advanced Knapsack Approximations (高度なナップサック近似)
documentation_of: ../../../src/approximate/dp/advanced_knapsack.hpp
---

非負整数の 0/1 ナップサック問題に対する、ヒューリスティックと状態数を制限した DP である。`approximate::dp` 名前空間にある。

## API 一覧

```cpp
KnapsackResult core_problem_knapsack(weights, values, capacity, core_size = 18);
KnapsackResult weight_scaling_knapsack_dp(weights, values, capacity, maximum_scaled_capacity);
MultiKnapsackResult surrogate_relaxation_knapsack(constraint_weights, values, capacities, multipliers);
KnapsackResult lagrangian_relaxation_knapsack(weights, values, capacity, penalty);
DantzigBoundResult dantzig_bound_knapsack(weights, values, capacity);
```

| API | 方法 | 時間計算量 | 追加空間計算量 |
| --- | --- | ---: | ---: |
| `core_problem_knapsack` | 価値/重さの比の順で容量を超える位置（break）の周辺の core を全探索 | $O(n\log n+c2^c)$ | $O(n)$ |
| `weight_scaling_knapsack_dp` | 重さを切り上げて縮小した後の容量 DP | $O(nC')$ | $O(C'+nC'/64)$ |
| `surrogate_relaxation_knapsack` | $m$ 本の制約の重み付き結合（代理緩和） | $O(mn+n\log n)$ | $O(mn)$（正規化した重さを含む） |
| `lagrangian_relaxation_knapsack` | 正の被約利得（reduced profit）の順に選ぶ | $O(n\log n)$ | $O(n)$ |
| `dantzig_bound_knapsack` | 分数緩和による上界と修正貪欲解 | $O(n\log n)$ | $O(n)$ |

ここで $c\le 24$ は `core_size`、$C'$ は縮小後の容量で `maximum_scaled_capacity` 以下である。$m$ は制約の本数である。

`MultiKnapsackResult` は `selected_indices`、制約ごとの重さの合計 `total_weights`、`total_value` を持つ。`DantzigBoundResult` は解 `solution` と分数上界 `fractional_upper_bound` を持つ。

## 実行可能性と保証

`core_problem_knapsack` は `modified_greedy_knapsack` の解より悪い解を返さないため、その $1/2$ 近似保証を保つ。core の全探索で解が改善することはあるが、関係する品物が全て core 内にある場合を除き、大域的な厳密解は保証しない。

`weight_scaling_knapsack_dp` は重さを切り上げるため、復元した解は元の問題でも実行可能である。ただし重さの縮小だけでは一般の価値の近似比を持たない。FPTAS の保証が必要なら `value_scaling_knapsack_fptas` を使う。

代理緩和とラグランジュ緩和の各関数は、品物を選ぶ前に必ず元の容量制約を確認するので実行可能解を返すが、一般の近似比は持たない主問題側のヒューリスティックである。`dantzig_bound_knapsack` の `fractional_upper_bound` は LP 緩和（分数ナップサック）による上界であり、浮動小数点の丸め誤差を除いて整数最適値以上である。

## 注意点

- `core_size > 24`、`maximum_scaled_capacity == 0`、負の入力、次元の不一致、非有限または負の `penalty` などには `std::invalid_argument` を送出する。算術オーバーフローには `std::overflow_error` を送出する。
- `core_size == 0` なら修正貪欲解をそのまま返す。
