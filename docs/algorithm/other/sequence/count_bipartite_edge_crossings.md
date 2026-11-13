---
title: Count Bipartite Edge Crossings (二部直線描画の辺交差数) [MSE06H]
documentation_of: ../../../../src/algorithm/other/sequence/count_bipartite_edge_crossings.hpp
---

左右 2 列の頂点をそれぞれ番号順に並べ、二部グラフの辺を線分で描いたとき、内部で交差する辺の組の数を返す。辺を左端点で整列し、右端点の列の転倒数として求める。

## count_bipartite_edge_crossings

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

- 同じ左端点を持つ辺同士、同じ右端点を持つ辺同士（重複辺を含む）は交差に数えない。
- 結果は `long long` で表現できなければならない。
