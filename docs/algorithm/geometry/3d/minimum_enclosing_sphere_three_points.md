---
title: Minimum Enclosing Sphere of Three 3D Points (三次元三点の最小包含球)
documentation_of: ../../../../src/algorithm/geometry/3d/minimum_enclosing_sphere_three_points.hpp
---

三点を含む半径最小の閉球を候補support集合から構築する。

## API

- `minimum_enclosing_sphere(first, second, third)`: 一点・二点・三点が定める
  全ての有限候補球から三点を含む半径最小の `Sphere3` を返す。

## API別の時間計算量・空間計算量

候補数が定数なので時間・追加領域ともに $O(1)$。

## 注意点

重複・共線・鈍角三角形も個別分岐なしで扱う。座標は有限でなければならず、
違反時は `std::invalid_argument`。有限結果を表せない場合は
`std::overflow_error`。
