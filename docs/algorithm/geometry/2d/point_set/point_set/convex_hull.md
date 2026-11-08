---
title: 2D Convex Hull (二次元凸包) [static_convex_hull]
documentation_of: ../../../../../../src/algorithm/geometry/2d/point_set/point_set/convex_hull.hpp
---

## API

- `std::vector<Point> convex_hull(std::vector<Point> points)`: 点集合の凸包の頂点を、$(x, y)$ の辞書順で最小の点から反時計回りに返す。

## API別の時間計算量・空間計算量

- `convex_hull`: 入力点数を $N$、出力点数を $H$ として、時間 $O(N \log N)$、作業領域 $O(N)$、出力 $O(H)$。

## 注意点

- 座標が完全に一致する重複点と、辺上にある中間の共線点は除く。
- 全点が共線なら両端の 2 点のみを返す。入力が空なら空列、相異なる点が 1 個ならその 1 点を返す。
- 辺上の共線点も残したい場合は `convex_hull_with_collinear` を使う。
