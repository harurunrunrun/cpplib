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

Bellman-Fordによる最短増加路法。

- $O(FVE)$
