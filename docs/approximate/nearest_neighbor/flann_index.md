---
title: FLANN-style Randomized KD Forest (FLANN風ランダム化KD forest)
documentation_of: ../../../src/approximate/nearest_neighbor/flann_index.hpp
---

## FLANN-style Randomized KD Forest / FLANN風ランダム化KD forest

`FlannIndex<Real, Dimension>` は複数のランダム化KD木を使う近似最近傍索引である。各内部節点では分散上位 `min(Dimension, 5)` 座標から乱数で分割軸を選び、中央値で二分する。探索は全木の未探索branchを下界の小さい順に辿るbest-bin-first方式で、同じ登録点は一度だけ評価する。

`Real` は算術型、`Dimension` は正、点は `std::array<Real, Dimension>` である。同じ点列・同じ乱数器状態なら木と同距離時の添字順は決定的になる。

## API

### `FlannIndex(points, tree_count, leaf_size, random)` / `reset(...)`

`tree_count` 本の木を構築する。木数と葉容量は正でなければならず、非有限座標を拒否する。空の点集合は構築できる。`reset` は構築に成功するまで既存索引を変更しない。

点数を `N`、次元を `D`、木数を `T` とすると、平均時間計算量は `O(TDN log(N+1))`、索引を含む空間計算量は `O(ND+TN)`。

### `nearest(query, k, checks=64)`

最大 `checks` 個の異なる点を真の二乗Euclid距離で評価し、`AnnNeighbor {index, squared_distance}` を距離・添字順で `k` 個返す。`checks >= k` が必要である。候補が `k` 個未満なら全点走査へ切り替える。

平均時間計算量は、処理branch数を `B` として `O(B log B+checks D+checks log checks)`、追加空間計算量は `O(N+B+checks)`。最悪時は全木を走査して `O(TN log(TN)+ND)` となる。`checks` を増やすほど一般にrecallは上がるが、近似比は保証しない。

### `neighbors_of(index, k, checks=64)`

登録点 `index` 自身を除いて検索する。不正添字、`k=0`、利用可能点数を超える `k` を拒否する。計算量は `nearest` と同じ。

### 属性

`size()`, `empty()`, `tree_count()`, `leaf_size()` は `O(1)`、`point(index)` は `O(1)` で元の点を返す。

## 注意点

これはFLANNの自動parameter tuningや複数種類の索引を再現するものではなく、FLANNで使われるrandomized KD forestを独立した汎用APIにしたものだ。距離計算が `long double` の範囲を超える入力は例外になる。
