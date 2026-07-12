---
title: K-th Shortest Path
documentation_of: ../src/algorithm/graph/kth_shortest_path.hpp
---

非負重み有向グラフで、単純パスを距離の小さい順に列挙する。

同じ頂点列は1つのパスとして扱う。

# 関数

```cpp
kth_shortest_paths<T>(graph, source, target, k, inf)
```

`graph[v]` は `KthShortestPathEdge<T>{to, cost}` の列。

戻り値は高々 `k` 個の `KthShortestPathResult<T>`。

```cpp
T cost;
vector<int> vertices;
vector<T> prefix_cost;
```

`prefix_cost[i]` は `source` から `vertices[i]` までの距離。

`inf` は内部の初期値であり、到達可能なパスの距離が `inf` 以上でも扱える。

パスの距離と辺コストの加算結果は `T` で表現できる必要がある。

## 時間計算量

Yen法。

- $O(KN(M+N)\log N)$
