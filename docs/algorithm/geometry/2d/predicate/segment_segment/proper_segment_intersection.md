---
title: Proper Segment Intersection (線分の真交差判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/predicate/segment_segment/proper_segment_intersection.hpp
---

## API

`bool proper_segment_intersection(first, second)`

## 引数

2 本の線分 `first`, `second` を渡す。

## 戻り値

両線分の内部同士が 1 点で交差するときだけ `true`。

## API別の時間計算量・空間計算量

- `proper_segment_intersection`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 端点での接触、共線部分の重なり、退化線分を含む場合は `false` を返す。
- 各向き判定には、基準線分からの垂直距離に基づく許容誤差を用いる。
