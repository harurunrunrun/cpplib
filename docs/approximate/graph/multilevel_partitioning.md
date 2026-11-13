---
title: Multilevel Graph Bipartitioning (多段階グラフ二分割)
documentation_of: ../../../src/approximate/graph/multilevel_partitioning.hpp
---

重い辺のマッチングによる粗視化（coarsening）、スペクトル二分割、詳細化（uncoarsening）時の Fiduccia–Mattheyses 法による改善を組み合わせた、重み付きグラフ分割のヒューリスティックである。最適解は保証しない。

## `MultilevelPartitionResult`

`side` と `cut_weight` に加え、実際に縮約した段数 `coarsening_levels` を返す。

## `multilevel_graph_partitioning`

~~~cpp
MultilevelPartitionResult multilevel_graph_partitioning(
    size_t vertex_count,
    const vector<WeightedUndirectedEdge>& edges,
    size_t coarsest_vertex_count = 40,
    size_t maximum_imbalance = 2,
    size_t maximum_levels = 32,
    size_t refinement_passes = 4
)
~~~

未マッチの各頂点について、未マッチの隣接頂点のうち辺重み最大のものと組にして縮約する。重みが等しい場合は頂点番号が小さいものを選ぶ。粗いグラフの平行辺は重みを加算する。詳細化の各段階で、両側の頂点数の差を `maximum_imbalance` 以下に戻してから改善を行う。

レベル $l$ の頂点数・辺数を $N_l, E_l$、スペクトル法の反復回数を $I$、改善のパス数を $R$ とする。

- 時間計算量: $O(\sum_l(E_l\log E_l + I(N_l+E_l)+R(N_l+E_l)\log N_l))$
- 追加空間計算量: $O(\sum_l(N_l+E_l))$

## 注意点

- `coarsest_vertex_count < 2` や実現不能な不均衡制約（`maximum_imbalance` が $N \bmod 2$ 未満）には `std::invalid_argument` を送出する
- `maximum_levels == 0` では粗視化を省略する
- 空グラフを許す
- 自己ループ・平行辺・重みの扱いは Weighted Undirected Graph Input に従う
