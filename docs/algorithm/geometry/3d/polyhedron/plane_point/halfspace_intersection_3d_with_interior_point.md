---
title: 3D Halfspace Intersection with Interior Point (内点指定の三次元半空間交差)
documentation_of: ../../../../../../src/algorithm/geometry/3d/polyhedron/plane_point/halfspace_intersection_3d_with_interior_point.hpp
---

既知の狭義内点を利用し、極双対の凸包から閉半空間の共通部分を構築する。

## API

- `halfspace_intersection_3d_with_interior_point(halfspaces, strict_interior_point)`: `strict_interior_point` を中心とする同次座標の双対凸包から、有界な共通部分の頂点と外向き三角形面を持つ `ConvexPolyhedron3` を返す。半空間の解釈は `halfspace_intersection_3d` と同じである。

## API 別の時間計算量・空間計算量

制約数を $H$、出力サイズを $K$ とする。入力と独立な増分順序のもとで期待時間 $O(H \log H + K)$、期待追加領域 $O(H + K)$。内部の固定シードに対する最悪時間は $O(H^2 \log H + K)$、最悪追加領域は $O(H^2 + K)$。

## 注意点

- `strict_interior_point` は有限で、全制約を狭義に満たさなければならない。違反時や、非有限・零法線の半空間には `std::invalid_argument` を送出する。
- 共通部分が非有界なら `std::domain_error` を送出する。
- 空集合や低次元の共通部分にはこの高速版を使えない。その場合は `halfspace_intersection_3d` を使う。
