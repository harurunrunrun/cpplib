---
title: Kernighan-Lin Graph Partitioning
documentation_of: ../src/approximate/graph/kernighan_lin.hpp
---

重み付き無向グラフのcut重みを局所的に小さくするKernighan–Lin法である。最適解を保証しない。

# kernighan_lin

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

各passで異なるsideから未固定頂点対を選び、累積利得が最大となるswap列のprefixだけを適用する。cut重みが厳密に減らないpassで終了する。swapだけを使うため、指定した初期分割の各sideの頂点数を保存する。初期分割を省略した場合は `balanced_bipartition` を使う。同利得では頂点番号の辞書順を選ぶ。

- 時間計算量: $O(E \log E + P(N^3 \log N+E))$、$P$ は実行pass数
- 追加空間計算量: $O(N+E)$

空グラフを許す。`maximum_passes == 0` では初期分割をそのまま返す。重みを全辺重み和で正規化して利得を計算し、大きな有限重みによる中間計算のoverflowを避ける。
