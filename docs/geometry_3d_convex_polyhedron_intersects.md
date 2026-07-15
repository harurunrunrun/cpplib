---
title: Convex Polyhedron Intersects (凸多面体同士の交差判定)
documentation_of: ../src/algorithm/geometry/3d/convex_polyhedron_intersects.hpp
---

## API

- `convex_polyhedron_intersects(first, second)`: 境界接触を含め共通部分が空でないか返す。

## API別の時間計算量・空間計算量

- 共通部分を構成するため、時間・領域は `convex_polyhedron_intersection` と同じ。

## 注意点

高速な判定だけが必要ならSAT/GJK版を使う。入力前提と例外は共通部分構成版に従う。
