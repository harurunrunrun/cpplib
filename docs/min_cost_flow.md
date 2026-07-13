---
title: Min Cost Flow
documentation_of: ../src/algorithm/matching/min_cost_flow.hpp
---

最小費用流。

# コンストラクタ

```cpp
MinCostFlow<T> graph(n)
```

# 関数

```cpp
graph.add_edge(from, to, cap, cost)
graph.min_cost_flow(source, sink, flow_limit, inf)
```

`source` と `sink` は異なる頂点、`flow_limit` は非負でなければならない。
残余グラフで `source` から到達可能な負閉路を検出した場合は `runtime_error` を送出する。
経路費用と `flow * cost` は `T` の表現範囲内である必要がある。

`MinCostFlowResult<T>` は次を持つ。

```cpp
T flow;
T cost;
```

## 時間計算量

$V$ を頂点数、$E$ を追加済み辺数、$A$ を今回成功する増加回数とする。

- `MinCostFlow(n)`: $O(V)$
- `add_edge`: 償却 $O(1)$
- 1回の最短増加路探索: Bellman--Fordにより $O(VE)$
- `min_cost_flow`: $O((A+1)(VE+V))$

最後に増加路が存在しないことを確認する探索も含む。整数容量で正の容量が1以上なら $A$ は追加流量 $F$ 以下なので $O((F+1)(VE+V))$ でも上から抑えられる。各増加で残余容量と公開 `edges` のflowを更新する。

## 空間計算量

- 残余グラフ、公開辺列、最短路作業領域を含めて $O(V+E)$
