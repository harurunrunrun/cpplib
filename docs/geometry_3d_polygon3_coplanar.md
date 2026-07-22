---
title: Polygon3 Coplanarity (三次元多角形の共面判定)
documentation_of: ../src/algorithm/geometry/3d/predicate/polygon3_coplanar.hpp
---

## API

- `polygon3_coplanar(polygon)`: 全頂点が同一平面上にあるか判定する。

## API別の時間計算量・空間計算量

- 頂点数を$N$として時間$O(N)$、追加領域$O(1)$。

## 注意点

3頂点未満には`std::invalid_argument`を送出する。全点共線は共面と判定する。
