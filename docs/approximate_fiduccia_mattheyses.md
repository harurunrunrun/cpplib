---
title: Fiduccia-Mattheyses Graph Partitioning
documentation_of: ../src/approximate/graph/fiduccia_mattheyses.hpp
---

重み付きグラフ用のFiduccia–Mattheyses局所探索である。最適解を保証しない。

# fiduccia_mattheyses

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

各passで、`abs(|side 0|-|side 1|) <= maximum_imbalance` を保てる未固定頂点のうち利得最大の頂点を移す。遅延削除付きpriority queueで重み付き利得を更新し、累積利得が最大となる移動列のprefixを採用する。同利得では頂点番号が小さいものを選ぶ。

- 時間計算量: $O(E \log E + P(N+E)\log N)$、$P$ は実行pass数
- 追加空間計算量: $O(N+E)$

初期分割を省略した場合は平衡分割を使う。指定した初期分割が制約を破る場合、または奇数頂点で `maximum_imbalance == 0` の場合は `std::invalid_argument` を送出する。空グラフを許す。
