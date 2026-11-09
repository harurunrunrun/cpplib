---
title: Minimum-Cost b-Flow (最小費用b-flow) [min_cost_b_flow]
documentation_of: ../../../../src/algorithm/matching/flow/minimum_cost_b_flow.hpp
---

各辺の流量の下限・上限・単位コストと、各頂点で要求される正味の流入量を満たす整数フローのうち、総コストが最小のものを求める。負コストの辺と負コストの閉路を扱える。

## API

```cpp
MinimumCostBFlow solver(vertex_count);
```

空のネットワークを構築する。

```cpp
int edge_id = solver.add_edge(from, to, lower, upper, cost);
```

流量の区間 $[\mathit{lower}, \mathit{upper}]$ とコストを持つ辺を追加し、その番号を返す。流量の下限・上限は負でもよい。ただし上限と下限の差は `long long` で表せる範囲とする。

```cpp
solver.set_balance(vertex, inflow_minus_outflow);
solver.add_balance(vertex, delta);
```

頂点で要求する「流入量 − 流出量」を設定、または加算する。正の値が需要、負の値が供給を表す。

```cpp
optional<MinimumCostBFlowResult> result = solver.solve();
```

実行可能なら入力順の流量 `flow` と `__int128` 型の総コスト `cost` を返し、実行不可能なら `nullopt` を返す。

`solver.dual_potentials(result->flow)` は、残余辺の被約コストを非負にする双対ポテンシャルを `vector<__int128>` で返す。流量列の長さが辺数と異なる場合や、流量が区間外の場合は `invalid_argument` を、最適でないフローのため残余グラフに負閉路が存在する場合は `domain_error` を送出する。このメソッドは時間 $O(VE)$、空間 $O(V)$ である。

## 時間計算量

頂点数を $V$、辺数を $E$、コストの絶対値の最大値を $C$ とする。

- `add_edge`、`set_balance`、`add_balance`: 償却 $O(1)$
- 実行可能フローの構築: Dinic 法で $O(V^2 E)$
- コストの最適化: 整数コストスケーリング法で $O(VE \log(VC))$ 回の基本操作

## 空間計算量

$O(V+E)$。

## 注意点

容量と需給は `long long`、コストと総コストの内部計算は `__int128` で行う。全頂点の需給の和が $0$ でない場合も `nullopt` を返す。頂点数が負の場合、頂点番号が範囲外の場合、`upper < lower` などの不正な辺に対しては `runtime_error` を、需給の加算や内部の需要量が `long long` を超える場合は `overflow_error` を送出する。
