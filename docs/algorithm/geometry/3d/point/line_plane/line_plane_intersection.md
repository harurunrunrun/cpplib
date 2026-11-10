---
title: 3D Line-Plane Intersection (三次元直線と平面の交点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/line_plane/line_plane_intersection.hpp
---

## API

- `line_plane_intersection(line, plane)`: 無限直線と平面の一意な交点を返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 直線が平面と平行（直線全体が平面上にある場合を含む）なら `std::domain_error` を送出する。平行判定は exact dyadic 演算による厳密判定である。
- 退化直線（`line.a == line.b`）、零法線、非有限入力には `std::invalid_argument` を送出する。
- 交点座標は exact dyadic な分子・分母から一度だけ `long double` へ丸める。真の交点を表現できない場合は `std::overflow_error` を送出する。
