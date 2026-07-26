---
title: 3D Line-Ray Parallelism (三次元直線と半直線の平行判定)
documentation_of: ../../../../src/algorithm/geometry/3d/line_ray_parallel.hpp
---

## API

- `parallel(line, ray)`: `Line3` と `Ray3` の方向が平行なら `true` を返す。
- `parallel(ray, line)`: 引数順を逆にした同じ判定を行う。

## API別の時間計算量・空間計算量

- 全overload: 時間・追加領域 $O(1)$。

## 注意点

支持直線が一致するかは判定しない。退化直線または退化半直線なら `std::invalid_argument` を送出する。
