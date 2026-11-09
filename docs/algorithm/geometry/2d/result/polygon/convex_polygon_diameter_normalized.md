---
title: Convex Polygon Diameter for Normalized Polygon (正規化済み凸多角形の直径)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/polygon/convex_polygon_diameter_normalized.hpp
---

正規化済みの凸多角形について、最遠点対と直径を求める。

```cpp
#include "../../../../../../src/algorithm/geometry/2d/result/polygon/convex_polygon_diameter_normalized.hpp"
```

## API

```cpp
ConvexPolygonDiameterResult convex_polygon_diameter(
    const NormalizedConvexPolygon& polygon
);
```

返り値の `first`, `second`（`first <= second`）は `polygon.vertices()` に対する添字であり、`squared_distance` はその 2 点間の距離の 2 乗である。距離そのものは `distance()` で得られる。最遠点対が複数ある場合は、そのうちの 1 組を返す。

## 退化入力

- 0 点: 無効な添字と距離 0 を返す。
- 1 点: `(first, second) == (0, 0)` と距離 0 を返す。
- 2 点（全点が一直線上の場合も正規化で 2 点になる）: 正規化後の両端点を返す。

## 時間計算量

頂点数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `convex_polygon_diameter(polygon)` | $O(N)$ | $O(N)$ |

各頂点から最も遠い頂点を、暗黙に表した totally monotone な行列上の SMAWK で求め、その中の最大を選ぶ。

## 注意点

入力は `NormalizedConvexPolygon` の契約を満たす必要がある。座標と中間演算の値は有限な `long double` の範囲に収まる必要がある。

## 使用例

```cpp
const std::vector<Point> vertices{
    {0, 0}, {3, 0}, {3, 2}, {0, 2},
};
NormalizedConvexPolygon polygon(vertices);
const auto result = convex_polygon_diameter(polygon);
```
