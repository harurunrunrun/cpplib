---
title: Exact Convex Geometry (厳密凸幾何)
documentation_of: ../../../src/integer_geometry/2d/convex.hpp
---

有理数座標の凸包、凸性判定、Minkowski和を厳密に求める。

## API

- `convex_hull(points)`: 重複点と辺上の中間点を除いた反時計回りの凸包を返す。
- `is_convex(polygon, strict)`: 多角形が凸か判定する。`strict=true` では連続3点の共線を許さない。
- `add_points(first, second)`: 2点を位置ベクトルとして加える。
- `minkowski_sum_convex(first, second)`: 2つの凸多角形のMinkowski和を返す。
- `minkowski_sum(first, second)`: 任意の有限点集合の凸包同士のMinkowski和を返す。

## API別の時間計算量・空間計算量

入力点数を $n,m$、座標比較・有理数演算時間を $R(L)$ とする。

- `convex_hull`: 時間 $O(n\log n\,R(L))$、領域 $O(nL)$。
- `is_convex`: 時間 $O(nR(L))$、追加領域 $O(L)$。
- `add_points`: 時間 $O(R(L))$、追加領域 $O(L)$。
- `minkowski_sum_convex`: 時間 $O((n+m)R(L))$、出力を含む領域 $O((n+m)L)$。
- `minkowski_sum`: 時間 $O((n\log n+m\log m)R(L))$、領域 $O((n+m)L)$。

## 注意点

`minkowski_sum_convex` の各入力は境界順の凸多角形でなければならない。空入力を含むMinkowski和は空である。
