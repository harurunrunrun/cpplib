---
title: Circle-Circle Intersection Count (円同士の交点数)
documentation_of: ../../../../../../src/algorithm/geometry/2d/integer/circle_circle/circle_circle_intersection_count.hpp
---

## API

- `int circle_circle_intersection_count(first, second)`: 2 つの円周の交点数を返す。
- `CIRCLE_INTERSECTION_INFINITE`: 2 円が一致することを表す値 `-1`。

## 引数

`first`, `second` は非負の半径を持つ円である。

## 戻り値

0、1、2 のいずれか、または 2 円が一致する場合は `CIRCLE_INTERSECTION_INFINITE`。
`circle_relation` の結果から、包含・分離なら 0、内接・外接なら 1、2 点で交わるなら 2 を返す。

## API別の時間計算量・空間計算量

関数・定数の参照とも、時間・追加領域は $O(1)$。

## 注意点

- 円の中心座標と半径は有限値であること。非有限値または負の半径には `std::invalid_argument` を送出する。
- 接触・包含の判定は、比較する各項の大きさに `GEOMETRY_EPS` を掛けた相対許容誤差で行う。中間値は有限な `long double` に収まる必要がある。
- 同じ点を中心とする半径 0 の円同士は 1 を返す。`CIRCLE_INTERSECTION_INFINITE` を返すのは、一致する正の半径の円に限る。
