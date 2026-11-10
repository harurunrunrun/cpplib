---
title: Polygon3 Normal (三次元多角形の法線)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/polygon/polygon3_normal.hpp
---

## API

- `polygon3_normal(polygon)`: 頂点順に対応する単位法線（`polygon3_area_vector` の向き）を返す。

## API別の時間計算量・空間計算量

- 頂点数を $N$ として時間 $O(N)$、追加領域 $O(1)$。

## 注意点

- 頂点数が 3 未満の場合、または面積ベクトルの長さが `GEOMETRY3D_EPS`（$10^{-10}$） 以下（面積が $0$ とみなせる）の場合は `std::invalid_argument` を送出する。
