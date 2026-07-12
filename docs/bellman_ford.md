---
title: Bellman Ford
documentation_of: ../src/algorithm/graph/bellman_ford.hpp
---

単一始点最短路。負辺に対応する。

# 関数

```cpp
bellman_ford<T>(int n, vector<BellmanFordEdge<T>> edges, int source, T inf)
```

`BellmanFordResult<T>` は次を持つ。

```cpp
vector<T> dist;
vector<int> parent;
vector<char> reachable;
vector<char> negative;
bool has_negative_cycle;
```

`reachable[v]` は始点から `v` へ到達可能なら真。`inf` は到達不能な頂点の `dist` に格納する初期値であり、到達可能な最短距離が `inf` 以上でも扱える。

`negative[v]` は、始点から到達可能な負閉路の影響を受ける頂点を表す。

距離と辺コストの加算結果は `T` で表現できる必要がある。

## 時間計算量

- $O(NM)$
