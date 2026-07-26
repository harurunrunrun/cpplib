---
title: 3D Line-Line Parallelism (三次元直線同士の平行判定)
documentation_of: ../../../../src/algorithm/geometry/3d/line_line_parallel.hpp
---

## API

- `parallel(first, second)`: 2本の `Line3` の方向ベクトルが平行なら `true` を返す。

## API別の時間計算量・空間計算量

- `parallel`: 時間・追加領域 $O(1)$。

## 注意点

同一直線上にあるかは判定しない。いずれかが退化直線なら `std::invalid_argument` を送出する。
