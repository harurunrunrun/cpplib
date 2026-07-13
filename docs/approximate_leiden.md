---
title: Leiden Community Detection
documentation_of: ../src/approximate/graph/leiden.hpp
---

重み付き無向グラフに対するLeiden-style community detectionである。Louvainの単なるaliasではなく、各levelで局所移動後のcommunityを連結成分へrefinementしてから縮約する。

## `LeidenResult`

- `labels[v]`: 頂点`v`のcommunity番号。頂点順に`0,1,...`へ正規化される
- `community_count`: community数
- `modularity`: 入力グラフに対する返却partitionのmodularity
- `iterations`: 全levelで実行したlocal-moving pass数の合計
- `levels`: 実行したlevel数

各fieldの参照は時間`O(1)`、`labels`の走査は時間`O(N)`である。

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

各levelで次を行う。

1. 頂点順をshuffleし、modularity増分が正で最大の隣接communityへ頂点を移す
2. 得られた各communityを、そのcommunityが誘導する正重み辺部分グラフの連結成分へ分割する
3. refined communityを1頂点へ縮約し、内部辺重みをself-loop重みとして次levelへ渡す

したがって、返す各communityが入力の正重み辺で連結であることを保証する。同値の正の移動増分ではcommunity番号最小を選ぶ。異なるseedでは別の局所解になり得る。

level `l`の頂点数・辺数を`N_l, E_l`、実行pass数を`P_l`とすると、

- 時間計算量: `O(E log E + sum_l(P_l (N_l + E_l) log N_l + E_l log E_l))`
- 追加空間計算量: `O(N + E)`

refinement自体は各levelで`O(N_l + E_l)`であり、上式の範囲内である。

### 入力規則と停止条件

- 辺重みは有限な非負値でなければならない
- zero-weight辺とself-loopは無視し、同じ無向辺の重みは加算する
- 頂点番号が範囲外なら`std::out_of_range`を送出する
- 辺がない場合、空グラフ、`maximum_levels == 0`、または`maximum_passes_per_level == 0`では、各頂点を別communityとして返す
- `levels <= maximum_levels`
- `iterations <= levels * maximum_passes_per_level`（右辺が表現可能な場合）

入力と`seed`が同じなら、同じ標準ライブラリ実装では決定的に同じ結果を返す。局所探索heuristicであり、modularityの大域最大化や正解communityの復元は保証しない。この実装のrefinement保証はcommunityの連結性であり、より強い内部密度を保証するものではない。
