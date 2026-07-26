---
title: Sphere Contains Point (球の点包含判定)
documentation_of: ../../../../src/algorithm/geometry/3d/sphere_contains_point.hpp
---

## API

- `contains(sphere, point)`: 内部なら`1`、球面上なら`0`、外部なら`-1`を返す。

## API別の時間計算量・空間計算量

- 全API: 時間・追加領域 $O(1)$。

## 注意点

半径は非負、中心と点は有限値でなければならない。
