---
title: Minimum Enclosing Sphere of Four 3D Points (三次元四点の最小包含球)
documentation_of: ../../../../src/algorithm/geometry/3d/minimum_enclosing_sphere_four_points.hpp
---

四点を含む半径最小の閉球を候補support集合から構築する。

## API

- `minimum_enclosing_sphere(first, second, third, fourth)`: 一点から四点が
  定める全ての有限候補球から四点を含む半径最小の `Sphere3` を返す。

## API別の時間計算量・空間計算量

候補数が定数なので時間・追加領域ともに $O(1)$。

## 注意点

重複・共線・共面入力も扱う。座標は有限でなければならず、違反時は
`std::invalid_argument`。有限な中心または半径を表せない場合は
`std::overflow_error`。
