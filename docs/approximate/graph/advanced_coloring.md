---
title: Saturation and Independent-Set Coloring (飽和次数順序と独立集合彩色)
documentation_of: ../../../src/approximate/graph/advanced_coloring.hpp
---

Additional deterministic graph-coloring constructions.
決定的なグラフ彩色構成を `approximate::graph` 名前空間へ追加する。

## `saturation_degree_ordering`

Returns the DSATUR selection order. At each step it maximizes the number of
distinct already-used neighbor colors, then static degree, then minimizes the
vertex index.
各 step で「彩色済み隣接色の種類数」、静的次数、頂点番号の昇順の順に tie-break する DSATUR 順序を返す。

- Time / 時間計算量: \(O(V^2+E\log V)\)
- Extra space / 追加空間計算量: \(O(V+E)\)

The returned vector is a permutation and can be passed to `greedy_coloring`.
返値は頂点置換であり、`greedy_coloring` の `order` にそのまま渡せる。

## `independent_set_coloring`

~~~cpp
ColoringResult independent_set_coloring(
    size_t vertex_count,
    const vector<pair<size_t, size_t>>& edges,
    vector<size_t> priority = {}
)
~~~

Repeatedly constructs a priority-greedy maximal independent set and assigns one
new color to it.
未彩色誘導部分グラフ上で `priority` 順の極大独立集合を繰り返し構成し、集合ごとに新しい1色を割り当てる。

If \(C\) colors are produced:

- Time / 時間計算量: \(O(C(V+E))\)
- Extra space / 追加空間計算量: \(O(V+E)\)

## 注意点

Both functions reject out-of-range endpoints, invalid priority permutations,
and self-loops. Parallel edges are accepted.
端点範囲外、不正な頂点置換、自己 loop には例外を送出し、平行辺は許す。
