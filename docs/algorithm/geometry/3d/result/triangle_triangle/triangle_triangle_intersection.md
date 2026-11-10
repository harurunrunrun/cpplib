---
title: 3D Triangle Intersection (三次元三角形同士の共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/triangle_triangle/triangle_triangle_intersection.hpp
---

## API

- `triangle_triangle_intersection(first, second)`: 2 つの三角形の共通部分を `PlanarIntersection3` として返す。空なら `std::monostate`、1 点なら `Point3`、線分なら `Segment3`、面積をもつなら凸多角形の `Polygon3` になる。

## API別の時間計算量・空間計算量

- 三角形の頂点数は固定なので、時間・追加領域ともに $O(1)$。

## 注意点

- 共面でない場合は、それぞれの三角形と相手の平面との共通部分（点または線分）同士の共通部分を求める。
- 平行で共面でない場合は空を返す。共面の場合は凸多角形クリッピング（Sutherland–Hodgman）を行い、結果の重複点を除いて点・線分・多角形に整理する。
- 退化した三角形には `std::invalid_argument` を送出する。
