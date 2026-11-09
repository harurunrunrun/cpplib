---
title: 3D Line-Segment Parallelism (三次元直線と線分の平行判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/line_segment_parallel.hpp
---

## API

- `parallel(line, segment)`: `Line3` と `Segment3` の方向が平行なら `true` を返す。
- `parallel(segment, line)`: 引数の順序を入れ替えた同じ判定を行う。

## API 別の時間計算量・空間計算量

- 全オーバーロード: 時間・追加空間ともに $O(1)$。

## 注意点

- 同一直線上にあるかどうかや、重なりは判定しない。
- 退化した直線または退化した線分には `std::invalid_argument` を送出する。
