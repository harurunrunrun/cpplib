---
title: Approximate Nearest Neighbor Common API (近似近傍探索共通API)
documentation_of: ../src/approximate/nearest_neighbor/ann_common.hpp
---

## Approximate Nearest Neighbor Common API (近似近傍探索共通API)

近似近傍探索indexが共有する返値型と内部補助関数。

## API

### `AnnNeighbor`

近傍点の登録時添字 `index` と、queryからの真の二乗距離 `squared_distance` を保持する。構築・参照の時間・追加空間計算量は $O(1)$。

### `detail::neighbor_less(first, second)`

二乗距離、添字の順に比較する。時間・追加空間計算量は $O(1)$。

### `detail::validate_ann_point(point)`

全座標の有限性を検査する。次元を $D$ として時間計算量は $O(D)$、追加空間計算量は $O(1)$。

### `detail::ann_squared_distance(first, second)`

2点の二乗距離を `long double` で返す。時間計算量は $O(D)$、追加空間計算量は $O(1)$。

### `detail::exact_knn(points, query, k, excluded=nullopt)`

全点走査で近い順の `k` 点を返す。`excluded` を指定するとその添字を候補から除く。点数を $N$ として時間計算量は $O(ND+N\log N)$、追加空間計算量は $O(N)$。

### `detail::sort_unique_indices(indices)`

添字列を昇順にして重複を除く。要素数を $N$ として時間計算量は $O(N\log N)$、追加空間計算量はsort実装に依存し通常 $O(\log N)$。

## 注意点

`AnnNeighbor` 以外は `detail` の内部APIである。`Dimension>0` かつ座標型は算術型でなければならない。非有限座標、不正な `k`、不正な除外添字、距離overflowを例外として拒否する。
