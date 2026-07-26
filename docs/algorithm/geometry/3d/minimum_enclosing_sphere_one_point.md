---
title: Minimum Enclosing Sphere of One 3D Point (三次元一点の最小包含球)
documentation_of: ../../../../src/algorithm/geometry/3d/minimum_enclosing_sphere_one_point.hpp
---

一点だけを含む半径0の閉球を返す。

## API

- `minimum_enclosing_sphere(point)`: 中心を `point`、半径を0とする
  `Sphere3` を返す。

## API別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

座標は有限でなければならず、違反時は `std::invalid_argument`。
入力点を丸めず中心へそのまま保持する。
