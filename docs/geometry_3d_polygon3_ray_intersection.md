---
title: Polygon3 Ray Intersection (三次元多角形と半直線の交差)
documentation_of: ../src/algorithm/geometry/3d/polygon3_ray_intersection.hpp
---

## API

- `polygon3_ray_intersection(polygon, ray)`: `origin+t*(through-origin), t>=0` と多角形の全共通区間・孤立点を返す。

## API別の時間計算量・空間計算量

- 頂点数を $N$ として時間 $O(N^2)$、領域 $O(N)$。

## 注意点

非凸多角形の複数区間を保持する。退化半直線には `std::invalid_argument` を送出する。
