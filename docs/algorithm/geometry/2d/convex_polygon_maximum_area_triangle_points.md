---
title: Convex Polygon Maximum Area Triangle from Points (点列からの凸多角形内最大面積三角形)
documentation_of: ../../../../src/algorithm/geometry/2d/convex_polygon_maximum_area_triangle_points.hpp
---

点列を正規化してから、頂点から選べる最大面積三角形を求める。

```cpp
#include "src/algorithm/geometry/2d/convex_polygon_maximum_area_triangle_points.hpp"
```

## API

```cpp
ConvexPolygonMaximumAreaTriangleResult
convex_polygon_maximum_area_triangle(std::vector<Point> polygon);
```

点列は値渡しされ、`NormalizedConvexPolygon` により向き、重複頂点、共線頂点を
正規化する。返り値の添字は入力順ではなく、正規化後の頂点列に対応する。

## 時間計算量

入力点数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `convex_polygon_maximum_area_triangle(polygon)` | $O(N^2)$ | $O(N)$ |

## 注意点

入力は凸多角形の境界を巡回する順に並んでいる必要がある。自己交差または非凸な
点列は受理せず、`NormalizedConvexPolygon` の検査で `std::invalid_argument` を
送出する。座標と中間演算は有限な `long double` の範囲に収まる必要がある。

## 使用例

```cpp
std::vector<Point> polygon{
    {0, 0}, {4, 0}, {0, 3},
};
const auto result = convex_polygon_maximum_area_triangle(std::move(polygon));
```
