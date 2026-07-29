---
title: Point on 3D Segment (三次元線分上判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/segment_point/on_segment.hpp
---

## API

- `on_segment(segment, point)`: 点が端点を含む閉線分上にあるか判定する。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

退化線分は1点として扱う。端点との比較には `Point3` の絶対許容誤差を用いるため、
計算で端点へ丸め戻した点も線分上と判定する。
