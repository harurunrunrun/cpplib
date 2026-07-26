---
title: Convex Polygon Tangent Result (凸多角形への接線の結果)
documentation_of: ../../../../src/algorithm/geometry/2d/convex_polygon_tangents_result.hpp
---

外点から凸多角形へ引いた左右の接線の接点添字を保持する結果型。

```cpp
#include "src/algorithm/geometry/2d/convex_polygon_tangents_result.hpp"
```

## API

```cpp
struct ConvexPolygonTangentResult {
    std::size_t left;
    std::size_t right;
};
```

- `left`: 有向直線「外点から接点」に対して凸集合が左側にある接点の添字。
- `right`: 有向直線「外点から接点」に対して凸集合が右側にある接点の添字。

既定構築した値では両方が `std::numeric_limits<std::size_t>::max()` になる。

## 時間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| 既定構築、`left`, `right` の参照 | $O(1)$ | $O(1)$ |

## 注意点

この型は頂点列を保持しない。添字は接線計算に用いた正規化後の頂点列に対して
解釈する。空集合に対する接線APIは既定構築と同じ無効添字を返す。

## 使用例

```cpp
ConvexPolygonTangentResult result;
assert(result.left == std::numeric_limits<std::size_t>::max());
```
