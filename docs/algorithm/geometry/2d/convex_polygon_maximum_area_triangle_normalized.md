---
title: Convex Polygon Maximum Area Triangle for Normalized Polygon (正規化済み凸多角形内の最大面積三角形)
documentation_of: ../../../../src/algorithm/geometry/2d/convex_polygon_maximum_area_triangle_normalized.hpp
---

正規化済み凸多角形の頂点から選べる最大面積三角形を求める。

```cpp
#include "src/algorithm/geometry/2d/convex_polygon_maximum_area_triangle_normalized.hpp"
```

## API

```cpp
ConvexPolygonMaximumAreaTriangleResult
convex_polygon_maximum_area_triangle(
    const NormalizedConvexPolygon& polygon
);
```

返り値の `indices` は `polygon.vertices()` に対する昇順添字である。同面積の
候補が複数ある場合はそのうち1つを返す。

## 退化入力

頂点数が3未満、または正規化後に3頂点未満となる共線入力では
`valid() == false`、`doubled_area == 0` を返す。

## 時間計算量

頂点数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `convex_polygon_maximum_area_triangle(polygon)` | $O(N^2)$ | $O(1)$ |

第1・第2頂点を走査し、第3頂点を単調に進めるrotating calipersを用いる。

## 注意点

入力は `NormalizedConvexPolygon` の契約を満たす必要がある。座標と中間演算は
有限な `long double` の範囲に収まる必要がある。

## 使用例

```cpp
const std::vector<Point> vertices{
    {0, 0}, {4, 0}, {0, 3},
};
NormalizedConvexPolygon polygon(vertices);
const auto result = convex_polygon_maximum_area_triangle(polygon);
```
