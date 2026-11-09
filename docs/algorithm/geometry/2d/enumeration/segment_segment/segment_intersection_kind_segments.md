---
title: Intersection Kind for Two Segments (2 線分の交差分類)
documentation_of: ../../../../../../src/algorithm/geometry/2d/enumeration/segment_segment/segment_intersection_kind_segments.hpp
---

2 線分の共通部分を、空集合・1 点・線分の 3 種類に分類する。

## API

```cpp
SegmentIntersectionKind segment_intersection_kind(
    const Segment& first,
    const Segment& second
);
```

## 引数・戻り値

`first` と `second` に分類したい 2 線分を渡す。
共通部分に応じて `SEGMENT_INTERSECTION_NONE`、`SEGMENT_INTERSECTION_POINT`、`SEGMENT_INTERSECTION_SEGMENT` のいずれかを返す。

## 時間計算量

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `segment_intersection_kind(first, second)` | $O(1)$ | $O(1)$ |

## 注意点

`intersect(first, second)` で交差を判定した後、`segment_overlap` で重なりを求める。
端点での接触や、1 点に退化した線分との交差は `SEGMENT_INTERSECTION_POINT` に分類する。
重なりが正の長さを持つ場合に限り `SEGMENT_INTERSECTION_SEGMENT` を返す。
