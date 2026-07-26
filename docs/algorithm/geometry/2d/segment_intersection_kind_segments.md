---
title: Intersection Kind for Two Segments (2線分の交差分類)
documentation_of: ../../../../src/algorithm/geometry/2d/segment_intersection_kind_segments.hpp
---

2線分の共通部分を空集合、1点、線分の3種類へ分類する。

## API

```cpp
SegmentIntersectionKind segment_intersection_kind(
    const Segment& first,
    const Segment& second
);
```

## 引数・戻り値

`first` と `second` に分類する2線分を渡す。
共通部分に対応する `SegmentIntersectionKind` を返す。

## 時間計算量

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `segment_intersection_kind(first, second)` | $O(1)$ | $O(1)$ |

## 注意点

端点接触と同一点で交わる退化線分は `SEGMENT_INTERSECTION_POINT` に分類する。
重なりが正の長さを持つ場合だけ `SEGMENT_INTERSECTION_SEGMENT` を返す。
