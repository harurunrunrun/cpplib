---
title: Navigating Spreading-out Graph (NSG / 拡散型近傍グラフ)
documentation_of: ../../../src/approximate/nearest_neighbor/navigating_spreading_graph.hpp
---

## Navigating Spreading-out Graph / 拡散型近傍グラフ

`NavigatingSpreadingGraph<Real, Dimension>`（短縮名 `NsgIndex`）は、有向近傍グラフをbest-firstに辿る近似最近傍索引である。全点から正確な近傍候補を作り、relative-neighborhood条件で同方向の辺を間引く。全点の重心に最も近い点をnavigation pointとし、そこから全頂点へ到達できるよう最短の接続辺を追加する。

## API

### `NavigatingSpreadingGraph(points, candidate_count, out_degree)` / `reset(...)`

各点について最大 `candidate_count` 個の近傍を調べ、通常最大 `out_degree` 本を選択する。連結性修復用の辺により最終次数がこれを超える場合がある。両parameterは正でなければならず、非有限座標を拒否する。

点数を `N`、次元を `D` とすると、候補生成と整列は時間 `O(N^2(D+log N))`、連結性修復を含む最悪時間計算量は `O(N^3D)`。空間計算量は候補作業領域を含め `O(ND+N^2)`、構築後は `O(ND+E)`（`E` は辺数）。

### `nearest(query, k, search_budget=64)`

navigation pointから距離の小さい未展開点を優先し、最大 `search_budget` 点を展開する。評価済み候補を真の二乗距離・添字順で返す。候補不足時は全点走査へ切り替える。

展開頂点数を `B`、列挙辺数を `F` として、時間計算量は `O(BD+F log(F+1)+B log B)`、追加空間計算量は `O(N+F+B)`。近似探索のrecall・近似比は保証しない。

### `neighbors_of(index, k, search_budget=64)`

登録点自身を除外して検索する。`search_budget >= k` が必要である。計算量は `nearest` と同じ。

### 属性

`size()`, `empty()`, `navigation_point()` は `O(1)`。`point(index)` と `outgoing(index)` は元の点と出辺を `O(1)` で参照する。

## 注意点

大規模データ向けNSG構築では近傍候補自体もANNで得ることが多い。本実装は外部索引に依存せず結果を再現しやすくするため候補構築を正確に行い、検索段階を近似化している。
