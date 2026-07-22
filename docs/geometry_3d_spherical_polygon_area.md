---
title: Spherical Polygon Area (球面多角形面積)
documentation_of: ../src/algorithm/geometry/3d/scalar/spherical_polygon_area.hpp
---

## API

- `spherical_polygon_area(polygon, radius=1)`: 頂点を短い大円弧で順に結ぶ球面多角形の非負面積を返す。頂点が3未満なら0を返す。

## API別の時間計算量・空間計算量

頂点数を$N$として時間$O(N)$、追加領域$O(N)$。

## 注意点

単純多角形が一つの開半球内にあり、入力順が境界順であることを前提とする。頂点は有限な非零ベクトル、半径は有限な非負値でなければならず、反対な頂点を結ぶ辺は許さない。向きを反転しても同じ非負面積を返す。
