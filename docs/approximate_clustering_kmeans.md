---
title: k-Means / k-Means++ / Mini-Batch k-Means (k平均法)
documentation_of: ../src/approximate/clustering/kmeans.hpp
---

`approximate::clustering`名前空間で、密な実数ベクトルに対する重心ベースの
クラスタリングを提供する。点集合は`DensePoints<Real>`
（`std::vector<std::vector<Real>>`）で渡す。全点の次元は正で等しく、座標は有限で
なければならない。距離が同じ場合は小さい中心番号を選ぶ。

## `KMeansResult`

- `centers`: `long double`で表した最終中心。
- `labels[i]`: 点`i`を割り当てた中心番号。
- `squared_error`: 全点の中心までの二乗Euclid距離の和（SSE）。
- `iterations`: 実行した更新反復数。

## `lloyd_k_means`

```cpp
auto result = lloyd_k_means(points, initial_centers,
                            maximum_iterations, tolerance);
```

Lloyd法を実行する。空になったクラスタの中心は直前の位置に保持する。割当が変化
しないか、全中心の移動距離が`tolerance`以下になると終了する。初期中心数を
$k$、点数を$n$、次元を$d$、実行反復数を$I$とすると、時間$O((I+1)nd)$、
追加領域$O(nd+kd)$。

## `k_means_plus_plus_centers`

```cpp
auto centers = k_means_plus_plus_centers(points, k, rng);
```

D² samplingで`k`個の相異なる入力点を初期中心に選ぶ。`URBG&`を外から渡すため
seedを固定できる。残りのD²重みがすべて0なら、未選択の最小添字を選ぶ。
時間$O(nkd)$、追加領域$O(n+kd)$。

## `k_means`

```cpp
auto result = k_means(points, k, rng, maximum_iterations, tolerance);
```

k-means++で初期化してLloyd法を行う。時間$O(nkd+(I+1)nd)$、追加領域
$O(nd+kd)$。

## `mini_batch_k_means`

```cpp
auto result = mini_batch_k_means(points, k, batch_size, iterations, rng);
```

k-means++で初期化し、各反復で復元抽出した`batch_size`点をオンライン平均で
更新する。最後に全点を割り当て直す。時間
$O(nkd+\mathrm{iterations}\,\mathrm{batch\_size}\,kd+nkd)$、
追加領域$O(n+kd)$。

全APIで`k`は$1\le k\le n$。不正な次元、非有限座標、負のtolerance、
`batch_size == 0`などでは`std::invalid_argument`を送出する。
距離またはSSEが`long double`の範囲を超える場合は`std::overflow_error`を送出する。
