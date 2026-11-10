---
title: Chinese Whispers Community Detection (Chinese Whispersコミュニティ検出)
documentation_of: ../../../src/approximate/graph/chinese_whispers.hpp
---

重み付き Chinese Whispers によるコミュニティ検出のヒューリスティックである。正解のコミュニティや最大のモジュラリティは保証しない。

## `chinese_whispers`

~~~cpp
CommunityResult chinese_whispers(
    size_t vertex_count,
    const vector<WeightedUndirectedEdge>& edges,
    uint64_t seed,
    size_t maximum_iterations = 100
)
~~~

反復ごとに頂点の順序をシャッフルし、各頂点のクラスを、隣接辺の重みの和が最大になるクラスへ非同期に更新する。同点の候補からは一様ランダムに選ぶ。同じ入力・`seed`・標準ライブラリの実装なら同じ結果を返す。孤立頂点は自身のクラスのまま残る。

頂点数を $N$、辺数を $E$、実行した反復回数を $I$（$I\le$ `maximum_iterations`）とすると、

- 時間計算量: $O(E\log E+I(N+E\log N))$
- 追加空間計算量: $O(N+E)$

## 注意点

- 確率的なヒューリスティックであり、`seed` が異なれば異なる局所解になりうる。
- 空グラフも受理する。頂点番号は `vertex_count` の範囲内でなければならない。
