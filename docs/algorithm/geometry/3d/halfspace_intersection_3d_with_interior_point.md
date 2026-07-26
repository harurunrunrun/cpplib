---
title: 3D Halfspace Intersection with Interior Point (内点指定の三次元半空間交差)
documentation_of: ../../../../src/algorithm/geometry/3d/halfspace_intersection_3d_with_interior_point.hpp
---

既知の狭義内点を利用し、極双対凸包から閉半空間の共通部分を構築する。

## API

- `halfspace_intersection_3d_with_interior_point(halfspaces, interior)`:
  `interior` を中心とする同次座標の双対凸包から、有界交差の頂点と
  外向き三角形面を持つ `ConvexPolyhedron3` を返す。

## API別の時間計算量・空間計算量

制約数を $H$、出力サイズを $K$ とする。入力独立な増分順序では期待時間
$O(H\log H+K)$、期待追加領域 $O(H+K)$。固定seedに対する最悪時間は
$O(H^2\log H+K)$、最悪追加領域は $O(H^2+K)$。

## 注意点

`interior` は有限で全制約を狭義に満たさなければならず、違反時は
`std::invalid_argument`。交差が非有界なら `std::domain_error`。
空集合またはlower-dimensionalな交差にはこの高速leafを使えない。
