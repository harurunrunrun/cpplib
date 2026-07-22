---
title: Polygon3 Area (三次元多角形の面積)
documentation_of: ../src/algorithm/geometry/3d/scalar/polygon3_area.hpp
---

## API

- `polygon3_area(polygon)`: 多角形の面積を返す。

## API別の時間計算量・空間計算量

- 頂点数を$N$として時間$O(N)$、追加領域$O(1)$。

## 注意点

頂点は同一平面上に境界順で与える。
