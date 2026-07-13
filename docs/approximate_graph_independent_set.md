---
title: Greedy Independent Set (貪欲独立集合)
documentation_of: ../src/approximate/graph/independent_set.hpp
---

無向グラフの極大独立集合を `approximate::graph` 名前空間で求める。自己ループを持つ頂点は選ばない。

# maximal_independent_set

~~~cpp
vector<size_t> maximal_independent_set(
    size_t vertex_count,
    const vector<pair<size_t, size_t>>& edges,
    vector<size_t> order = {}
)
~~~

指定順に、選択済み頂点と隣接しない頂点を加える。空の `order` は頂点番号順を表す。

- 時間計算量: $O(N + E\log N)$
- 追加空間計算量: $O(N+E)$

# greedy_minimum_degree_independent_set

現在の誘導部分グラフで次数最小の頂点を選び、その頂点と近傍を削除する。同次数なら頂点番号が小さい方を選ぶ。

- 時間計算量: $O((N+E)\log N)$
- 追加空間計算量: $O(N+E)$

平行辺は1辺として扱う。不正な順列には `std::invalid_argument`、範囲外の端点には `std::out_of_range` を送出する。

## 注意点

頂点番号は指定した頂点数の範囲内でなければならない。自己loop・平行辺・重みの扱いは各APIの記載に従い、明記した保証を除いて最適解を返すとは限らない。
