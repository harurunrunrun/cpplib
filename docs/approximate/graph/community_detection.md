---
title: Weighted Community Detection Common API (重み付きコミュニティ検出共通API)
documentation_of: ../../../src/approximate/graph/community_detection.hpp
---

`approximate::graph` 名前空間のコミュニティ検出で共通に使う結果型と、モジュラリティの計算である。自己ループの辺は無視し、平行辺の重みは加算する。以下、頂点数を $N$、辺数を $E$ とする。

## `CommunityResult`

`label[v]` は頂点 `v` のコミュニティ番号である。番号は、頂点番号順に見て初めて現れた順に 0, 1, ... となるよう正規化される。ほかに、コミュニティ数 `community_count`、重み付き無向グラフのモジュラリティ `modularity`、アルゴリズムが実行した反復回数 `iterations` を持つ。

## `canonical_community_labels`

~~~cpp
vector<size_t> canonical_community_labels(const vector<size_t>& labels)
~~~

同値関係を変えずに、コミュニティ番号を連続な番号に付け直す。

- 時間計算量: $O(N\log N)$
- 追加空間計算量: $O(N)$

## `community_modularity`

~~~cpp
long double community_modularity(
    size_t vertex_count,
    const vector<WeightedUndirectedEdge>& edges,
    const vector<size_t>& labels
)
~~~

$Q=\sum_C(w_C/m-(d_C/(2m))^2)$ を返す。ここで $w_C$ はコミュニティ内部の辺の重みの和、$d_C$ は次数の和、$m$ は全ての辺の重みの和である。辺がない場合は 0 を返す。

- 時間計算量: $O(E\log E+N\log N)$
- 追加空間計算量: $O(N+E)$

## 注意点

- `labels` の長さが頂点数と異なる場合は `std::invalid_argument` を送出する。
- 辺の入力条件（端点の範囲、重みの有限性など）は `weighted_undirected_graph.hpp` の規約に従う。
