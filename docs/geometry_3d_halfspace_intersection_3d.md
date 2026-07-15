---
title: 3D Halfspace Intersection (三次元半空間交差)
documentation_of: ../src/algorithm/geometry/3d/halfspace_intersection_3d.hpp
---

平面が表す閉半空間の共通部分を頂点列挙し、有界凸多面体として返す。

## API

- `halfspace_intersection_3d(halfspaces)`: 各 `Plane3{point, normal}` を
  $normal\cdot(x-point)\leq0$ と解釈する。3境界面の交点を列挙し、全制約を満たす
  点の凸包を返す。空集合なら空の `ConvexPolyhedron3` を返し、非空かつ非有界なら
  `std::domain_error` を送出する。

非空性は原点、各境界の原点最近点、2面交線、3面交点から検査する。非有界性は
recession coneの座標軸、法線、2法線の外積からextreme directionを検査する。

## API別の時間計算量・空間計算量

制約数を $H$、得られる頂点数を $V$、凸包構築中の面数を $F$ として、候補列挙と
全制約検査は時間 $O(H^4)$、凸包は時間 $O(VF)$。追加領域は $O(H+V+F)$。

## 注意点

- 法線は非零かつ全座標が有限でなければならない。違反時は
  `std::invalid_argument` を送出する。
- 空集合と非有界集合を区別する。非有界集合のrayを返すAPIではない。
- lower-dimensionalな有界交差も、3面交点として列挙した頂点から構成する。
