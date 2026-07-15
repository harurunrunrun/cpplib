---
title: Polygon3 Signed Area (三次元多角形の向き付き面積)
documentation_of: ../src/algorithm/geometry/3d/polygon3_signed_area.hpp
---

## API

- `polygon3_signed_area(polygon, normal)`: 面積ベクトルを`normal`方向へ射影した符号付き面積を返す。

## API別の時間計算量・空間計算量

- 頂点数を$N$として時間$O(N)$、追加領域$O(1)$。

## 注意点

零参照法線には`std::invalid_argument`を送出する。
