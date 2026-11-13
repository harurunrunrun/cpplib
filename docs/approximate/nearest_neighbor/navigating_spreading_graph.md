---
title: Navigating Spreading-out Graph (NSG / 拡散型近傍グラフ)
documentation_of: ../../../src/approximate/nearest_neighbor/navigating_spreading_graph.hpp
---

`NavigatingSpreadingGraph<Real, Dimension>`（短縮名 `NsgIndex`）は、有向の近傍グラフを best-first にたどる近似最近傍インデックスである。全点から正確な近傍候補を作り、相対近傍グラフ（relative neighborhood）の条件で同じ方向の辺を間引く。全点の重心に最も近い点をナビゲーション点とし、そこから全頂点へ到達できるよう、最短の接続辺を追加する。

以下、点数を $N$、次元を $D$ とする。

## API

### `NavigatingSpreadingGraph(points, candidate_count, out_degree)` / `reset(...)`

各点について最大 `candidate_count` 個の近傍を調べ、通常は最大 `out_degree` 本の辺を選ぶ。連結性を修復するための辺により、最終的な次数がこれを超える場合がある。

候補の生成と整列は時間 $O(N^2(D+\log N))$、連結性の修復を含む最悪の時間計算量は $O(N^3D)$ である。空間計算量は候補の作業領域を含めて $O(ND+N^2)$、構築後は辺数を $E$ として $O(ND+E)$ である。

### `nearest(query, k, search_budget = 64)`

ナビゲーション点から、距離の小さい未展開の点を優先して最大 `search_budget` 点を展開する。評価済みの候補を真の二乗距離・添字の順で返す。候補が不足したら全点走査に切り替える。

展開した頂点数を $B$、列挙した辺数を $F$ として、時間計算量は $O(BD+F \log(F+1)+B \log B)$、追加空間計算量は $O(N+F+B)$ である。近似探索の再現率・近似比は保証しない。

### `neighbors_of(index, k, search_budget = 64)`

登録点 `index` 自身を除いて検索する。計算量は `nearest` と同じである。

### 属性

`size()`, `empty()`, `navigation_point()` は $O(1)$ である。`point(index)` と `outgoing(index)` は元の点と出辺を $O(1)$ で参照する。

## 注意点

- `candidate_count == 0`、`out_degree == 0`、非有限な座標、`k == 0`、利用可能な点数を超える `k`、`search_budget < k` には `std::invalid_argument` を送出する
- `neighbors_of` の不正な添字には `std::out_of_range` を送出する
- 大規模データ向けの NSG 構築では近傍候補自体も近似近傍探索で得ることが多い。本実装は外部のインデックスに依存せず結果を再現しやすくするため、候補の構築を正確に行い、検索段階だけを近似している
