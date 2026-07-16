---
title: Polygon3 Ray Intersection (三次元多角形と半直線の交差)
documentation_of: ../src/algorithm/geometry/3d/polygon3_ray_intersection.hpp
---

## API

- `polygon3_ray_intersection(polygon, ray)`:
  `origin + t * (through - origin)`, $t\geq 0$ と平面多角形の全共通区間・孤立点を
  `Polygon3LinearIntersection3` で返す。

## API別の時間計算量・空間計算量

頂点数を $N$ として、半直線の支持直線が多角形の平面を横切る場合は時間 $O(N)$、
平面上にある場合は最悪時間 $O(N\log N)$。追加領域はいずれも $O(N)$。
返り値の `empty()` は $O(1)$。

## 注意点

非凸多角形の複数区間を保持し、半直線の始点を含む。退化半直線には
`std::invalid_argument` を送出する。
