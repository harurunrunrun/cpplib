---
title: Divide and Conquer Convex Hull 3D (三次元分割統治凸包)
documentation_of: ../src/algorithm/geometry/3d/divide_and_conquer_convex_hull_3d.hpp
---

## API

- `divide_and_conquer_convex_hull_3d(points)`: 点を辞書順に二分して子凸包を再帰構築し、2個の子凸包を支持面から直接mergeした `ConvexPolyhedron3` を返す。

mergeでは、子の幾何面、および一方の子の幾何辺と他方の子の頂点が張る平面を候補にする。全頂点が片側にある候補だけを残し、同じ支持平面上の点集合を一意化してから二次元凸包順に三角形分割する。このため、merge中に増分法やQuickHullを再実行しない。

## API別の時間計算量・空間計算量

- `divide_and_conquer_convex_hull_3d`: 最悪時間 $O(N^3)$、空間 $O(N^2)$。子凸包の出力が小さい場合は候補数も減る。

## 注意点

重複点を除去し、空集合、1点、共線、共面をそれぞれアフィン次元 $-1,0,1,2$ の凸包として返す。三次元では共面三角形を幾何面ごとにまとめ、境界上の内部点と三角形分割の内部対角線だけに現れる点を最終頂点列から除外する。支持面判定と共線除去には適応的・exact predicateを用いる。非有限座標には `std::invalid_argument` を送出する。
