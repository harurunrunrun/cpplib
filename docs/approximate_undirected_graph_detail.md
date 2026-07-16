---
title: Undirected Graph Normalization (無向グラフ正規化)
documentation_of: ../src/approximate/graph/detail/undirected_graph.hpp
---

## Undirected Graph Normalization (無向グラフ正規化)

辺列から重複のない隣接リストと自己loop情報を作る内部API。

## API

### `UndirectedGraph`

`adjacency[v]` に自己loopを除く隣接頂点、`self_loop[v]` に自己loopの有無を保持する。空間計算量は $O(N+M)$。

### `validate_endpoint(vertex_count, vertex)`

頂点番号が範囲内か検査する。時間・追加空間計算量は $O(1)$。

### `make_undirected_graph(vertex_count, edges)`

各隣接列を昇順かつ一意に正規化する。頂点数を $N$、入力辺数を $M$、次数を $d_v$ として時間計算量は $O(M+\sum_v d_v\log d_v)$、追加空間計算量は $O(N+M)$。

## 注意点

`approximate::graph::detail` の内部APIである。範囲外の端点では `std::out_of_range` を送出する。多重辺は1本にまとめ、自己loopは `adjacency` に入れない。
