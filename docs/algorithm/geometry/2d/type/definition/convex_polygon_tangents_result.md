---
title: Convex Polygon Tangent Result (凸多角形への接線の結果)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/convex_polygon_tangents_result.hpp
---

外部の点から凸多角形へ引いた左右 2 本の接線について、接点の添字を保持する結果型。

```cpp
#include "../../../../../../src/algorithm/geometry/2d/type/definition/convex_polygon_tangents_result.hpp"
```

## API

```cpp
struct ConvexPolygonTangentResult {
    std::size_t left;
    std::size_t right;
};
```

- `left`: 外点から接点へ向かう有向直線に対して、凸集合が左側にある接点の添字。
- `right`: 外点から接点へ向かう有向直線に対して、凸集合が右側にある接点の添字。

デフォルト構築した値では、両方が `std::numeric_limits<std::size_t>::max()` になる。

## 時間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| デフォルト構築、`left`, `right` の参照 | $O(1)$ | $O(1)$ |

## 注意点

この型は頂点列を保持しない。添字は、接線の計算に用いた正規化後の頂点列に対するものとして
解釈する。空集合に対する接線 API は、デフォルト構築と同じ無効な添字を返す。

## 使用例

```cpp
ConvexPolygonTangentResult result;
assert(result.left == std::numeric_limits<std::size_t>::max());
```
