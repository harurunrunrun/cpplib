---
title: Star Augmented Minimum Cut (星状辺の重み更新と大域最小カット) [global_minimum_cut_of_dynamic_star_augmented_graph]
documentation_of: ../../../../src/algorithm/graph/flow_cut/star_augmented_minimum_cut.hpp
---

固定した非負容量の無向グラフに頂点を1個追加し、元の各頂点との辺を張ったグラフの大域最小カットを管理する。追加頂点に接続した辺の容量を変更できる。

## API

`StarAugmentedMinimumCut(n, edges, star_weights)` を構築する。`edges` は `StarAugmentedCutEdge{from, to, capacity}` の配列。`star_weights[v]` は追加頂点と頂点 `v` の間の容量で、省略時は全て0。

- `set_weight(v, capacity)`: 星状辺の容量を置き換え、更新後の大域最小カット値を返す。
- `minimum_cut()`: 現在の大域最小カット値。
- `minimum_cut_side()`: 最小カットのうち追加頂点を含まない側の元の頂点集合。順序は未規定。

最小次数順の縮約で extreme-set hierarchy を作り、その各部分木のカット容量を管理する。星状辺の更新は階層木上の根へのパス加算になり、HLD と区間加算最小値のセグメント木で処理する。切断された基礎グラフにも対応する。

## 時間計算量

元の頂点数を $N$、辺数を $M$ とする。

- 前処理: $O(N(N+M)\log(N+M))$。
- `set_weight`: $O(\log^2 N)$。
- `minimum_cut`: $O(1)$。
- `minimum_cut_side`: $O(N)$。

## 空間計算量

$O(N+M)$。

## 注意点

`n >= 1`、容量が全て非負、端点が頂点範囲内であることが必要。容量の総和は `long long` に収まらなければならない。自己ループは無視し、多重辺を扱う。条件違反は例外で通知する。元の辺の容量は構築後に変更しない。

階層の構築は [上流の実装](https://github.com/yosupo06/library-checker-problems/blob/04c8de378bab67be926325de2871f0babb8e6451/graph/global_minimum_cut_of_dynamic_star_augmented_graph/sol/correct.cpp) に基づく。Apache-2.0 の全文はヘッダ末尾に記載している。
