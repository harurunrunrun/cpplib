---
title: DBSCAN / Mean Shift (密度ベースクラスタリング)
documentation_of: ../../../src/approximate/clustering/density_clustering.hpp
---

`approximate::clustering` 名前空間で、密な実数ベクトルに対する密度ベースのクラスタリングを提供する。全点の次元は正で等しく、座標は有限でなければならない。

## `DBSCANResult`

- `labels[i]`: 点 `i` のクラスタ番号。ノイズは `-1`。
- `cluster_count`: 見つかったクラスタの個数。

## `dbscan`

```cpp
auto result = dbscan(points, epsilon, minimum_points);
```

点自身を含む閉球（距離 `<= epsilon`）内に `minimum_points` 個以上の点がある点をコア点として DBSCAN を行う。点も近傍も添字順に調べるため結果は決定的で、クラスタ番号は最初のコア点を発見した順になる。空の点集合も受理する。点数を $n$、次元を $d$ とすると、時間計算量は $O(n^2d)$、追加空間計算量は $O(n^2)$ である。

## `MeanShiftResult`

- `modes`: 収束位置を `merge_radius` 以内で連結した成分ごとに併合したモード。
- `labels[i]`: 点 `i` のモード番号。
- `iterations[i]`: 点 `i` の軌道で実行した更新回数。

## `mean_shift`

```cpp
auto result = mean_shift(points, bandwidth, maximum_iterations = 100,
                         tolerance = 1e-9L, merge_radius = -1.0L);
```

一様カーネルを使い、各入力点から独立に mean shift を行う。移動距離が `tolerance` 以下になると停止する。`merge_radius` が負なら `tolerance` を使う。点数 $n$、次元 $d$、最大反復数 $I$ に対し、時間計算量は $O(In^2d+n^2d)$、追加空間計算量は $O(nd+n)$ である。

## 注意点

- 非有限値、負の `epsilon`・`tolerance`、`minimum_points == 0`、次元の不揃いでは `std::invalid_argument` を送出する。
- Mean Shift の `bandwidth` は正の有限値、`merge_radius` は有限値でなければならない。Mean Shift は空の点集合を受理せず、`std::invalid_argument` を送出する。
- 点数が `int` のクラスタ番号で表せない場合、DBSCAN は `std::length_error` を送出する。
