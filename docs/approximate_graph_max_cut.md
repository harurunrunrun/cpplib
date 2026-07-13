---
title: Random and Greedy Max-Cut
documentation_of: ../src/approximate/graph/max_cut.hpp
---

無向多重グラフのcutを `approximate::graph` 名前空間で構築する。自己ループはcutに数えず、平行辺は個別に数える。

# cut_edge_count

指定した0/1割当てが切る辺数を数える。

- 時間計算量: $O(N+E)$
- 追加空間計算量: $O(1)$

# random_cut

~~~cpp
template<class URBG>
MaxCutResult random_cut(
    size_t vertex_count,
    const vector<pair<size_t, size_t>>& edges,
    URBG& generator,
    double probability_of_one = 0.5
)
~~~

各頂点を独立にside 1へ置く。既定値では期待cut辺数は自己ループを除く辺数の半分である。

- 時間計算量: $O(N+E)$
- 追加空間計算量: $O(N)$

# greedy_max_cut

頂点番号順に、既に割り当てた隣接辺を多く切る側へ置く。同数ならside 0を選ぶ。自己ループを除く辺の半分以上を決定的に切る。

- 時間計算量: $O(N+E)$
- 追加空間計算量: $O(N+E)$

端点範囲、sideの長さ・値、確率を検査する。不正値には `std::out_of_range` または `std::invalid_argument` を送出する。
