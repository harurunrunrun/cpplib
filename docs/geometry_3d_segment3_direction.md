---
title: 3D Segment Direction (三次元線分の方向)
documentation_of: ../src/algorithm/geometry/3d/point/segment3_direction.hpp
---

## API

- `segment3_direction(segment)`: `segment.b - segment.a` を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

退化線分には方向がないため `std::invalid_argument` を送出する。
