---
title: Convex Polygon Minimum Width from Points (頂点列からの凸多角形最小幅)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/point_set/convex_polygon_minimum_width_points.hpp
---

凸多角形を周回する頂点列を正規化し、最小幅（多角形を挟む平行な 2 直線間の距離の最小値）を求める。

## API

```cpp
long double convex_polygon_minimum_width(std::vector<Point> polygon);
```

入力を `NormalizedConvexPolygon` に変換し、正規化済みの多角形を受け取る多重定義へ委譲する。

## 時間計算量

入力点数を $N$ とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `convex_polygon_minimum_width(polygon)` | $O(N)$ | $O(N)$ |

## 注意点

入力は凸多角形を周回する弱凸な頂点列でなければならない。非凸な入力には `std::invalid_argument` を送出する。点または線分に退化する入力では 0 を返す。
