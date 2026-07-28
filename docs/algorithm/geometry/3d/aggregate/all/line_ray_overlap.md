---
title: 3D Line-Ray Overlap (三次元直線と半直線の重なり判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/line_ray_overlap.hpp
---

## API

- `overlap(line, ray)`: `Line3` と `Ray3` の共通部分が正の長さを持つなら `true` を返す。
- `overlap(ray, line)`: 引数順を逆にした同じ判定を行う。

## API別の時間計算量・空間計算量

- 全overload: 時間・追加領域 $O(1)$。

## 注意点

1点での接触は `false`。退化直線または退化半直線なら `std::invalid_argument` を送出する。
