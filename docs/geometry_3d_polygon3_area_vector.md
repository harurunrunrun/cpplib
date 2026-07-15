---
title: Polygon3 Area Vector (三次元多角形の面積ベクトル)
documentation_of: ../src/algorithm/geometry/3d/polygon3_area_vector.hpp
---

## API

- `polygon3_area_vector(polygon)`: 頂点順に対応する向き付き面積ベクトルを返す。

## API別の時間計算量・空間計算量

- 頂点数を$N$として時間$O(N)$、追加領域$O(1)$。

## 注意点

頂点は同一平面上に境界順で与える。3頂点未満には`std::invalid_argument`を送出する。
