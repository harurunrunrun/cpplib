---
title: Label Propagation Community Detection (ラベル伝播コミュニティ検出)
documentation_of: ../src/approximate/graph/label_propagation.hpp
---

重み付きlabel propagationによるcommunity detection heuristicである。正解communityや最大modularityを保証しない。

# label_propagation

~~~cpp
CommunityResult label_propagation(
    size_t vertex_count,
    const vector<WeightedUndirectedEdge>& edges,
    size_t maximum_iterations = 100
)
~~~

各頂点を頂点番号順に処理し、隣接辺重み和が最大の隣接labelへ非同期更新する。現在labelも最大なら保持し、それ以外の同点では最小labelを選ぶ。変更がないiterationで終了する。孤立頂点は固有communityのまま残る。処理順とtie-breakが固定されているため決定的である。

- 時間計算量: $O(E\log E+I(N+E\log N))$
- 追加空間計算量: $O(N+E)$

`I <= maximum_iterations` である。`maximum_iterations == 0` では各頂点を別communityとして返す。空グラフを許す。

## 注意点

頂点番号は指定した頂点数の範囲内でなければならない。自己loop・平行辺・重みの扱いは各APIの記載に従い、明記した保証を除いて最適解を返すとは限らない。
