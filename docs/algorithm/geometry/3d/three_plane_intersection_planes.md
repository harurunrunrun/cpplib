---
title: Common Intersection of Three Planes (3平面の共通部分)
documentation_of: ../../../../src/algorithm/geometry/3d/three_plane_intersection_planes.hpp
---

3個の3次元平面すべてに共通する集合を計算します。

## API

- `three_plane_intersection(first, second, third)`: 共通部分を空集合・一点・直線・平面のいずれかとして返します。

## API別の時間計算量・空間計算量

時間・追加空間ともに $O(1)$ です。

## 注意点

各平面の点と法線は有限値で、法線は非零でなければなりません。下位の平面演算と同じ入力契約・例外条件に従います。