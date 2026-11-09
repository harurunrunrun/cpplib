---
title: Minimum Enclosing Sphere of One 3D Point (三次元一点の最小包含球)
documentation_of: ../../../../../../src/algorithm/geometry/3d/sphere/point/minimum_enclosing_sphere_one_point.hpp
---

1 点だけを含む半径 $0$ の閉球を返す。

## API

- `minimum_enclosing_sphere(point)`: 中心を `point`、半径を $0$ とする `Sphere3` を返す。

## API別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

- 座標は有限でなければならず、違反時は `std::invalid_argument` を送出する。
- 入力点を丸めず、そのまま中心として保持する。
