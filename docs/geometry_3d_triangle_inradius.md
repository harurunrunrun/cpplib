---
title: 3D Triangle Inradius (三次元三角形の内接円半径)
documentation_of: ../src/algorithm/geometry/3d/scalar/triangle_inradius.hpp
---

## API

- `triangle_inradius(triangle)`: 内接円半径を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

退化三角形・非有限入力には `std::invalid_argument` を送出する。辺差分を共通の
局所scaleへ正規化し、正規化面積と周長の比から半径を求める。局所形状または半径を
`long double` で表現できない場合は `std::overflow_error` を送出する。
