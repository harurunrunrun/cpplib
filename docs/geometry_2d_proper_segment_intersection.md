---
title: Proper Segment Intersection (線分の真交差判定)
documentation_of: ../src/algorithm/geometry/2d/predicate/proper_segment_intersection.hpp
---

## API

`bool proper_segment_intersection(first, second)`

## 引数

2線分を渡す。

## 戻り値

両線分の内部が1点で交差するときだけ `true`。

## API別の時間計算量・空間計算量

- `proper_segment_intersection`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

端点接触、共線重複、退化線分は `false`。
各向き判定は基準線分からの垂直距離に基づく許容誤差を用いる。
