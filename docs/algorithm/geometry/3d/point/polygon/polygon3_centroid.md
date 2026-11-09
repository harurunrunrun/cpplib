---
title: Polygon3 Centroid (三次元多角形の重心)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/polygon/polygon3_centroid.hpp
---

## API

- `polygon3_centroid(polygon)`: 面積に関する多角形の重心を返す。

## API別の時間計算量・空間計算量

- 頂点数を $N$ として時間 $O(N)$、追加領域 $O(N)$。

## 注意点

- 内部で `polygon3_to_2d` により平面座標へ写し、二次元の `polygon_centroid` で求めた重心を三次元へ戻す。
- 頂点は同一平面上に境界順で与える。
- 頂点数が 3 未満の場合や面積が $0$ の多角形には `std::invalid_argument` を送出する。
