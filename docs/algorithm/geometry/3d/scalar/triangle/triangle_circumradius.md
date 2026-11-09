---
title: 3D Triangle Circumradius (三次元三角形の外接円半径)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/triangle/triangle_circumradius.hpp
---

## API

- `triangle_circumradius(triangle)`: 三角形の外接円の半径を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

- `circumcircle(triangle).radius` を返す。すなわち、`circumcircle` と同じく二進有理数による厳密な外心計算と正規化した局所距離計算を用いる。
- 退化した三角形や非有限な入力には `std::invalid_argument` を、半径を `long double` で表現できない場合は `std::overflow_error` を送出する。
