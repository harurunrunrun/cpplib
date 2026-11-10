---
title: Undirected Graph Normalization (無向グラフ正規化)
documentation_of: ../../../../src/approximate/graph/detail/undirected_graph.hpp
---

辺の列から、重複のない隣接リストと自己ループの情報を作る内部 API である。以下、頂点数を $N$、入力の辺数を $M$、頂点 $v$ の次数を $d_v$ とする。

## API

### `UndirectedGraph`

`adjacency[v]` に自己ループを除く隣接頂点を、`self_loop[v]` に自己ループの有無を保持する。空間計算量は $O(N+M)$ である。

### `validate_endpoint(vertex_count, vertex)`

頂点番号が範囲内かを検査する。時間計算量・追加空間計算量はともに $O(1)$ である。

### `make_undirected_graph(vertex_count, edges)`

各隣接リストを昇順かつ重複なしに正規化する。時間計算量は $O(M+\sum_v d_v\log d_v)$、追加空間計算量は $O(N+M)$ である。

## 注意点

- `approximate::graph::detail` の内部 API である。
- 範囲外の端点では `std::out_of_range` を送出する。多重辺は 1 本にまとめ、自己ループは `adjacency` に入れない。
