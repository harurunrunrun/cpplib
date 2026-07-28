---
title: Convex Polygon Maximum Area Triangle Result (凸多角形内の最大面積三角形の結果)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/convex_polygon_maximum_area_triangle_result.hpp
---

凸多角形内の最大面積三角形を表す結果型。

```cpp
#include "../../../../../../src/algorithm/geometry/2d/type/definition/convex_polygon_maximum_area_triangle_result.hpp"
```

## API

```cpp
struct ConvexPolygonMaximumAreaTriangleResult {
    std::array<std::size_t, 3> indices;
    long double doubled_area;

    bool valid() const;
    long double area() const;
};
```

- `indices`: 選ばれた3頂点の昇順添字。
- `doubled_area`: 符号なし面積の2倍。
- `valid()`: 有効な3頂点が格納されているとき `true`。
- `area()`: `doubled_area / 2` を返す。

既定構築した値では全添字が `std::numeric_limits<std::size_t>::max()`、
`doubled_area` が0となる。

## 時間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| 既定構築、各フィールドの参照 | $O(1)$ | $O(1)$ |
| `valid()`, `area()` | $O(1)$ | $O(1)$ |

## 注意点

この型だけでは頂点列を保持しないため、添字を解釈するには計算に用いた正規化後の
頂点列が必要である。`doubled_area` は非負かつ有限であることを前提とする。

## 使用例

```cpp
ConvexPolygonMaximumAreaTriangleResult result;
assert(!result.valid());
```
