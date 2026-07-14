---
title: Count Bipartite Edge Crossings (二部直線描画の辺交差数) [MSE06H]
documentation_of: ../src/algorithm/other/count_bipartite_edge_crossings.hpp
---

左右2列の頂点を各番号順に並べたとき、二部グラフの辺同士が内部で交差する組数を返す。

# count_bipartite_edge_crossings

```cpp
long long count_bipartite_edge_crossings(
    vector<pair<int, int>> edges
);
```

各辺を `(left_vertex, right_vertex)` で渡す。

## 時間計算量

辺数を $M$ として $O(M\log M)$。

## 空間計算量

$O(M)$。

## 注意点

- 同じ左端点を持つ辺同士、または同じ右端点を持つ辺同士は交差に数えない。
- 結果は `long long` で表現できる必要がある。
