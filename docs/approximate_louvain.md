---
title: Louvain Community Detection (Louvainコミュニティ検出)
documentation_of: ../src/approximate/graph/louvain.hpp
---

local movingとcommunity縮約を反復する重み付きLouvain法である。最大modularityや一意なcommunityを保証しない。

## louvain_communities

~~~cpp
CommunityResult louvain_communities(
    size_t vertex_count,
    const vector<WeightedUndirectedEdge>& edges,
    uint64_t seed,
    size_t maximum_levels = 20,
    size_t maximum_passes_per_level = 100
)
~~~

各levelでは頂点順をshuffleし、modularityが正に増加する隣接communityのうち増分最大のものへ頂点を移す。局所移動が停止したらcommunityを頂点へ縮約し、community内部辺をself-loop重みとして保持して次levelを処理する。同じ入力・`seed`・標準ライブラリ実装では同じ結果を返す。

level `l` の頂点数・辺数を $N_l,E_l$、実行pass数を $P_l$ とする。

- 時間計算量: $O(E\log E+\sum_l P_l(N_l+E_l)\log N_l+\sum_l E_l\log E_l)$
- 追加空間計算量: $O(N+E)$。縮約前後を同時に保持する一時領域を含む

`maximum_levels == 0` または `maximum_passes_per_level == 0` では各頂点を別communityとして返す。異なるseedでは異なる局所解になり得る。辺がない場合と空グラフを許す。

## 注意点

頂点番号は指定した頂点数の範囲内でなければならない。自己loop・平行辺・重みの扱いは各APIの記載に従い、明記した保証を除いて最適解を返すとは限らない。
