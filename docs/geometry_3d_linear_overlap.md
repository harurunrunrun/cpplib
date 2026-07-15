---
title: 3D Linear Overlap Test (三次元線形図形の重なり判定)
documentation_of: ../src/algorithm/geometry/3d/linear_overlap.hpp
---

## API

- `overlap(first, second)`: `Line3`, `Ray3`, `Segment3` の全組合せについて、共通部分が正の長さを持つかを返す。

## API別の時間計算量・空間計算量

- 全overload: 時間・追加領域 $O(1)$。

## 注意点

1点だけの接触は `false`。退化直線・半直線には `std::invalid_argument` を送出する。
