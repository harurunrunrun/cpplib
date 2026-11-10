---
title: ConvexPolyhedron3 (三次元凸多面体)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/convex_polyhedron3.hpp
---

## API

- `ConvexPolyhedron3::affine_dimension`: 空集合を $-1$、点・線分・平面図形・立体をそれぞれ $0$ から $3$ で表す。
- `vertices`: 凸包の頂点列（`std::vector<Point3>`）。
- `faces`: 外側から見て反時計回りの三角形面を、頂点インデックスの組（`std::array<std::size_t, 3>`）で保持する。二次元の場合は片面の三角形分割である。

## API別の時間計算量・空間計算量

- 各メンバの参照は時間・追加領域 $O(1)$。列挙は要素数に比例する。

## 注意点

- `affine_dimension < 2` のとき `faces` は空である。閉じた多面体を表すのは三次元の場合だけである。
