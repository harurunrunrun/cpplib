---
title: Segment Intersection Kind Aggregator (線分交差分類集約ヘッダ)
documentation_of: ../../../../src/algorithm/geometry/2d/segment_intersection_kind.hpp
---

線分交差分類の結果型と2線分入力APIをまとめる後方互換集約ヘッダ。

## 構成

| leaf header | 提供するAPI |
| --- | --- |
| `segment_intersection_kind_result.hpp` | `SegmentIntersectionKind` |
| `segment_intersection_kind_segments.hpp` | `segment_intersection_kind(first, second)` |

## 集約されるAPI

```cpp
SegmentIntersectionKind segment_intersection_kind(
    const Segment& first,
    const Segment& second
);
```

## API別の時間計算量・空間計算量

| API・操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `segment_intersection_kind`、結果値の参照 | $O(1)$ | $O(1)$ |

## 注意点

既存コードはこのヘッダを変更せず利用できる。
