---
title: Multilevel Graph Bipartitioning (多段階グラフ二分割)
documentation_of: ../src/approximate/graph/multilevel_partitioning.hpp
---

heavy-edge matchingによるcoarsening、spectral bisection、uncoarsening時のFiduccia–Mattheyses refinementを組み合わせた重み付きグラフ分割heuristicである。最適解を保証しない。

# MultilevelPartitionResult

`side` と `cut_weight` に加え、実際に縮約した `coarsening_levels` を返す。

# multilevel_graph_partitioning

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

未match頂点ごとに最大重みの未match隣接頂点を選んで縮約する。同重みでは頂点番号が小さいものを選ぶ。coarse graphの平行辺は重みを加算する。各uncoarsening段階で頂点数差を `maximum_imbalance` 以下へ戻した後にrefinementする。

level `l` の頂点数・辺数を $N_l,E_l$、spectral反復回数を $I$、refinement pass数を $R$ とする。

- 時間計算量: $O(\sum_l(E_l\log E_l + I(N_l+E_l)+R(N_l+E_l)\log N_l))$
- 追加空間計算量: $O(\sum_l(N_l+E_l))$

`coarsest_vertex_count < 2`、実現不能なimbalance制約には `std::invalid_argument` を送出する。`maximum_levels == 0` ではcoarseningを省略する。空グラフを許す。
