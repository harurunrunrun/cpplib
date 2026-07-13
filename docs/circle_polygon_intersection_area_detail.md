---
title: Circle-Polygon Intersection Area Internals (円と多角形の共通部分面積の内部関数)
documentation_of: ../src/algorithm/geometry/2d/circle_polygon_intersection_area/detail.hpp
---

## API

- `circle_polygon_intersection_internal::edge_area(a, b, radius)`: 原点中心円と有向辺が寄与する符号付き面積を返す。

## API別の時間計算量・空間計算量

- `edge_area`: 時間・追加領域 $O(1)$。

## 注意点

上位の面積API向け内部関数であり、互換性は保証しない。`radius` は非負、座標と中間演算は有限であることを前提とする。
