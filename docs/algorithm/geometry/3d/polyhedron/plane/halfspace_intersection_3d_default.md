---
title: 3D Halfspace Intersection without Interior Point (内点不要の三次元半空間交差)
documentation_of: ../../../../../../src/algorithm/geometry/3d/polyhedron/plane/halfspace_intersection_3d_default.hpp
---

閉半空間の共通部分を、狭義内点の探索を含めて有界凸多面体として構築する。

## API

- `halfspace_intersection_3d(halfspaces)`: `Plane3{point, normal}` を
  $normal\cdot(x-point)\leq0$ と解釈する。空集合なら空の
  `ConvexPolyhedron3` を返し、非空かつ非有界なら
  `std::domain_error` を送出する。退化した有界交差も返せる。

## API別の時間計算量・空間計算量

制約数を $H$、出力サイズを $K$ とする。full-dimensionalな非空入力では
期待時間 $O(H\log H+K)$、期待追加領域 $O(H+K)$。空集合・退化入力・
表現不能な中間射影を処理する完全経路は最悪時間 $O(H^4+K)$、
追加領域 $O(H+K)$。exact整数演算を一回と数える。

## 注意点

法線は非零、全座標は有限でなければならない。入力不正は
`std::invalid_argument`、必須結果を `long double` で表せない場合は
`std::overflow_error`。非有界集合のrayは返さない。
