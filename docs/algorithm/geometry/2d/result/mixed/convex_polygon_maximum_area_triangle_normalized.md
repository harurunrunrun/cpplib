---
title: Convex Polygon Maximum Area Triangle for Normalized Polygon (正規化済み凸多角形内の最大面積三角形)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/mixed/convex_polygon_maximum_area_triangle_normalized.hpp
---

正規化済み凸多角形の頂点から選べる最大面積三角形を求める。

```cpp
#include "../../../../../../src/algorithm/geometry/2d/result/mixed/convex_polygon_maximum_area_triangle_normalized.hpp"
```

## API

```cpp
ConvexPolygonMaximumAreaTriangleResult
convex_polygon_maximum_area_triangle(
    const NormalizedConvexPolygon& polygon
);
```

戻り値の `indices` は `polygon.vertices()` に対する昇順の添字 3 つ、`doubled_area` は面積の 2 倍である。`area()` で面積、`valid()` で有効な結果かを取得できる。面積が等しい候補が複数ある場合は、`indices` が辞書順で最小のものを返す。

## 退化入力

頂点数が $3$ 未満の場合（正規化によって $3$ 頂点未満になる共線な入力を含む）は、`valid() == false`、`doubled_area == 0` の結果を返す。

## 時間計算量

頂点数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `convex_polygon_maximum_area_triangle(polygon)` | $O(N^2)$ | $O(1)$ |

第 1・第 2 頂点を全通り走査し、第 3 頂点を単調に進める rotating calipers を用いる。

## 注意点

入力は `NormalizedConvexPolygon` の契約を満たす必要がある。座標と中間演算は有限な `long double` の範囲に収まる必要がある。

## 使用例

```cpp
const std::vector<Point> vertices{
    {0, 0}, {4, 0}, {0, 3},
};
NormalizedConvexPolygon polygon(vertices);
const auto result = convex_polygon_maximum_area_triangle(polygon);
```
