---
title: 3D Line-Segment Parallelism (三次元直線と線分の平行判定)
documentation_of: ../../../../src/algorithm/geometry/3d/line_segment_parallel.hpp
---

## API

- `parallel(line, segment)`: `Line3` と `Segment3` の方向が平行なら `true` を返す。
- `parallel(segment, line)`: 引数順を逆にした同じ判定を行う。

## API別の時間計算量・空間計算量

- 全overload: 時間・追加領域 $O(1)$。

## 注意点

同一直線上にあるかや重なりは判定しない。退化直線または退化線分なら `std::invalid_argument` を送出する。
