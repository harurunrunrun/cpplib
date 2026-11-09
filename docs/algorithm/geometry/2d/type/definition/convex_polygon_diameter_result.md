---
title: Convex Polygon Diameter Result (凸多角形の直径の結果)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/convex_polygon_diameter_result.hpp
---

凸多角形の直径（最遠点対）を表す結果型。

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
- `squared_distance`: 最遠点対の距離の二乗。
- `distance()`: `std::sqrt(squared_distance)` を返す。

デフォルト構築した値では、`first` と `second` が
`std::numeric_limits<std::size_t>::max()`、距離が $0$ になる。

## 時間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| デフォルト構築、各フィールドの参照 | $O(1)$ | $O(1)$ |
| `distance()` | $O(1)$ | $O(1)$ |

## 注意点

この型は頂点列を保持しないため、添字を解釈するには計算に用いた正規化後の
頂点列が必要である。`squared_distance` は非負かつ有限であることを前提とする。

空の凸多角形に対する API も、デフォルト構築と同じ無効な添字を返す。

## 使用例

```cpp
ConvexPolygonDiameterResult result;
assert(result.first == std::numeric_limits<std::size_t>::max());
```
