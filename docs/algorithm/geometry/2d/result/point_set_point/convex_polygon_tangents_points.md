---
title: Convex Polygon Tangents from Points (点列で表した凸多角形への接線)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set_point/convex_polygon_tangents_points.hpp
---

凸多角形の頂点列を正規化してから、外部の点から凸多角形へ引いた左右の接線の接点を求める。

```cpp
#include "../../../../../../src/algorithm/geometry/2d/result/point_set_point/convex_polygon_tangents_points.hpp"
```

## API

```cpp
ConvexPolygonTangentResult convex_polygon_tangents(
    std::vector<Point> polygon,
    const Point& point
);
```

`polygon` は値渡しで受け取り、`NormalizedConvexPolygon` によって向き・重複頂点・共線頂点を正規化する。返り値の添字 `left`, `right` は入力順ではなく、正規化後の頂点列に対する添字である。正規化後の頂点列が空なら、無効な添字を持つ結果を返す。

## 時間計算量

入力点数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `convex_polygon_tangents(polygon, point)` | $O(N)$ | $O(N)$ |

正規化が $O(N)$、正規化後の接線探索が $O(\log N)$ である。

## 注意点

点列は凸多角形の境界を巡回する順に並んでいる必要がある。非凸な点列や自己交差する点列に対しては `std::invalid_argument` を送出する。凸集合が空でない場合、`point` はその外部になければならず、内部または境界上にあれば `std::invalid_argument` を送出する。

## 使用例

```cpp
std::vector<Point> polygon{
    {0, 0}, {4, 0}, {4, 4}, {0, 4},
};
const auto result = convex_polygon_tangents(std::move(polygon), Point{8, 2});
```
