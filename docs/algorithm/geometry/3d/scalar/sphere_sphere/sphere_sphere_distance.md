---
title: 3D Sphere-Sphere Distance (三次元球面間距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/sphere_sphere/sphere_sphere_distance.hpp
---

## 概要

2 つの三次元球面の間の最短ユークリッド距離を求める。球面同士が交わるか接する場合は $0$、互いに離れている場合は外側の隙間、一方が他方の内部にあって球面同士が交わらない場合は内側の隙間を返す。

## API

- `sphere_sphere_distance(first, second)`: `first` と `second` の球面間の最短距離を返す。
- `distance(first, second)`: `sphere_sphere_distance(first, second)` と同じ値を返す。

## API別の時間計算量・空間計算量

- `sphere_sphere_distance(first, second)`: 時間 $O(1)$、追加領域 $O(1)$。
- `distance(first, second)`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 各球は中心座標と半径が有限、かつ半径が非負でなければならず、違反時は `std::invalid_argument` を送出する。
- 中心座標と半径を共通スケールで正規化してから計算するため、有限な入力同士の座標差や中心間距離が中間計算でオーバーフローすることはない。返り値が `long double` で表現できない場合は `std::overflow_error` を送出する。
