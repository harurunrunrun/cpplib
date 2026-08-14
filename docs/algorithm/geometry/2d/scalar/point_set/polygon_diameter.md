---
title: General Polygon Diameter (一般多角形の直径)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/point_set/polygon_diameter.hpp
---

凸性を仮定しない多角形の閉領域に含まれる2点間の最大距離を返す。最大点対は凸包上にあるため、凸包構築後にrotating calipersを用いる。

## API

- `polygon_diameter(polygon)`: 一般多角形の直径を返す。頂点が1個以下なら $0$。

## API別の時間計算量・空間計算量

- 頂点数を $N$、凸包頂点数を $H$ として時間計算量 $O(N\log N+H)$、追加空間計算量 $O(N)$。

## 注意点

頂点順・凸性・自己交差を問わず、入力頂点集合の凸包の直径として計算する。座標は有限でなければならない。
