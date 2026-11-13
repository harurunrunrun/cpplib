---
title: Label Propagation Community Detection (ラベル伝播コミュニティ検出)
documentation_of: ../../../src/approximate/graph/label_propagation.hpp
---

重み付きラベル伝播によるコミュニティ検出のヒューリスティックである。正解コミュニティや最大モジュラリティは保証しない。

## `label_propagation`

~~~cpp
CommunityResult label_propagation(
    size_t vertex_count,
    const vector<WeightedUndirectedEdge>& edges,
    size_t maximum_iterations = 100
)
~~~

各頂点を頂点番号順に処理し、隣接辺の重み和が最大となる隣接ラベルへ非同期に更新する。現在のラベルも最大なら保持し、それ以外の同点では最小のラベルを選ぶ。ラベルが変化しない反復で終了する。孤立頂点は固有のコミュニティのまま残る。処理順と同点の扱いが固定されているため決定的である。

- 時間計算量: $O(E\log E+I(N+E\log N))$。$I \le \texttt{maximum_iterations}$ は反復回数
- 追加空間計算量: $O(N+E)$

## 注意点

- `maximum_iterations == 0` では各頂点を別のコミュニティとして返す
- 空グラフを許す
- 頂点番号は指定した頂点数の範囲内でなければならない。自己ループ・平行辺・重みの扱いは Weighted Undirected Graph Input に従う
