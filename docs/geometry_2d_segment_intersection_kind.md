---
title: Segment Intersection Kind (線分交差部分の分類)
documentation_of: ../src/algorithm/geometry/2d/predicate/segment_intersection_kind.hpp
---

## API

`SegmentIntersectionKind segment_intersection_kind(first, second)`

## 引数

2線分を渡す。

## 戻り値

交差なし、1点、非零長の線分を `SEGMENT_INTERSECTION_NONE`, `POINT`, `SEGMENT` で返す。

## API別の時間計算量・空間計算量

- `segment_intersection_kind`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

端点接触と同一点の退化線分は `POINT` に分類する。
