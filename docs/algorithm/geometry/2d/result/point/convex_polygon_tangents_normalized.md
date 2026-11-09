---
title: Convex Polygon Tangents for Normalized Polygon (正規化済み凸多角形への接線)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point/convex_polygon_tangents_normalized.hpp
---

外部の点から正規化済み凸多角形へ引いた左右の接線の接点を求める。

```cpp
#include "../../../../../../src/algorithm/geometry/2d/result/point/convex_polygon_tangents_normalized.hpp"
```

## API

```cpp
ConvexPolygonTangentResult convex_polygon_tangents(
    const NormalizedConvexPolygon& polygon,
    const Point& point
);
```

`vertices = polygon.vertices()` とする。戻り値の `left`, `right` は `vertices` の添字であり、全頂点 `p` に対して次を満たす。

```cpp
cross(vertices[result.left] - point, p - point) >= 0
cross(vertices[result.right] - point, p - point) <= 0
```

支持直線が辺と重なる場合は、条件を満たす端点のうち添字の小さい方を返す。

## 退化入力

- 0 点: 両方の添字が無効値（`std::numeric_limits<std::size_t>::max()`）の結果を返す。
- 1 点: `left == right == 0` を返す。
- 2 点（全点が一直線上の場合を含む）: 通常は両端を返す。外部の点が両端を通る直線上にあり、両端が条件を同等に満たすなら `left == right == 0` を返す。

## 時間計算量

頂点数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `convex_polygon_tangents(polygon, point)` | $O(\log N)$ | $O(1)$ |

外部の点かどうかの判定と、左右の接点の探索に、凸性を利用した二分探索を用いる。

## 注意点

- 空でない凸集合では、`point` は集合の外部になければならない。内部または境界上なら `std::invalid_argument` を送出する。
- 入力は `NormalizedConvexPolygon` の契約を満たし、座標と中間演算は有限な `long double` の範囲に収まる必要がある。

## 使用例

```cpp
const std::vector<Point> vertices{
    {0, 0}, {4, 0}, {4, 4}, {0, 4},
};
NormalizedConvexPolygon polygon(vertices);
const auto result = convex_polygon_tangents(polygon, Point{8, 2});
```
