---
title: Line-Segment Intersection Point (直線と線分の交点)
documentation_of: ../src/algorithm/geometry/2d/point/line_segment_intersection.hpp
---

## API

`Point line_segment_intersection(line, segment)`

## 引数

直線と線分を渡す。

## 戻り値

一意な交点を返す。線分が1点なら、その点が直線上にある場合に返す。

## API別の時間計算量・空間計算量

- `line_segment_intersection`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

退化直線では `invalid_argument`、交点なしまたは交点が無数なら `domain_error`。
退化判定に該当しない範囲では、直線の方向ベクトルを正の定数倍しても交点は変わらない。
