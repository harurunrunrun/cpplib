---
title: 2D Convex Hull (二次元凸包) [CGL_4_A] [static_convex_hull]
documentation_of: ../src/algorithm/geometry/2d/convex_hull.hpp
---

## API

- `convex_hull(points)`: 点集合の凸包頂点を反時計回りに返す。

## API別の時間計算量・空間計算量

- 入力点数を $N$、出力点数を $H$ として、時間 $O(N\log N)$、作業領域 $O(N)$、出力 $O(H)$。

## 注意点

重複点と辺上の中間共線点は除く。全点が共線なら両端のみを返し、入力が空なら空列を返す。
