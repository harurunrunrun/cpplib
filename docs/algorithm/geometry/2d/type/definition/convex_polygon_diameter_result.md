---
title: Convex Polygon Diameter Result (凸多角形の直径の結果)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/convex_polygon_diameter_result.hpp
---

凸多角形の直径を表す結果型。

```cpp
#include "../../../../../../src/algorithm/geometry/2d/type/definition/convex_polygon_diameter_result.hpp"
```

## API

```cpp
struct ConvexPolygonDiameterResult {
    std::size_t first;
    std::size_t second;
    long double squared_distance;

    long double distance() const;
};
```

- `first`, `second`: 最遠点対の添字。
- `squared_distance`: 最遠点対間の距離の二乗。
- `distance()`: `sqrt(squared_distance)` を返す。

既定構築した値では `first` と `second` が
`std::numeric_limits<std::size_t>::max()`、距離が0になる。

## 時間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| 既定構築、各フィールドの参照 | $O(1)$ | $O(1)$ |
| `distance()` | $O(1)$ | $O(1)$ |

## 注意点

この型だけでは頂点列を保持しないため、添字を解釈するには計算に用いた正規化後の
頂点列が必要である。`squared_distance` は非負かつ有限であることを前提とする。

空の凸多角形に対するAPIも既定構築と同じ無効添字を返す。

## 使用例

```cpp
ConvexPolygonDiameterResult result;
assert(result.first == std::numeric_limits<std::size_t>::max());
```
