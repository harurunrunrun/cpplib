---
title: Greedy Modularity Community Detection (貪欲モジュラリティコミュニティ検出)
documentation_of: ../src/approximate/graph/greedy_modularity.hpp
---

modularity増分が最大の隣接community対を反復してmergeするagglomerative heuristicである。最大modularityを保証しない。

## greedy_modularity

~~~cpp
CommunityResult greedy_modularity(
    size_t vertex_count,
    const vector<WeightedUndirectedEdge>& edges,
    size_t maximum_merges = numeric_limits<size_t>::max()
)
~~~

最初は各頂点を別communityとする。mergeによるmodularity増分が正の隣接community対だけを候補とし、最大増分の対をmergeする。同増分ではcommunity番号の辞書順を選ぶ。正の候補がない場合、または `maximum_merges` 回で終了する。

- 時間計算量: $O(E\log E+M(E\log E+N))$、$M$ は実行merge数
- 追加空間計算量: $O(N+E)$

各mergeでmodularityを厳密に増加させるが、得られる局所解は入力順やgreedy選択に依存する。辺がない場合は各頂点を別communityとして返す。空グラフを許す。

## 注意点

頂点番号は指定した頂点数の範囲内でなければならない。自己loop・平行辺・重みの扱いは各APIの記載に従い、明記した保証を除いて最適解を返すとは限らない。
