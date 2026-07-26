---
title: Line-Line Closest Points (二直線の最近点対)
documentation_of: ../../../../src/algorithm/geometry/3d/line_line_closest_points.hpp
---

## API

- `closest_points(first, second)`: 二直線間の距離を達成する各直線上の点対を返す。

## API別の時間計算量・空間計算量

- 対応API: 時間・追加領域 $O(1)$。

## 注意点

両直線は退化しておらず、最近点を座標型で表せない場合は例外を送出する。
