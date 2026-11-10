---
title: Convex Polygon Maximum Area Triangle from Points (点列からの凸多角形内最大面積三角形)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set/convex_polygon_maximum_area_triangle_points.hpp
---

凸多角形の頂点列を正規化してから、頂点から選べる最大面積の三角形を求める。

```cpp
#include "../../../../../../src/algorithm/geometry/2d/result/point_set/convex_polygon_maximum_area_triangle_points.hpp"
```

## API

```cpp
ConvexPolygonMaximumAreaTriangleResult
convex_polygon_maximum_area_triangle(std::vector<Point> polygon);
```

点列は値渡しで受け取り、`NormalizedConvexPolygon` によって向き・重複頂点・共線頂点を正規化する。返り値の添字 `indices` は入力順ではなく、正規化後の頂点列に対する添字である。正規化後の頂点数が 3 未満なら無効な結果を返す。面積が等しい候補が複数あるときは、添字の組が辞書順最小のものを選ぶ。

## 時間計算量

入力点数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `convex_polygon_maximum_area_triangle(polygon)` | $O(N^2)$ | $O(N)$ |

## 注意点

入力は凸多角形の境界を巡回する順に並んでいる必要がある。自己交差する点列や非凸な点列は受理せず、`NormalizedConvexPolygon` の検査で `std::invalid_argument` を送出する。座標と中間演算の値は有限な `long double` の範囲に収まる必要がある。

## 使用例

```cpp
std::vector<Point> polygon{
    {0, 0}, {4, 0}, {0, 3},
};
const auto result = convex_polygon_maximum_area_triangle(std::move(polygon));
```
