---
title: 3D Ray-Segment Parallelism (三次元半直線と線分の平行判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/ray_segment_parallel.hpp
---

## API

- `parallel(ray, segment)`: `Ray3` と `Segment3` の方向が平行なら `true` を返す。
- `parallel(segment, ray)`: 引数順を逆にした同じ判定を行う。

## API別の時間計算量・空間計算量

- 全overload: 時間・追加領域 $O(1)$。

## 注意点

重なりは判定しない。退化半直線または退化線分なら `std::invalid_argument` を送出する。
