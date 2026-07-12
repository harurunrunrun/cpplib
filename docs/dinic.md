---
title: Dinic
documentation_of: ../src/algorithm/matching/dinic.hpp
---

Dinic法による最大流。

# コンストラクタ

```cpp
Dinic<T> graph(n)
```

# 関数

```cpp
graph.add_edge(from, to, cap)
graph.max_flow(s, t, flow_limit)
graph.min_cut(s)
```

`add_edge` は辺 id を返す。辺情報は `graph.edges[id]` で参照できる。

`flow_limit` は非負である必要がある。`s == t` の場合は `0` を返す。

self-loop を追加できるが、最大流量には寄与しない。

## 時間計算量

- $O(V^2E)$
