---
title: Minimum Enclosing Sphere of Two 3D Points (三次元二点の最小包含球)
documentation_of: ../../../../../../src/algorithm/geometry/3d/sphere/point_point/minimum_enclosing_sphere_two_points.hpp
---

二点を含む半径最小の閉球を定数時間で構築する。

## API

- `minimum_enclosing_sphere(first, second)`: 二点の中点を中心、距離の半分を
  半径とする `Sphere3` を返す。重複点にも対応する。

## API別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

座標は有限でなければならない。結果を `long double` で表せない場合は
`std::overflow_error`。点差は局所scaleで正規化し、直接減算のoverflowを避ける。
