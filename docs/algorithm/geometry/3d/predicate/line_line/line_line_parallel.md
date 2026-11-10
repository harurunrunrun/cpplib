---
title: 3D Line-Line Parallelism (三次元直線同士の平行判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/line_line/line_line_parallel.hpp
---

## API

- `parallel(first, second)`: 2 本の `Line3` の方向ベクトルが、相対許容誤差 `GEOMETRY3D_EPS` の範囲で平行なら `true` を返す。

## API 別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

- 同一直線上にあるかは判定しない（一致する直線も `true`）。
- いずれかが退化した直線なら `std::invalid_argument` を送出する。
