---
title: Clustering Common Types and Euclidean Distances (クラスタリング共通型とユークリッド距離)
documentation_of: ../src/approximate/clustering/common.hpp
---

`approximate::clustering`名前空間の密ベクトル共通型と距離関数である。

## `DensePoint<Real>` / `DensePoints<Real>`

それぞれ`std::vector<Real>`とその配列を表すaliasである。クラスタリングAPIでは
全点が同じ正の次元を持ち、浮動小数点座標は有限であることを要求する。

## `squared_euclidean_distance`

```cpp
long double d2 = squared_euclidean_distance(left, right);
```

2点のEuclid距離の二乗を`long double`で返す。次元を$d$とすると時間$O(d)$、
追加領域$O(1)$。次元が異なると`std::invalid_argument`、結果がoverflowすると
`std::overflow_error`を送出する。

## `euclidean_distance`

```cpp
long double d = euclidean_distance(left, right);
```

`squared_euclidean_distance`の平方根を返す。時間$O(d)$、追加領域$O(1)$で、
例外条件も同じである。
