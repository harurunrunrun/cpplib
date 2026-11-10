---
title: Convex Polygon Maximum Area Triangle Result (凸多角形内の最大面積三角形の結果)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/convex_polygon_maximum_area_triangle_result.hpp
---

凸多角形の頂点から選んだ最大面積の三角形を表す結果型。

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

- `indices`: 選ばれた 3 頂点の添字（昇順）。
- `doubled_area`: 符号なし面積の 2 倍。
- `valid()`: 有効な 3 頂点が格納されているとき（`indices[0]` が無効値でないとき）`true` を返す。
- `area()`: `doubled_area / 2` を返す。

デフォルト構築した値では、すべての添字が `std::numeric_limits<std::size_t>::max()`、
`doubled_area` が $0$ となる。

## 時間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| デフォルト構築、各フィールドの参照 | $O(1)$ | $O(1)$ |
| `valid()`, `area()` | $O(1)$ | $O(1)$ |

## 注意点

この型は頂点列を保持しないため、添字を解釈するには計算に用いた正規化後の
頂点列が必要である。`doubled_area` は非負かつ有限であることを前提とする。

## 使用例

```cpp
ConvexPolygonMaximumAreaTriangleResult result;
assert(!result.valid());
```
