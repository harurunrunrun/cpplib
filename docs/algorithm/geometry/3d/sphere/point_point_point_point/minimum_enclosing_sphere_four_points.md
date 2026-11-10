---
title: Minimum Enclosing Sphere of Four 3D Points (三次元四点の最小包含球)
documentation_of: ../../../../../../src/algorithm/geometry/3d/sphere/point_point_point_point/minimum_enclosing_sphere_four_points.hpp
---

4 点を含む半径最小の閉球を、境界上の点（サポート点）の候補集合から構築する。

## API

- `minimum_enclosing_sphere(first, second, third, fourth)`: 1 点から 4 点までの部分集合が定める有限な候補球のうち、4 点すべてを含み半径が最小の `Sphere3` を返す。

## API別の時間計算量・空間計算量

候補数が定数なので、時間・追加領域ともに $O(1)$。

## 注意点

- 点の重複、同一直線上・同一平面上にある入力も扱える。
- 座標は有限でなければならず、違反時は `std::invalid_argument` を送出する。有限な中心または半径を表現できない場合は `std::overflow_error` を送出する。
- 包含判定にはスケールに比例する浮動小数点の許容誤差を用いる。
