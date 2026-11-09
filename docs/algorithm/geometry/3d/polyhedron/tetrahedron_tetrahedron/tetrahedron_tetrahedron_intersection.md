---
title: 3D Tetrahedron Intersection (三次元四面体同士の共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/polyhedron/tetrahedron_tetrahedron/tetrahedron_tetrahedron_intersection.hpp
---

## API

- `tetrahedron_tetrahedron_intersection(first, second)`: 相手に含まれる頂点と、辺と面の交点を列挙し、その凸包として共通部分を `ConvexPolyhedron3` で返す。

## API 別の時間計算量・空間計算量

候補点の数は定数なので、凸包の構築を含めて時間・追加領域ともに $O(1)$。

## 注意点

- `affine_dimension` は、共通部分が空なら $-1$、点なら $0$、線分なら $1$、平面多角形なら $2$、立体なら $3$ である。
- 退化した（体積 0 の）四面体には `std::invalid_argument` を送出する。
