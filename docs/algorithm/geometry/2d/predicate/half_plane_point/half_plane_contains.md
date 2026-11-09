---
title: Half-Plane Contains (半平面の点包含判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/predicate/half_plane_point/half_plane_contains.hpp
---

## API

- `bool half_plane_contains(half_plane, point)`: 有向直線 `half_plane.a -> half_plane.b` の左側の閉半平面に点が含まれるか判定する。

## API別の時間計算量・空間計算量

- `half_plane_contains`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 境界線上の点も含む。境界の判定には、方向を単位化した外積に対する許容誤差（`GEOMETRY_EPS` と機械丸め誤差の和）を用いる。
- `half_plane.b - half_plane.a` の長さが `GEOMETRY_EPS` 以下の退化した境界線には `std::invalid_argument` を送出する。
