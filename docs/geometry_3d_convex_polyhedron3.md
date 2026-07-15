---
title: ConvexPolyhedron3 (三次元凸多面体)
documentation_of: ../src/algorithm/geometry/3d/convex_polyhedron3.hpp
---

## API

- `ConvexPolyhedron3::affine_dimension`: 空集合を-1、点・線分・平面図形・立体を0から3で表す。
- `vertices`: 凸包頂点列。
- `faces`: 外向き反時計回りの三角形面を頂点indexで保持する。2次元の場合は片面の三角形分割である。

## API別の時間計算量・空間計算量

- 各member参照は時間・追加領域$O(1)$。列挙は要素数に比例する。

## 注意点

`affine_dimension < 2`では`faces`は空である。3次元の場合だけ閉じた多面体を表す。
