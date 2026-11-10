---
title: k-Means / k-Means++ / Hartigan-Wong / Mini-Batch k-Means (k平均法)
documentation_of: ../../../src/approximate/clustering/kmeans.hpp
---

`approximate::clustering` 名前空間で、密な実数ベクトルに対する重心ベースのクラスタリングを提供する。点集合は `DensePoints<Real>`（`std::vector<std::vector<Real>>`）で渡す。全点の次元は正で等しく、座標は有限でなければならない。距離が同じ場合は小さい中心番号を選ぶ。

## `KMeansResult`

- `centers`: `long double` で表した最終的な中心。
- `labels[i]`: 点 `i` を割り当てた中心番号。
- `squared_error`: 全点について、割り当てた中心までのユークリッド距離の二乗を足した値（SSE）。
- `iterations`: 実行した更新の反復回数。

## `lloyd_k_means`

```cpp
auto result = lloyd_k_means(points, initial_centers,
                            maximum_iterations = 100, tolerance = 1e-12L);
```

Lloyd 法を実行する。空になったクラスタの中心は直前の位置に保つ。割当が変化しないか、全ての中心の移動距離が `tolerance` 以下になると終了する。初期中心の個数を $k$、点数を $n$、次元を $d$、実行した反復回数を $I$ とすると、時間計算量は $O((I+1)nkd)$、追加空間計算量は $O(nd+kd)$ である。

## `k_means_plus_plus_centers`

```cpp
auto centers = k_means_plus_plus_centers(points, k, rng);
```

$D^2$ サンプリングで、相異なる `k` 個の入力点を初期中心に選ぶ。乱数生成器 `URBG&` を外から渡すため、シードを固定できる。残りの $D^2$ 重みが全て 0 なら、未選択の点のうち添字が最小のものを選ぶ。時間計算量は $O(nkd)$、追加空間計算量は $O(n+kd)$ である。

## `k_means`

```cpp
auto result = k_means(points, k, rng, maximum_iterations = 100, tolerance = 1e-12L);
```

k-means++ で初期化してから Lloyd 法を行う。時間計算量は $O(nkd+(I+1)nkd)$、追加空間計算量は $O(nd+kd)$ である。

## `hartigan_wong_k_means`

```cpp
auto result = hartigan_wong_k_means(
    points, initial_centers, maximum_passes = 100);
```

初期割当で生じた空クラスタを決定的に修復した後、各点を別のクラスタへ移したときの SSE の差を Hartigan の更新式で評価し、SSE を厳密に減らす最良の移動を反映する。1 パスの中でも、更新後の中心と要素数を直ちに後続の点へ反映する。実行したパス数を $I$ とすると、時間計算量は $O((I+1)nkd)$、追加空間計算量は $O(n+kd)$ である。

## `mini_batch_k_means`

```cpp
auto result = mini_batch_k_means(points, k, batch_size, iterations, rng);
```

k-means++ で初期化し、各反復で復元抽出した `batch_size` 個の点を使ってオンライン平均で中心を更新する。最後に全点を割り当て直す。`iterations` を $T$、`batch_size` を $B$ とすると、時間計算量は $O(nkd+TBkd)$、追加空間計算量は $O(n+kd)$ である。

## 注意点

- 全ての API で $1\le k\le n$ でなければならない。
- 不正な次元、非有限の座標、負の `tolerance`、`batch_size == 0` などでは `std::invalid_argument` を送出する。
- 距離や SSE が `long double` の範囲を超える場合は `std::overflow_error` を送出する。
- 初期値に依存する局所探索であり、大域最適性は保証しない。
