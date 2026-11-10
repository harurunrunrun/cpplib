---
title: 2D Circle-Circle Intersection Test (二次元円同士の交差判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/predicate/circle_circle/intersect_circle_circle.hpp
---

## API

- `bool intersect_circle_circle(first, second)`: 2 つの円周が少なくとも 1 点を共有するか判定する。

## API別の時間計算量・空間計算量

- `intersect_circle_circle`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 接する円同士と一致する円同士も `true` を返す。一方が他方の内部に含まれて接しない場合と、離れている場合は `false` を返す。
- 円の中心座標と半径は有限値でなければならない。非有限値または負の半径には `std::invalid_argument` を送出する。
- 中心間距離と半径の和・差は、入力のスケールに対する相対許容誤差で比較する。
