---
title: Minimum-Cost b-Flow (最小費用b-flow) [min_cost_b_flow]
documentation_of: ../../../../src/algorithm/matching/flow/minimum_cost_b_flow.hpp
---

各辺の下限・上限・単位費用と、各頂点の要求純流入量を満たす整数流のうち総費用最小のものを求める。負費用辺と負費用閉路を扱う。

## API

```cpp
MinimumCostBFlow solver(vertex_count);
```

空のネットワークを構築する。

```cpp
int edge_id = solver.add_edge(from, to, lower, upper, cost);
```

容量区間と費用を持つ辺を追加する。流量の下限・上限は負でもよい。ただし上限と下限の差は `long long` で表せる範囲とする。

```cpp
solver.set_balance(vertex, inflow_minus_outflow);
solver.add_balance(vertex, delta);
```

頂点で要求する「流入量－流出量」を設定または加算する。正値が需要、負値が供給を表す。

```cpp
optional<MinimumCostBFlowResult> result = solver.solve();
```

実行可能なら入力順の `flow` と `__int128` 型の `cost` を返し、不可能なら `nullopt` を返す。

`solver.dual_potentials(result->flow)` は残余辺の被約費用を非負にする双対ポテンシャルを `vector<__int128>` で返す。最適でない流により負閉路が存在する場合は `domain_error`。このメソッドは $O(VE)$ 時間・$O(V)$ 空間。

## 時間計算量

頂点数を $V$、辺数を $E$、費用絶対値の最大を $C$ とする。

- `add_edge`、`set_balance`、`add_balance`: 償却 $O(1)$
- 実行可能流構築: Dinic法で $O(V^2E)$
- 費用最適化: 整数 cost-scaling 法で $O(VE\log(VC))$ 回の基本操作

## 空間計算量

$O(V+E)$。

## 注意点

容量・需給は `long long`、費用と総費用の内部積は `__int128` である。全需給の和が `0` でない場合も `nullopt` を返す。
