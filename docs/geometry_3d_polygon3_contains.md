---
title: Polygon3 Point Containment (三次元多角形の点包含)
documentation_of: ../src/algorithm/geometry/3d/predicate/polygon3_contains.hpp
---

## API

- `polygon3_contains(polygon, point)`: 外部を0、境界を1、内部を2として返す。平面外の点は外部とする。

## API別の時間計算量・空間計算量

- 頂点数を$N$として時間・追加領域$O(N)$。

## 注意点

単純な共面多角形を与える。面積0の多角形には`std::invalid_argument`を送出する。
