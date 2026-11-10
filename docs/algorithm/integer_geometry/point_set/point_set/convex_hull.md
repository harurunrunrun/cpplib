---
title: Exact Convex Hull (厳密凸包)
documentation_of: ../../../../../src/algorithm/integer_geometry/point_set/point_set/convex_hull.hpp
---

点集合の凸包の頂点を反時計回りに列挙する。

## API

- `std::vector<Point> convex_hull(std::vector<Point> points)`: 重複点と辺上の余分な点を除いた凸包を、辞書順最小の点から反時計回りに返す。点が 1 個以下ならソート・重複除去した点をそのまま返し、全点が一直線上なら両端の 2 点を返す。

## API別の時間計算量・空間計算量

- `convex_hull(points)`: 時間 $O(n \log n)$、戻り値を含む追加領域 $O(n)$

## 注意点

- 計算は任意精度整数（`Integer`）と既約有理数（`Rational`）で厳密に行う。記載した計算量は算術演算の回数であり、実際にはこれに加えて座標のビット長に応じた多倍長整数演算のコストがかかる。
- すべての API は名前空間 `integer_geometry` に属する。
