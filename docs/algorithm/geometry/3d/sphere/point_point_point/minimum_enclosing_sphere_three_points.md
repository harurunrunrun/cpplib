---
title: Minimum Enclosing Sphere of Three 3D Points (三次元三点の最小包含球)
documentation_of: ../../../../../../src/algorithm/geometry/3d/sphere/point_point_point/minimum_enclosing_sphere_three_points.hpp
---

3 点を含む半径最小の閉球を、境界上の点（サポート点）の候補集合から構築する。

## API

- `minimum_enclosing_sphere(first, second, third)`: 1 点・2 点・3 点が定める有限な候補球のうち、3 点すべてを含み半径が最小の `Sphere3` を返す。

## API別の時間計算量・空間計算量

候補数が定数なので、時間・追加領域ともに $O(1)$。

## 注意点

- 点の重複、3 点が同一直線上にある場合、鈍角三角形の場合も個別の分岐なしで扱える。
- 座標は有限でなければならず、違反時は `std::invalid_argument` を送出する。有限な結果を表現できない場合は `std::overflow_error` を送出する。
- 包含判定にはスケールに比例する浮動小数点の許容誤差を用いる。
