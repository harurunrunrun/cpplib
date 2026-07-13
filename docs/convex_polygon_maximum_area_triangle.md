---
title: Convex Polygon Maximum Area Triangle
documentation_of: ../src/algorithm/geometry/convex_polygon_maximum_area_triangle.hpp
---

凸多角形の頂点から選べる最大面積三角形。

```cpp
#include "src/algorithm/geometry/convex_polygon_maximum_area_triangle.hpp"

auto result = convex_polygon_maximum_area_triangle(polygon);
```

## 返り値

```cpp
struct ConvexPolygonMaximumAreaTriangleResult {
    std::array<std::size_t, 3> indices;
    long double doubled_area;
    bool valid() const;
    long double area() const;
};
```

`indices` は正規化後の頂点列に対して昇順である。`doubled_area` は符号なし面積の
2倍で、`area()` は通常の面積を返す。同面積の三角形が複数あれば、そのうち
いずれか1つを返す。

## API

```cpp
ConvexPolygonMaximumAreaTriangleResult
convex_polygon_maximum_area_triangle(
    const NormalizedConvexPolygon& polygon
);

ConvexPolygonMaximumAreaTriangleResult
convex_polygon_maximum_area_triangle(std::vector<Point> polygon);
```

第1・第2頂点を走査し、第3頂点を単調に進める rotating calipers で求める。

## 退化入力

0点、1点、2点、全点 collinear では `valid() == false`、面積0、各添字は
`CONVEX_POLYGON_NPOS` になる。

## 計算量

| API | 時間計算量 | 追加領域 |
| --- | --- | --- |
| 正規化済み overload | $O(N^2)$ | $O(1)$ |
| `vector<Point>` overload | $O(N^2)$ | 正規化に $O(N)$ |
| `valid`, `area` | $O(1)$ | $O(1)$ |
