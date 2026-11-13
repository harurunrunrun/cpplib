---
title: Weighted Graph Bipartition Utilities (重み付きグラフ二分割補助)
documentation_of: ../../../src/approximate/graph/graph_partition.hpp
---

`approximate::graph` 名前空間で、重み付き無向グラフの 2 分割を扱う補助関数群である。

## `BipartitionResult`

`side[v]` は頂点 `v` が属する側を 0 または 1 で表す。`cut_weight` は両端の側が異なる辺の重み和（カット重み）である。

## `weighted_cut_weight`

~~~cpp
long double weighted_cut_weight(
    size_t vertex_count,
    const vector<WeightedUndirectedEdge>& edges,
    const vector<unsigned char>& side
)
~~~

指定した 2 分割のカット重みを返す。`side` の長さが `vertex_count` でない場合や、0, 1 以外の値を含む場合は `std::invalid_argument` を送出する。辺の入力条件は Weighted Undirected Graph Input に従う。

- 時間計算量: $O(E \log E + N)$
- 追加空間計算量: $O(N+E)$

## `balanced_bipartition`

~~~cpp
vector<unsigned char> balanced_bipartition(size_t vertex_count)
~~~

頂点番号の前半 $\lceil N/2 \rceil$ 個を側 0、残りを側 1 とする。2 つの側の頂点数の差は高々 1 である。

- 時間計算量: $O(N)$
- 追加空間計算量: $O(N)$

## 注意点

頂点番号は指定した頂点数の範囲内でなければならない。自己ループ・平行辺・重みの扱いは Weighted Undirected Graph Input に従う。
