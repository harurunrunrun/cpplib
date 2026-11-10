---
title: Polygon3 Signed Area (三次元多角形の向き付き面積)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/polygon_point/polygon3_signed_area.hpp
---

## API

- `polygon3_signed_area(polygon, reference_normal)`: 面積ベクトルを `reference_normal` 方向へ射影した符号付き面積を返す。

## API別の時間計算量・空間計算量

- 頂点数を $N$ として、時間 $O(N)$、追加領域 $O(1)$。

## 注意点

- 参照法線の長さが許容誤差内で $0$ の場合は `std::invalid_argument` を送出する。参照法線が非有限の場合や、頂点数が 3 未満の場合も `std::invalid_argument` を送出する。
