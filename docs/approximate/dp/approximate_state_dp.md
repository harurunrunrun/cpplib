---
title: Approximate State-Space Dynamic Programming (状態空間の近似動的計画法)
documentation_of: ../../../src/approximate/dp/approximate_state_dp.hpp
---

Generic layer-by-layer DP controllers. An `expand(state, layer)` callback returns
a range of successor states. Larger `score(state)` values are better.
汎用の layer 型 DP 制御器である。`expand(state, layer)` は次状態 range を返し、`score(state)` は大きいほど良いものとする。
各 API の時間計算量と追加空間計算量は以下の各表・節に示す。

Every layered function returns `StateDpResult<State>` with retained `states`,
`layers_completed`, and `interrupted`.
各関数は保持状態、完了 layer 数、中断 flag を返す。callback は決定的である必要はないが、比較関数は strict weak ordering と整合させる。

## Bounded selection / 状態数制限

| API | Retention rule / 保持規則 | Per-layer control cost / layer ごとの制御時間 |
|---|---|---:|
| `beam_dynamic_programming` | best beam after linear selection / 線形選択後の上位 beam | \(O(N+K\log K)\) |
| `top_k_dynamic_programming` | partial-sort top K / partial sort 上位 K | \(O(N\log K)\) |
| `hash_beam_dynamic_programming` | best per hash bucket, then beam / hash bucket ごとの最良後 beam | expected \(O(N+B\log B)\) |
| `state_count_limited_dynamic_programming` | first states in transition order / 遷移順の先頭 | \(O(N)\) |
| `stochastic_beam_dynamic_programming` | uniform shuffle then truncate / 一様 shuffle 後打切り | \(O(N)\) |

\(N\) is the generated state count, \(K\) the limit, and \(B\) the occupied
bucket count. Space is \(O(N)\) while processing a layer and \(O(K)\) retained.
遷移結果を materialize するため処理中空間は \(O(N)\)、保持空間は \(O(K)\) である。

## Merging and approximation / 統合と近似

- `sparse_dynamic_programming`: keeps the maximum-score state per exact key.
  exact key ごとに最大 score を保持。Expected \(O(N)\).
- `value_rounding_dynamic_programming`: additive value buckets of width
  `quantum`. 幅 `quantum` の加法 bucket。Expected \(O(N)\).
- `state_compression_dynamic_programming`: canonicalizes states and removes
  exact duplicates. 状態を canonical 化して重複除去。Expected \(O(N)\).
- `state_aggregation_dynamic_programming`: merges equal-key states using an
  associative callback. 同一 key を callback で集約。Expected \(O(N)\).
- `dominance_pruned_dynamic_programming`: retains a nondominated frontier.
  非支配 frontier を保持。\(O(N^2)\) dominance tests.
- `epsilon_dominance_dynamic_programming`: keeps one representative per
  multiplicative objective box. 乗法 epsilon box ごとに1状態。Expected
  \(O(ND)\).

The hash-based expected bounds assume well-distributed caller-supplied hashes.
hash 系の期待計算量は適切に分散する hash を仮定する。

## Frontier and interruption / frontier と中断

`approximate_pareto_frontier(candidates, objectives, epsilon)` first compresses
nonnegative maximization objectives into multiplicative epsilon boxes and then
removes exactly dominated representatives. Its complexity is
\(O(ND+F^2D)\) time and \(O(FD)\) extra space.
非負の最大化目的を epsilon box 化した後、代表間の厳密支配を除去する。\(F\) は box 代表数である。

`interrupted_dynamic_programming` checks `stop(layer, states)` before every
layer. It returns the current frontier with `interrupted == true` when requested.
各 layer 前に停止 callback を評価するため、時間・memory budget や外部停止 flag を安全に接続できる。

## 注意点

Limits must be positive. Epsilon and rounding quantum must be finite and
positive; epsilon objectives must be finite, nonnegative, and fixed-dimensional.
不正値には `std::invalid_argument`、bucket index overflow には
`std::overflow_error` を送出する。
