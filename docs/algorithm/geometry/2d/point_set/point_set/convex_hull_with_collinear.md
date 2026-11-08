---
title: Convex Hull with Collinear Points (共線点を残す凸包) [CGL_4_A]
documentation_of: ../../../../../../src/algorithm/geometry/2d/point_set/point_set/convex_hull_with_collinear.hpp
---

## API

- `std::vector<Point> convex_hull_with_collinear(std::vector<Point> points)`: 凸包の境界上にある入力点をすべて返す。

## 引数

`points` は重複を含んでもよい点集合である。

## 戻り値

座標が完全に一致する重複点を除いた境界上の点を、$(x, y)$ の辞書順で最小の点から反時計回りに返す。全点が共線なら辞書順に返す。

## API別の時間計算量・空間計算量

- `convex_hull_with_collinear`: 点数を $N$、出力点数を $H$ として、時間 $O(N \log N)$、作業領域 $O(N)$、出力 $O(H)$。

## 注意点

- 空集合と 1 点にも対応する。
- 辺上の中間の共線点を除く場合は `convex_hull` を使う。
