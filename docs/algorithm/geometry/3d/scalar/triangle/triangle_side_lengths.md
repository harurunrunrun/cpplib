---
title: 3D Triangle Side Lengths (三次元三角形の三辺長)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/triangle/triangle_side_lengths.hpp
---

## API

- `triangle_side_lengths(triangle)`: 辺の長さを $\lVert b - a \rVert, \lVert c - b \rVert, \lVert a - c \rVert$ の順に `std::array<long double, 3>` で返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

- 退化した三角形にも使用でき、長さ $0$ の辺はそのまま $0$ を返す。
- 座標差が非有限値になる場合は `std::invalid_argument` を、長さが `long double` で表現できない場合は `std::overflow_error` を送出する。
