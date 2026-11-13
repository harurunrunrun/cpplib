---
title: Kernighan–Lin Graph Partitioning (Kernighan–Linグラフ分割)
documentation_of: ../../../src/approximate/graph/kernighan_lin.hpp
---

重み付き無向グラフのカット重みを局所的に小さくする Kernighan–Lin 法である。最適解は保証しない。

## `kernighan_lin`

~~~cpp
BipartitionResult kernighan_lin(
    size_t vertex_count,
    const vector<WeightedUndirectedEdge>& edges,
    vector<unsigned char> initial_side,
    size_t maximum_passes = 20
)

BipartitionResult kernighan_lin(
    size_t vertex_count,
    const vector<WeightedUndirectedEdge>& edges,
    size_t maximum_passes = 20
)
~~~

各パスで、異なる側にある未固定の頂点対を利得最大のものから順に選んで仮に交換し、累積利得が最大となる交換列の先頭部分だけを適用する。カット重みが厳密に減らないパスで終了する。交換だけを使うため、初期分割の各側の頂点数は保存される。初期分割を省略した場合は `balanced_bipartition` を使う。利得が等しい場合は頂点番号の組が辞書順最小のものを選ぶ。

- 時間計算量: $O(E \log E + P(N^3 \log N+E))$。$P$ は実行したパス数
- 追加空間計算量: $O(N+E)$

## 注意点

- 空グラフを許す。頂点数が 2 未満、辺重みの総和が 0、または `maximum_passes == 0` の場合は初期分割をそのまま返す
- 利得は重みを全辺の重み和で正規化して計算し、大きな有限重みによる中間計算のオーバーフローを避ける
- `initial_side` の検査、自己ループ・平行辺・重みの扱いは Weighted Undirected Graph Input と `weighted_cut_weight` に従う
