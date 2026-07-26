---
title: 3D Line-Ray Skewness (三次元直線と半直線のねじれ判定)
documentation_of: ../../../../src/algorithm/geometry/3d/line_ray_skew.hpp
---

## API

- `skew(line, ray)`: `Line3` と `Ray3` の支持直線同士がねじれの位置にあるなら `true` を返す。
- `skew(ray, line)`: 引数順を逆にした同じ判定を行う。

## API別の時間計算量・空間計算量

- 全overload: 時間・追加領域 $O(1)$。

## 注意点

半直線の向きや始点による範囲制限は考慮しない。退化直線または退化半直線なら `std::invalid_argument` を送出する。
