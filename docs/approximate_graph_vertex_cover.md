---
title: Maximal Matching Vertex Cover (極大マッチングによる頂点被覆)
documentation_of: ../src/approximate/graph/vertex_cover.hpp
---

`approximate::graph` 名前空間にある無向グラフの頂点被覆2近似。

## maximal_matching_vertex_cover / two_approximate_vertex_cover

~~~cpp
VertexCoverResult maximal_matching_vertex_cover(
    size_t vertex_count,
    const vector<pair<size_t, size_t>>& edges
)

VertexCoverResult two_approximate_vertex_cover(...)
~~~

入力順に極大matchingを作り、その各辺の両端を頂点被覆へ加える。自己ループの頂点は先に強制選択する。`two_approximate_vertex_cover` は同じ処理の別名である。結果の頂点数は最小頂点被覆の2倍以下になる。

- 時間計算量: $O(N+E)$
- 追加空間計算量: $O(N+E)$（返却値を含む）

`VertexCoverResult::vertices` は昇順、`matching_edge_indices` は入力辺添字の選択順である。範囲外の端点には `std::out_of_range` を送出する。

## 注意点

頂点番号は指定した頂点数の範囲内でなければならない。自己loop・平行辺・重みの扱いは各APIの記載に従い、明記した保証を除いて最適解を返すとは限らない。
