---
title: Approximate Nearest Neighbor Common API (近似近傍探索共通API)
documentation_of: ../../../src/approximate/nearest_neighbor/ann_common.hpp
---

近似近傍探索のインデックスが共有する返り値型と内部補助関数である。以下、次元を $D$、点数を $N$ とする。

## API

### `AnnNeighbor`

近傍点の登録時の添字 `index` と、クエリからの真の二乗距離 `squared_distance`（`long double`）を保持する。構築・参照の時間計算量・追加空間計算量は $O(1)$ である。

### `detail::neighbor_less(first, second)`

二乗距離、添字の順に比較する。時間計算量・追加空間計算量は $O(1)$ である。

### `detail::validate_ann_point(point)`

全座標が有限であることを検査し、非有限な座標があれば `std::invalid_argument` を送出する。時間計算量は $O(D)$、追加空間計算量は $O(1)$ である。

### `detail::ann_squared_distance(first, second)`

2 点の二乗距離を `long double` で返す。結果が有限でなければ `std::overflow_error` を送出する。時間計算量は $O(D)$、追加空間計算量は $O(1)$ である。

### `detail::exact_knn(points, query, k, excluded = nullopt)`

全点を走査して近い順に `k` 点を返す。`excluded` を指定すると、その添字を候補から除く。時間計算量は $O(ND+N\log N)$、追加空間計算量は $O(N)$ である。

### `detail::sort_unique_indices(indices)`

添字列を昇順に並べて重複を除く。要素数を $N$ として時間計算量は $O(N\log N)$、追加空間計算量はソートの実装に依存し、通常は $O(\log N)$ である。

## 注意点

- `AnnNeighbor` 以外は `detail` の内部 API である
- `Dimension > 0` かつ座標型は算術型でなければならない
- `exact_knn` は `k == 0` や利用可能な点数を超える `k` に `std::invalid_argument`、範囲外の除外添字に `std::out_of_range` を送出する
