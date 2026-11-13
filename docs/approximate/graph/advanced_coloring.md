---
title: Saturation and Independent-Set Coloring (飽和次数順序と独立集合彩色)
documentation_of: ../../../src/approximate/graph/advanced_coloring.hpp
---

`approximate::graph` 名前空間に、決定的なグラフ彩色の構成法を追加する。以下、頂点数を $V$、辺数を $E$ とする。

## `saturation_degree_ordering`

```cpp
vector<size_t> saturation_degree_ordering(
    size_t vertex_count,
    const vector<pair<size_t, size_t>>& edges
)
```

DSATUR の頂点選択順を返す。各ステップで、彩色済みの隣接頂点の色の種類数が最大の頂点を選び、同数なら静的な次数が大きい頂点、さらに同数なら頂点番号が小さい頂点を選ぶ。

- 時間計算量: $O(V^2+E\log V)$
- 追加空間計算量: $O(V+E)$

戻り値は頂点の順列であり、`greedy_coloring` の `order` にそのまま渡せる。

## `independent_set_coloring`

~~~cpp
ColoringResult independent_set_coloring(
    size_t vertex_count,
    const vector<pair<size_t, size_t>>& edges,
    vector<size_t> priority = {}
)
~~~

未彩色の頂点が誘導する部分グラフ上で、`priority` の順に貪欲に極大独立集合を作ることを繰り返し、集合ごとに新しい 1 色を割り当てる。

使う色数を $C$ とすると、

- 時間計算量: $O(C(V+E))$
- 追加空間計算量: $O(V+E)$

## 注意点

- どちらの関数も、範囲外の端点（`std::out_of_range`）、不正な頂点の順列 `priority`、自己ループ（`std::invalid_argument`）を拒否する。平行辺は受理する。
