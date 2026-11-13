---
title: Greedy Independent Set (貪欲独立集合)
documentation_of: ../../../src/approximate/graph/independent_set.hpp
---

無向グラフの極大独立集合を貪欲に求める（`approximate::graph` 名前空間）。自己ループを持つ頂点は選ばない。

## `maximal_independent_set`

~~~cpp
vector<size_t> maximal_independent_set(
    size_t vertex_count,
    const vector<pair<size_t, size_t>>& edges,
    vector<size_t> order = {}
)
~~~

`order` の順に、選択済みの頂点と隣接しない頂点を加える。空の `order` は頂点番号順を表す。

- 時間計算量: $O(N + E\log N)$
- 追加空間計算量: $O(N+E)$

## `greedy_minimum_degree_independent_set`

~~~cpp
vector<size_t> greedy_minimum_degree_independent_set(
    size_t vertex_count,
    const vector<pair<size_t, size_t>>& edges
)
~~~

現在の誘導部分グラフで次数最小の頂点を選び、その頂点と近傍を削除することを繰り返す。次数が等しければ頂点番号が小さい方を選ぶ。

- 時間計算量: $O((N+E)\log N)$
- 追加空間計算量: $O(N+E)$

## 注意点

- 平行辺は 1 本の辺として扱う
- 空でない `order` が $0, \dots, N-1$ の順列でなければ `std::invalid_argument`、範囲外の端点には `std::out_of_range` を送出する
- 返すのは極大独立集合であり、最大独立集合とは限らない
