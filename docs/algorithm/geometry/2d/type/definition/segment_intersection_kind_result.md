---
title: Segment Intersection Kind Result (線分交差分類の結果)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/segment_intersection_kind_result.hpp
---

2線分の共通部分の次元を表す非scoped列挙型。

## API

```cpp
enum SegmentIntersectionKind {
    SEGMENT_INTERSECTION_NONE = 0,
    SEGMENT_INTERSECTION_POINT = 1,
    SEGMENT_INTERSECTION_SEGMENT = 2,
};
```

それぞれ空集合、1点、正の長さを持つ線分を表す。

## 時間計算量

| 操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 構築・比較・整数変換 | $O(1)$ | $O(1)$ |

## 注意点

後方互換性のためunscoped enumであり、列挙子はglobal namespaceへ導入される。
列挙子の整数値はAPI契約の一部である。
