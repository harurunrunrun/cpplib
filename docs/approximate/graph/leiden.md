---
title: Leiden Community Detection (Leidenコミュニティ検出)
documentation_of: ../../../src/approximate/graph/leiden.hpp
---

重み付き無向グラフに対する Leiden 型のコミュニティ検出である。Louvain 法の単なる別名ではなく、各レベルで局所移動後のコミュニティを連結成分に細分（refinement）してから縮約する。

## `LeidenResult`

- `labels[v]`: 頂点 `v` のコミュニティ番号。頂点順に `0, 1, ...` へ正規化される
- `community_count`: コミュニティ数
- `modularity`: 返す分割の、入力グラフに対するモジュラリティ
- `iterations`: 全レベルで実行した局所移動パス数の合計
- `levels`: 実行したレベル数

各フィールドの参照は時間 $O(1)$、`labels` の走査は時間 $O(N)$ である。

## `leiden_communities`

~~~cpp
LeidenResult leiden_communities(
    size_t vertex_count,
    const vector<WeightedUndirectedEdge>& edges,
    uint64_t seed,
    size_t maximum_levels = 20,
    size_t maximum_passes_per_level = 100
)
~~~

各レベルで次を行う。

1. 頂点順をシャッフルし、モジュラリティ増分が正で最大の隣接コミュニティへ頂点を移す
2. 得られた各コミュニティを、そのコミュニティが誘導する正重み辺の部分グラフの連結成分に分割する
3. 細分後の各コミュニティを 1 頂点に縮約し、内部辺の重みを自己ループの重みとして次のレベルへ渡す

したがって、返す各コミュニティは入力の正重み辺で連結であることが保証される。正の移動増分が等しい場合はコミュニティ番号が最小のものを選ぶ。`seed` が異なると別の局所解になり得る。細分後のコミュニティ数が 1 以下になったら終了する。

レベル $l$ の頂点数・辺数を $N_l, E_l$、実行パス数を $P_l$ とすると、

- 時間計算量: $O(E \log E + \sum_l (P_l (N_l + E_l) \log N_l + E_l \log E_l))$
- 追加空間計算量: $O(N + E)$

細分自体は各レベルで $O(N_l + E_l)$ であり、上式に含まれる。

## 注意点

- 辺重みは有限な非負値でなければならない
- 重み 0 の辺と自己ループは無視し、同じ無向辺の重みは加算する
- 頂点番号が範囲外なら `std::out_of_range` を送出する
- 辺がない場合、空グラフ、`maximum_levels == 0`、`maximum_passes_per_level == 0` のいずれかでは、各頂点を別のコミュニティとして返す
- `levels <= maximum_levels` である
- `iterations <= levels * maximum_passes_per_level` である。パス数の合計が `size_t` で表現できなくなると `std::overflow_error` を送出する
- 入力と `seed` が同じなら、同じ標準ライブラリ実装のもとで同じ結果を返す
- 局所探索のヒューリスティックであり、モジュラリティの大域最大化や正解コミュニティの復元は保証しない。この実装の細分で保証するのはコミュニティの連結性であり、より強い内部密度の保証はない
