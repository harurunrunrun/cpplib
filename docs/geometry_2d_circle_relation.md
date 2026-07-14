---
title: 2D Circle Relation (二次元円の位置関係) [CGL_7_A]
documentation_of: ../src/algorithm/geometry/2d/circle_relation.hpp
---

## API

- `circle_relation(first, second)`: `CIRCLE_INSIDE(0)`, `CIRCLE_INTERNALLY_TANGENT(1)`, `CIRCLE_INTERSECT(2)`, `CIRCLE_EXTERNALLY_TANGENT(3)`, `CIRCLE_SEPARATE(4)`, `CIRCLE_COINCIDENT(5)` のいずれかを返す。
- 上記6定数: 円同士の分類値。

## API別の時間計算量・空間計算量

- 判定と定数参照: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

負の半径には `std::invalid_argument` を送出する。接触と一致の判定には許容誤差を用いる。

- 円の中心座標と半径は有限値であること。非有限値または負半径にはstd::invalid_argumentを送出する。
- 中心距離・半径和・半径差を同じ長さscaleで比較する。同一点の半径0円同士はCIRCLE_COINCIDENT。
