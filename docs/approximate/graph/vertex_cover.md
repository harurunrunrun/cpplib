---
title: Maximal Matching Vertex Cover (極大マッチングによる頂点被覆)
documentation_of: ../../../src/approximate/graph/vertex_cover.hpp
---

無向グラフの頂点被覆を 2 近似で求める（`approximate::graph` 名前空間）。

## `maximal_matching_vertex_cover` / `two_approximate_vertex_cover`

~~~cpp
VertexCoverResult maximal_matching_vertex_cover(
    size_t vertex_count,
    const vector<pair<size_t, size_t>>& edges
)

VertexCoverResult two_approximate_vertex_cover(
    size_t vertex_count,
    const vector<pair<size_t, size_t>>& edges
)
~~~

入力順に極大マッチングを作り、その各辺の両端を頂点被覆に加える。自己ループを持つ頂点は先に強制的に選ぶ。`two_approximate_vertex_cover` は同じ処理の別名である。結果の頂点数は最小頂点被覆の 2 倍以下になる。

- 時間計算量: $O(N+E)$
- 追加空間計算量: $O(N+E)$（返り値を含む）

`VertexCoverResult::vertices` は昇順、`matching_edge_indices` は選んだ辺の入力での添字を選択順に並べたものである。

## 注意点

- 範囲外の端点には `std::out_of_range` を送出する
