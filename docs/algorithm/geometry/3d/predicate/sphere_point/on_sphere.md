---
title: Point on 3D Sphere (三次元球面上判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/sphere_point/on_sphere.hpp
---

## API

- `on_sphere(sphere, point)`: 点 `point` が球 `sphere` の球面上にあるかを返す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- `Sphere3` は中心・半径が有限かつ半径が非負でなければならず、違反時は `std::invalid_argument` を送出する。`point` が非有限な場合も `std::invalid_argument` を送出する。
- 半径 0 の球面は中心の 1 点である。
- 中心からの差と半径を共通のスケールで正規化し、中心からの距離と半径の差を `geometry3d_scaled_sign` による相対許容誤差で判定する。
