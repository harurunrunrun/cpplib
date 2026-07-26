---
title: 3D Segment-Segment Parallelism (三次元線分同士の平行判定)
documentation_of: ../../../../src/algorithm/geometry/3d/segment_segment_parallel.hpp
---

## API

- `parallel(first, second)`: 2本の `Segment3` の方向が平行なら `true` を返す。

## API別の時間計算量・空間計算量

- `parallel`: 時間・追加領域 $O(1)$。

## 注意点

共線性や重なりは判定しない。いずれかが退化線分なら `std::invalid_argument` を送出する。
