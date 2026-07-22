---
title: Polygon3 to 2D (三次元多角形の二次元化)
documentation_of: ../src/algorithm/geometry/3d/point_collection/polygon3_to_2d.hpp
---

## API

- `polygon3_to_2d(polygon, system)`: 指定平面座標系で全頂点を2次元化する。
- `polygon3_to_2d(polygon)`: 多角形から座標系を構築し、座標系と2次元頂点列を返す。

## API別の時間計算量・空間計算量

- 頂点数を$N$として各overloadは時間・戻り値領域$O(N)$。

## 注意点

自動構築版は面積0または3頂点未満の多角形に`std::invalid_argument`を送出する。
