---
title: Closest Points in 3D (三次元最近点対)
documentation_of: ../src/algorithm/geometry/3d/closest_points.hpp
---

## API

- `closest_points(first_line, second_line)`: 2無限直線上の最近点を1組返す。
- `closest_points(first_segment, second_segment)`: 2閉線分上の最近点を1組返す。

## API別の時間計算量・空間計算量

- 各overload: 時間・追加領域 $O(1)$。

## 注意点

平行な場合は複数ある最近点対のうち1組を返す。線分は退化してもよいが、直線は非退化でなければならない。
