---
title: Convex Polygon Tangents for Normalized Polygon (正規化済み凸多角形への接線)
documentation_of: ../../../../src/algorithm/geometry/2d/convex_polygon_tangents_normalized.hpp
---

外点から正規化済み凸多角形へ引いた左右の接線の接点を求める。

```cpp
#include "src/algorithm/geometry/2d/convex_polygon_tangents_normalized.hpp"
```

## API

```cpp
ConvexPolygonTangentResult convex_polygon_tangents(
    const NormalizedConvexPolygon& polygon,
    const Point& point
);
```

`vertices = polygon.vertices()` とする。返り値は全頂点 `p` に対して次を満たす。

```cpp
cross(vertices[result.left] - point, p - point) >= 0
cross(vertices[result.right] - point, p - point) <= 0
```

支持直線が辺と重なる場合は、条件を満たす端点のうち小さい添字を返す。

## 退化入力

- 0点: 両添字が無効な結果を返す。
- 1点: `left == right == 0` を返す。
- 2点または全点が一直線上: 通常は両端を返す。外点が支持直線上にあり両端が
  同値なら、`left == right == 0` を返す。

## 時間計算量

頂点数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `convex_polygon_tangents(polygon, point)` | $O(\log N)$ | $O(1)$ |

外点判定と左右接点探索に凸性を利用した二分探索を用いる。

## 注意点

空でない凸集合では `point` が集合の外部にある必要がある。内部または境界上なら
`std::invalid_argument` を送出する。入力は `NormalizedConvexPolygon` の契約を
満たし、座標と中間演算が有限な `long double` の範囲に収まる必要がある。

## 使用例

```cpp
const std::vector<Point> vertices{
    {0, 0}, {4, 0}, {4, 4}, {0, 4},
};
NormalizedConvexPolygon polygon(vertices);
const auto result = convex_polygon_tangents(polygon, Point{8, 2});
```
