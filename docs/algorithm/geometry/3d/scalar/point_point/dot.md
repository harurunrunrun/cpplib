---
title: 3D Dot Product (三次元内積)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/point_point/dot.hpp
---

## API

- `dot(left, right)`: 2 ベクトルの内積を `long double` で返す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 入力の検査やオーバーフローの検出は行わない。中間の積と和が `long double` で表現可能でなければならない。
