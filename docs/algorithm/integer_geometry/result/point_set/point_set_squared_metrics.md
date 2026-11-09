---
title: Exact Point Set Squared Metrics (厳密点集合距離指標)
documentation_of: ../../../../../src/algorithm/integer_geometry/result/point_set/point_set_squared_metrics.hpp
---

点集合の最近点対距離の二乗と直径の二乗を計算する。

## API

- `closest_pair_squared(points)`: 最近点対の距離の二乗を返す。点が $ 個未満なら `std::nullopt` を返す。分割統治で計算する。
- `convex_diameter_squared(polygon)`: 凸多角形の直径の二乗を回転キャリパー法で返す。空なら `std::nullopt`、$ 点なら /bin/bash$ を返す。全頂点が共線な場合も扱える。
- `point_set_diameter_squared(points)`: 点集合の凸包を取ってから直径の二乗を返す。空なら `std::nullopt` を返す。

戻り値の型はいずれも `std::optional<Rational>` である。

## 時間計算量・空間計算量

点数を $ とする。

- `closest_pair_squared(points)`: 時間 (n \log n)$、追加領域 (n)$。
- `convex_diameter_squared(polygon)`: 時間 (n)$、追加領域 (1)$。
- `point_set_diameter_squared(points)`: 時間 (n \log n)$、追加領域 (n)$。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した計算量は有理数演算の回数であり、各演算には座標のビット長に応じた多倍長整数演算のコストが別途掛かる。

`convex_diameter_squared` の入力は、境界に沿った順に頂点を並べた凸多角形とする。
