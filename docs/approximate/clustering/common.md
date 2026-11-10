---
title: Clustering Common Types and Euclidean Distances (クラスタリング共通型とユークリッド距離)
documentation_of: ../../../src/approximate/clustering/common.hpp
---

`approximate::clustering` 名前空間の、密ベクトルの共通型と距離関数である。

## `DensePoint<Real>` / `DensePoints<Real>`

それぞれ `std::vector<Real>` と、その `std::vector` を表す別名である。クラスタリングの各 API は、全点が同じ正の次元を持ち、浮動小数点の座標が有限であることを要求する。

## `squared_euclidean_distance`

```cpp
long double d2 = squared_euclidean_distance(left, right);
```

2 点間のユークリッド距離の二乗を `long double` で返す。次元を $d$ とすると、時間計算量は $O(d)$、追加空間計算量は $O(1)$ である。

## `euclidean_distance`

```cpp
long double d = euclidean_distance(left, right);
```

`squared_euclidean_distance` の平方根を返す。時間計算量は $O(d)$、追加空間計算量は $O(1)$ である。

## 注意点

- どちらの関数も、次元が異なる場合は `std::invalid_argument`、結果がオーバーフローした場合は `std::overflow_error` を送出する。
