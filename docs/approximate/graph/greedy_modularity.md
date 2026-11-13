---
title: Greedy Modularity Community Detection (貪欲モジュラリティコミュニティ検出)
documentation_of: ../../../src/approximate/graph/greedy_modularity.hpp
---

モジュラリティの増分が最大の隣接コミュニティ対を繰り返しマージする、凝集型のヒューリスティックである。最大モジュラリティは保証しない。

## `greedy_modularity`

~~~cpp
CommunityResult greedy_modularity(
    size_t vertex_count,
    const vector<WeightedUndirectedEdge>& edges,
    size_t maximum_merges = numeric_limits<size_t>::max()
)
~~~

最初は各頂点を別のコミュニティとする。マージによるモジュラリティ増分が正の隣接コミュニティ対だけを候補とし、増分最大の対をマージする。増分が等しい場合はコミュニティ番号の組が辞書順最小のものを選ぶ。正の候補がなくなるか、`maximum_merges` 回マージした時点で終了する。

- 時間計算量: $O(E\log E+M(E\log E+N))$。$M$ は実行したマージ回数
- 追加空間計算量: $O(N+E)$

各マージでモジュラリティは厳密に増加するが、得られる局所解は入力順や貪欲選択に依存する。辺がない場合は各頂点を別のコミュニティとして返す。空グラフを許す。

## 注意点

頂点番号は指定した頂点数の範囲内でなければならない。自己ループ・平行辺・重みの扱いは Weighted Undirected Graph Input に従う。
