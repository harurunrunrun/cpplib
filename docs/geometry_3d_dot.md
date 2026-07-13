---
title: 3D Dot Product (三次元内積)
documentation_of: ../src/algorithm/geometry/3d/dot.hpp
---

## API

- `dot(left, right)`: 2ベクトルの内積を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

戻り値は `long double` であり、中間積が表現可能でなければならない。
