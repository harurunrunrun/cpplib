---
title: Louvain Community Detection (Louvainコミュニティ検出)
documentation_of: ../../../src/approximate/graph/louvain.hpp
---

局所移動とコミュニティの縮約を繰り返す、重み付きの Louvain 法である。最大モジュラリティや一意なコミュニティは保証しない。

## `louvain_communities`

~~~cpp
CommunityResult louvain_communities(
    size_t vertex_count,
    const vector<WeightedUndirectedEdge>& edges,
    uint64_t seed,
    size_t maximum_levels = 20,
    size_t maximum_passes_per_level = 100
)
~~~

各レベルでは頂点順をシャッフルし、モジュラリティが正に増加する隣接コミュニティのうち増分最大のものへ頂点を移す。局所移動が停止したらコミュニティを頂点に縮約し、コミュニティ内部の辺を自己ループの重みとして保持して次のレベルを処理する。縮約後のコミュニティ数が 1 以下になったら終了する。同じ入力・`seed`・標準ライブラリ実装なら同じ結果を返す。

レベル $l$ の頂点数・辺数を $N_l, E_l$、実行パス数を $P_l$ とする。

- 時間計算量: $O(E\log E+\sum_l P_l(N_l+E_l)\log N_l+\sum_l E_l\log E_l)$
- 追加空間計算量: $O(N+E)$。縮約前後のグラフを同時に保持する一時領域を含む

## 注意点

- `maximum_levels == 0` または `maximum_passes_per_level == 0` では各頂点を別のコミュニティとして返す
- 辺がない場合と空グラフを許す
- `seed` が異なると異なる局所解になり得る
- パス数の合計が `size_t` で表現できなくなると `std::overflow_error` を送出する
- 頂点番号は指定した頂点数の範囲内でなければならない。自己ループ・平行辺・重みの扱いは Weighted Undirected Graph Input に従う
