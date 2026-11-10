---
title: Distance between 3D Segment and Line in Segment-Line Order (線分・直線順の三次元距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/segment_line/line_segment_distance_segment_line.hpp
---

## API

- `distance(segment, line)`: `Segment3` と `Line3` の集合間距離を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 引数の順序を入れ替えて `distance(line, segment)` に委譲する。戻り値と例外の条件は `distance(line, segment)` と同じである。
