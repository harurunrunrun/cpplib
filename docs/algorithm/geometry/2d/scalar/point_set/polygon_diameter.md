---
title: General Polygon Diameter (一般多角形の直径)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/point_set/polygon_diameter.hpp
---

凸性を仮定しない多角形について、その閉領域に含まれる 2 点間の最大距離を返す。最大距離を与える点対は凸包の頂点上にあるため、凸包を構築してから rotating calipers を用いる。

## API

```cpp
long double polygon_diameter(std::vector<Point> polygon);
```

一般の多角形の直径を返す。頂点が 1 個以下なら $0$ を返す。

## 時間計算量

頂点数を $N$ として、時間計算量 $O(N \log N)$（凸包の構築が支配的）、追加空間計算量 $O(N)$。

## 注意点

頂点の順序・凸性・自己交差の有無を問わず、入力頂点集合の凸包の直径として計算する。座標は有限でなければならない。
