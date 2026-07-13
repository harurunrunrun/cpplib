---
title: Ford Fulkerson
documentation_of: ../src/algorithm/matching/ford_fulkerson.hpp
---

Ford-Fulkerson法による最大流。

# コンストラクタ

```cpp
FordFulkerson<T> graph(n)
```

# 関数

```cpp
graph.add_edge(from, to, cap)
graph.max_flow(s, t, flow_limit)
```

容量が整数で、最大流量が小さい場合に使う。

`flow_limit` は非負である必要がある。`s == t` の場合は `0` を返す。

self-loop を追加できるが、最大流量には寄与しない。

## 時間計算量

$V$ を頂点数、$E$ を追加済み辺数、$A$ を今回の `max_flow` で成功する増加回数とする。

- `FordFulkerson(n)`: $O(V)$
- `add_edge`: 償却 $O(1)$
- `dfs`: 1回 $O(V+E)$
- `max_flow`: $O((A+1)(V+E))$

整数容量で正の容量が1以上なら $A$ は追加流量以下なので、追加流量を $F$ として $O((F+1)(V+E))$ でも上から抑えられる。

## 空間計算量

- 残余グラフとDFS領域を含めて $O(V+E)$
