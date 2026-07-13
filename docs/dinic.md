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

$V$ を頂点数、$E$ を追加済み辺数とする。

- `Dinic(n)`: $O(V)$
- `add_edge`: 償却 $O(1)$
- `bfs`: $O(V+E)$
- `dfs`: 1回 worst-case $O(V+E)$。1つのlevel graphでblocking flowを作る全呼出しは worst-case $O(VE)$
- `max_flow`: 一般容量で $O(V^2E)$
- `min_cut`: $O(V+E)$。長さ $V$ の到達列出力を含む

## 空間計算量

- 残余グラフ、level/iterator、戻り値を含めて $O(V+E)$
