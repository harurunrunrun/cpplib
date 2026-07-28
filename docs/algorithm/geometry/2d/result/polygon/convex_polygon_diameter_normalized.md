---
title: Convex Polygon Diameter for Normalized Polygon (正規化済み凸多角形の直径)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/polygon/convex_polygon_diameter_normalized.hpp
---

正規化済み凸多角形の最遠点対と直径を求める。

```cpp
#include "../../../../../../src/algorithm/geometry/2d/result/polygon/convex_polygon_diameter_normalized.hpp"
```

## API

```cpp
ConvexPolygonDiameterResult convex_polygon_diameter(
    const NormalizedConvexPolygon& polygon
);
```

返り値の `first`, `second` は `polygon.vertices()` に対する添字であり、
`squared_distance` はその2点間の距離の二乗である。複数の最遠点対がある場合は
そのうち1組を返す。

## 退化入力

- 0点: 無効添字と距離0を返す。
- 1点: `(first, second) == (0, 0)` と距離0を返す。
- 2点または全点が一直線上: 正規化後の両端を返す。

## 時間計算量

頂点数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `convex_polygon_diameter(polygon)` | $O(N)$ | $O(N)$ |

各頂点の最遠頂点を暗黙totally monotone matrix上のSMAWKで求め、その最大を選ぶ。

## 注意点

入力は `NormalizedConvexPolygon` の契約を満たす必要がある。座標と中間演算は
有限な `long double` の範囲に収まる必要がある。

## 使用例

```cpp
const std::vector<Point> vertices{
    {0, 0}, {3, 0}, {3, 2}, {0, 2},
};
NormalizedConvexPolygon polygon(vertices);
const auto result = convex_polygon_diameter(polygon);
```
