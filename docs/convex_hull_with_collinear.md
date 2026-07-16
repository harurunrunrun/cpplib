---
title: Convex Hull with Collinear Points (共線点を残す凸包) [CGL_4_A]
documentation_of: ../src/algorithm/geometry/2d/convex_hull_with_collinear.hpp
---

## API

- `convex_hull_with_collinear(points)`: 凸包境界上の入力点をすべて返す。

## 引数

`points` は重複を含んでもよい点集合である。

## 戻り値

重複を除いた境界点を反時計回りに返す。全点共線なら辞書順に返す。

## API別の時間計算量・空間計算量

点数を $N$、出力点数を $H$ として、時間 $O(N\log N)$、作業領域 $O(N)$、出力 $O(H)$。

## 注意点

空集合と1点にも対応する。辺上の中間共線点を除く場合は `convex_hull` を使う。
