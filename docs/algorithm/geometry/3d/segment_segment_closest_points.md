---
title: Segment-Segment Closest Points (二線分の最近点対)
documentation_of: ../../../../src/algorithm/geometry/3d/segment_segment_closest_points.hpp
---

## API

- `closest_points(first, second)`: 二閉線分間の距離を達成する点対を返す。

## API別の時間計算量・空間計算量

- 対応API: 時間・追加領域 $O(1)$。

## 注意点

退化線分も一点として扱う。最近点を表せない場合は例外を送出する。
