---
title: Convex Polyhedron Contains (凸多面体の点包含)
documentation_of: ../src/algorithm/geometry/3d/predicate/convex_polyhedron_contains.hpp
---

## API

- `convex_polyhedron_contains(polyhedron, point)`: 境界を含め、点が凸形状に含まれるか返す。退化次元にも対応する。

## API別の時間計算量・空間計算量

- 面数を $F$ として時間 $O(F)$、追加領域 $O(1)$。

## 注意点

3次元では面が外向きであることを前提とし、判定には適応的 `orient3d` を使う。非有限queryを拒否する。
