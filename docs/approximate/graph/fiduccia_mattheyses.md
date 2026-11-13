---
title: Fiduccia–Mattheyses Graph Partitioning (Fiduccia–Mattheysesグラフ分割)
documentation_of: ../../../src/approximate/graph/fiduccia_mattheyses.hpp
---

重み付きグラフの二分割に対する Fiduccia–Mattheyses 局所探索である。最適解は保証しない。

## `fiduccia_mattheyses`

~~~cpp
BipartitionResult fiduccia_mattheyses(
    size_t vertex_count,
    const vector<WeightedUndirectedEdge>& edges,
    vector<unsigned char> initial_side,
    size_t maximum_imbalance = 2,
    size_t maximum_passes = 20
)

BipartitionResult fiduccia_mattheyses(
    size_t vertex_count,
    const vector<WeightedUndirectedEdge>& edges,
    size_t maximum_imbalance = 2,
    size_t maximum_passes = 20
)
~~~

各パスで、2 つの側の頂点数の差が `maximum_imbalance` 以下に保たれる未固定の頂点のうち、利得が最大のものを移動する。遅延削除付きの優先度付きキューで重み付きの利得を更新し、累積利得が最大となる移動列の先頭部分を採用する。同じ利得なら頂点番号が小さいものを選ぶ。戻り値の `BipartitionResult` は各頂点の側 `side` とカットの重み `cut_weight` を持つ。

頂点数を $N$、辺数を $E$、実行したパス数を $P$ とすると、

- 時間計算量: $O(E\log E+P(N+E)\log N)$
- 追加空間計算量: $O(N+E)$

初期分割を省略した場合は、平衡な分割から始める。

## 注意点

- 指定した初期分割が不均衡の制約を破る場合、または頂点数が奇数で `maximum_imbalance == 0` の場合は `std::invalid_argument` を送出する。
- 空グラフも受理する。頂点番号は `vertex_count` の範囲内でなければならない。
