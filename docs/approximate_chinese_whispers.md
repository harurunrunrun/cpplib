---
title: Chinese Whispers Community Detection (Chinese Whispersコミュニティ検出)
documentation_of: ../src/approximate/graph/chinese_whispers.hpp
---

重み付きChinese Whispersによるcommunity detection heuristicである。正解communityや最大modularityを保証しない。

## chinese_whispers

~~~cpp
CommunityResult chinese_whispers(
    size_t vertex_count,
    const vector<WeightedUndirectedEdge>& edges,
    uint64_t seed,
    size_t maximum_iterations = 100
)
~~~

iterationごとに頂点順をshuffleし、隣接辺重み和が最大のclassへ非同期更新する。同点候補は一様乱択する。同じ入力・`seed`・標準ライブラリ実装では同じ結果を返す。孤立頂点は固有classのまま残る。

- 時間計算量: $O(E\log E+I(N+E\log N))$
- 追加空間計算量: $O(N+E)$

`I <= maximum_iterations` である。これは確率的heuristicであり、異なるseedでは異なる局所解になり得る。空グラフを許す。

## 注意点

頂点番号は指定した頂点数の範囲内でなければならない。自己loop・平行辺・重みの扱いは各APIの記載に従い、明記した保証を除いて最適解を返すとは限らない。
