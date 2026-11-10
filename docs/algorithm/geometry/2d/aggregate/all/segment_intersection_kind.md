---
title: Segment Intersection Kind Aggregator (線分交差分類集約ヘッダ)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/segment_intersection_kind.hpp
---

線分の交差の種類を表す結果型と、2 線分を入力とする API をまとめた後方互換の集約ヘッダである。既存のコードはこのヘッダをそのまま利用できる。

## 構成

| leaf header | 提供する API |
| --- | --- |
| `segment_intersection_kind_result.hpp` | `SegmentIntersectionKind` |
| `segment_intersection_kind_segments.hpp` | `segment_intersection_kind(first, second)` |

## 集約される API

```cpp
enum SegmentIntersectionKind {
    SEGMENT_INTERSECTION_NONE = 0,
    SEGMENT_INTERSECTION_POINT = 1,
    SEGMENT_INTERSECTION_SEGMENT = 2,
};

SegmentIntersectionKind segment_intersection_kind(const Segment& first, const Segment& second);
```

2 つの閉線分が交わらなければ `SEGMENT_INTERSECTION_NONE`、共通部分が 1 点なら `SEGMENT_INTERSECTION_POINT`、正の長さの線分なら `SEGMENT_INTERSECTION_SEGMENT` を返す。

## 計算量

| API・操作 | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| `segment_intersection_kind`、結果値の参照 | $O(1)$ | $O(1)$ |
