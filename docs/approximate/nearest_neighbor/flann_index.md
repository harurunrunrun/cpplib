---
title: FLANN-style Randomized KD Forest (FLANN風ランダム化KD forest)
documentation_of: ../../../src/approximate/nearest_neighbor/flann_index.hpp
---

`FlannIndex<Real, Dimension>` は複数のランダム化 KD 木を使う近似最近傍インデックスである。各内部節点では、分散が上位 $\min(\texttt{Dimension}, 5)$ 個の座標から乱数で分割軸を選び、中央値で二分する。探索は全木の未探索の枝を下界の小さい順にたどる best-bin-first 方式で、同じ登録点は 1 度だけ評価する。

`Real` は算術型、`Dimension` は正で、点は `std::array<Real, Dimension>` である。同じ点列・同じ乱数生成器の状態なら、木の形と距離が等しいときの添字順は決定的になる。

以下、点数を $N$、次元を $D$、木の本数を $T$ とする。

## API

### `FlannIndex(points, tree_count, leaf_size, random)` / `reset(...)`

`tree_count` 本の木を構築する。`tree_count` と葉の容量 `leaf_size` は正でなければならず、非有限な座標は拒否する。空の点集合でも構築できる。`reset` は構築に成功するまで既存のインデックスを変更しない。

平均時間計算量は $O(TDN \log(N+1))$、インデックスを含む空間計算量は $O(ND+TN)$ である。

### `nearest(query, k, checks = 64)`

最大 `checks` 個の相異なる点を真の二乗ユークリッド距離で評価し、`AnnNeighbor { index, squared_distance }` を距離・添字の順に `k` 個返す。`checks >= k` でなければならない。候補が `k` 個未満なら全点走査に切り替える。

処理した枝の数を $B$、$C = \texttt{checks}$ として、平均時間計算量は $O(B \log B+CD+C \log C)$、追加空間計算量は $O(N+B+C)$ である。最悪時は全木を走査して $O(TN \log(TN)+ND)$ となる。`checks` を増やすほど一般に再現率は上がるが、近似比は保証しない。

### `neighbors_of(index, k, checks = 64)`

登録点 `index` 自身を除いて検索する。計算量は `nearest` と同じである。

### 属性

`size()`, `empty()`, `tree_count()`, `leaf_size()` は $O(1)$、`point(index)` は $O(1)$ で元の点を返す。

## 注意点

- `tree_count == 0`、`leaf_size == 0`、非有限な座標、`k == 0`、利用可能な点数を超える `k`、`checks < k` には `std::invalid_argument` を送出する
- `neighbors_of` の不正な添字には `std::out_of_range` を送出する
- 距離の計算が `long double` の範囲を超える入力には `std::overflow_error` を送出する
- FLANN の自動パラメータ調整や複数種類のインデックスを再現するものではなく、FLANN で使われるランダム化 KD forest を独立した汎用 API にしたものである
