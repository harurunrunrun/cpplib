---
title: Polygon3 Area Vector (三次元多角形の面積ベクトル)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/polygon/polygon3_area_vector.hpp
---

## API

- `polygon3_area_vector(polygon)`: 頂点順に対応する向き付き面積ベクトルを返す。向きは頂点順から右手系で定まる法線方向、長さは面積である。

## API別の時間計算量・空間計算量

- 頂点数を $N$ として時間 $O(N)$、追加領域 $O(1)$。

## 注意点

- 頂点は同一平面上に境界順で与える。
- 頂点数が 3 未満なら `std::invalid_argument` を送出する。
