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

- $O(FE)$
