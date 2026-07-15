---
title: 3D Triangle Circumradius (三次元三角形の外接円半径)
documentation_of: ../src/algorithm/geometry/3d/triangle_circumradius.hpp
---

## API

- `triangle_circumradius(triangle)`: 外接円半径を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

`circumcircle`と同じexact dyadic外心・局所距離計算を使う。退化三角形・非有限入力には
`std::invalid_argument`、半径を表現できない場合は `std::overflow_error` を送出する。
