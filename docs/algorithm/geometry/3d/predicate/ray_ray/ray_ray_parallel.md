---
title: 3D Ray-Ray Parallelism (三次元半直線同士の平行判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/ray_ray/ray_ray_parallel.hpp
---

## API

- `parallel(first, second)`: 2本の `Ray3` の方向が平行なら `true` を返す。向きが反対でも平行とする。

## API別の時間計算量・空間計算量

- `parallel`: 時間・追加領域 $O(1)$。

## 注意点

始点や向きの一致は判定しない。いずれかが退化半直線なら `std::invalid_argument` を送出する。
