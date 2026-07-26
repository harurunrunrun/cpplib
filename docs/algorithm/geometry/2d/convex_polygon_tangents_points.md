---
title: Convex Polygon Tangents from Points (点列で表した凸多角形への接線)
documentation_of: ../../../../src/algorithm/geometry/2d/convex_polygon_tangents_points.hpp
---

点列を正規化してから、外点から凸多角形への左右の接線を求める。

```cpp
#include "src/algorithm/geometry/2d/convex_polygon_tangents_points.hpp"
```

## API

```cpp
ConvexPolygonTangentResult convex_polygon_tangents(
    std::vector<Point> polygon,
    const Point& point
);
```

`polygon` は値渡しされ、向き、重複頂点、共線頂点を正規化する。返り値の添字は
入力順ではなく、正規化後の頂点列に対応する。

## 時間計算量

入力点数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `convex_polygon_tangents(polygon, point)` | $O(N)$ | $O(N)$ |

正規化が $O(N)$、正規化後の接線探索が $O(\log N)$ である。

## 注意点

点列は凸多角形の境界を巡回する順に並んでいる必要がある。非凸または自己交差する
点列は `std::invalid_argument` の対象となる。空でない凸集合では `point` が
集合の外部にある必要があり、内部または境界上なら `std::invalid_argument` を
送出する。

## 使用例

```cpp
std::vector<Point> polygon{
    {0, 0}, {4, 0}, {4, 4}, {0, 4},
};
const auto result = convex_polygon_tangents(std::move(polygon), Point{8, 2});
```
