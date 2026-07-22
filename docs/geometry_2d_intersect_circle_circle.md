---
title: 2D Circle-Circle Intersection Test (二次元円同士の交差判定)
documentation_of: ../src/algorithm/geometry/2d/predicate/intersect_circle_circle.hpp
---

## API

- `intersect_circle_circle(first, second)`: 2円周が少なくとも1点を共有するか判定する。

## API別の時間計算量・空間計算量

- `intersect_circle_circle`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

接円と一致する円も真。負半径には `std::invalid_argument` を送出する。

- 円の中心座標と半径は有限値であること。非有限値または負半径にはstd::invalid_argumentを送出する。
- 中心距離と半径和・半径差は入力scaleに対する相対許容誤差で比較する。
