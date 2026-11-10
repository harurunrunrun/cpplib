---
title: Point on 3D Ray (三次元半直線上判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/ray_point/on_ray.hpp
---

## API

- `on_ray(ray, point)`: 点 `point` が始点を含む半直線 `ray` 上にあるかを判定する。

## API別の時間計算量・空間計算量

- `on_ray`: 時間・追加領域ともに $O(1)$。

## 注意点

- 退化した半直線には `std::invalid_argument` を送出する。非有限な点にも `std::invalid_argument` を送出する。
- 始点から点へのベクトルが半直線の方向と平行で（`GEOMETRY3D_EPS` による相対許容誤差）、内積が許容誤差の範囲で非負なら `true` とする。
