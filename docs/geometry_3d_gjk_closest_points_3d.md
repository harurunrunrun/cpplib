---
title: GJK Closest Points 3D (3次元GJK最近点)
documentation_of: ../src/algorithm/geometry/3d/point_collection/gjk_closest_points_3d.hpp
---

## API

- `gjk_closest_points_3d(first, second, tolerance=1e-12L, max_iterations=96)`: 第1・第2形状上の最近点をこの順で返す。

## API別の時間計算量・空間計算量

- $I$ 回反復し、時間 $O(I(V_1+V_2))$、追加領域 $O(1)$。

## 注意点

交差時は共通点を表す1組を返す。最近点が一意でない場合、返る組も一意でない。

返却点は入力と同じ座標系。共通平行移動と一様scaleに対して不変で、点の復元結果が `long double` で表現不能なら `std::overflow_error`。
