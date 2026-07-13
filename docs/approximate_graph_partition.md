---
title: Weighted Graph Bipartition Utilities (重み付きグラフ二分割補助)
documentation_of: ../src/approximate/graph/graph_partition.hpp
---

`approximate::graph` 名前空間で重み付き無向グラフの2分割を扱う。

# BipartitionResult

`side[v]` は頂点 `v` の所属する側を0または1で表す。`cut_weight` は両端のsideが異なる辺の重み和である。

# weighted_cut_weight

~~~cpp
long double weighted_cut_weight(
    size_t vertex_count,
    const vector<WeightedUndirectedEdge>& edges,
    const vector<unsigned char>& side
)
~~~

指定した2分割のcut重みを返す。

- 時間計算量: $O(E \log E + N)$
- 追加空間計算量: $O(N+E)$

# balanced_bipartition

~~~cpp
vector<unsigned char> balanced_bipartition(size_t vertex_count)
~~~

頂点番号の前半をside 0、後半をside 1とする。2つの頂点数の差は高々1である。

- 時間計算量: $O(N)$
- 追加空間計算量: $O(N)$

`side` の長さ・値を検査し、不正値には `std::invalid_argument` を送出する。辺の入力条件は Weighted Undirected Graph Input に従う。

## 注意点

頂点番号は指定した頂点数の範囲内でなければならない。自己loop・平行辺・重みの扱いは各APIの記載に従い、明記した保証を除いて最適解を返すとは限らない。
