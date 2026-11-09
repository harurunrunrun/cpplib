---
title: Star Augmented Minimum Cut (星状辺の重み更新と大域最小カット) [global_minimum_cut_of_dynamic_star_augmented_graph]
documentation_of: ../../../../src/algorithm/graph/flow_cut/star_augmented_minimum_cut.hpp
---

非負容量の固定された無向グラフに頂点を 1 個追加し、元の各頂点との間に辺（星状辺）を張ったグラフの大域最小カットを管理する。星状辺の容量を変更できる。

## API

```cpp
struct StarAugmentedCutEdge { int from; int to; long long capacity; };

StarAugmentedMinimumCut(
    int n,
    const std::vector<StarAugmentedCutEdge>& edges,
    std::vector<long long> star_weights = {}
)
```

`star_weights[v]` は追加頂点と頂点 `v` の間の容量であり、省略時（空のとき）はすべて $0$ とする。

- `long long set_weight(int v, long long capacity)`: 頂点 `v` の星状辺の容量を `capacity` に置き換え、更新後の大域最小カットの値を返す。
- `long long minimum_cut() const`: 現在の大域最小カットの値を返す。
- `std::vector<int> minimum_cut_side() const`: 最小カットのうち追加頂点を含まない側に属する元の頂点の集合を返す。順序は未規定である。

最小次数順の縮約で extreme-set hierarchy を作り、その各部分木のカット容量を管理する。星状辺の更新は階層木上の根へのパス加算になり、HLD と区間加算・区間最小値のセグメント木で処理する。基礎グラフが非連結でもよい。

## 時間計算量

元の頂点数を $N$、辺数を $M$ とする。

- 前処理: $O(N (N + M) \log (N + M))$
- `set_weight`: $O(\log^2 N)$
- `minimum_cut`: $O(1)$
- `minimum_cut_side`: $O(N)$

## 空間計算量

$O(N + M)$

## 注意点

- $n \ge 1$ でなければならない。容量はすべて非負で、辺の端点は $[0, n)$ の範囲内でなければならない。`star_weights` は空か長さ $n$ でなければならない。これらの違反時（`set_weight` の引数の違反を含む）は `invalid_argument` を送出する。
- 星状辺を含む容量の総和は `long long` に収まらなければならない。超える場合は構築時・`set_weight` 時に `overflow_error` を送出する。
- 自己ループと容量 $0$ の辺は無視し、多重辺はそのまま扱う。元のグラフの辺の容量は構築後に変更できない。

階層の構築は [上流の実装](https://github.com/yosupo06/library-checker-problems/blob/04c8de378bab67be926325de2871f0babb8e6451/graph/global_minimum_cut_of_dynamic_star_augmented_graph/sol/correct.cpp) に基づく。Apache-2.0 ライセンスの全文はヘッダ末尾に記載している。
