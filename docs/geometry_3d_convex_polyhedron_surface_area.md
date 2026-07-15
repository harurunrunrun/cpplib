---
title: Convex Polyhedron Surface Area (凸多面体の表面積)
documentation_of: ../src/algorithm/geometry/3d/convex_polyhedron_surface_area.hpp
---

## API

- `convex_polyhedron_surface_area(polyhedron)`: 三角形面の面積を合計して表面積を返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として時間 $O(V+F)$、追加領域 $O(1)$。

## 注意点

非有限頂点・不正添字を拒否する。有限型で表せない結果には `std::overflow_error` を送出する。
