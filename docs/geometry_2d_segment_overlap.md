---
title: Collinear Segment Overlap (共線線分の共通部分)
documentation_of: ../src/algorithm/geometry/2d/shape/segment_overlap.hpp
---

## API

`optional<Segment> segment_overlap(first, second)`

## 引数

2線分を渡す。

## 戻り値

共線な共通部分を辞書順に並べた端点で返す。1点接触は退化線分、共通部分なしまたは非共線なら `nullopt`。

## API別の時間計算量・空間計算量

- `segment_overlap`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

浮動小数点比較には `GEOMETRY_EPS` を用いる。
退化線分を1点として扱い、非退化線分の共線性は線分からの垂直距離で判定する。
