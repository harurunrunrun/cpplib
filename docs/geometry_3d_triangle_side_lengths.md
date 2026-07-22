---
title: 3D Triangle Side Lengths (三次元三角形の三辺長)
documentation_of: ../src/algorithm/geometry/3d/scalar/triangle_side_lengths.hpp
---

## API

- `triangle_side_lengths(triangle)`: $|ab|, |bc|, |ca|$ の順に長さを返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

退化三角形にも使用でき、長さ0の辺をそのまま返す。
