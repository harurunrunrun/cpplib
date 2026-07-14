---
title: Circle-Circle Intersection Count (円同士の交点数)
documentation_of: ../src/algorithm/geometry/2d/circle_circle_intersection_count.hpp
---

## API

- `circle_circle_intersection_count(first, second)`: 2円周の交点数を返す。
- `CIRCLE_INTERSECTION_INFINITE`: 一致円を表す値 `-1`。

## 引数

`first`, `second` は非負半径の円である。

## 戻り値

0、1、2、または一致円なら `CIRCLE_INTERSECTION_INFINITE`。

## API別の時間計算量・空間計算量

関数と定数参照はいずれも時間・追加領域 $O(1)$。

## 注意点

負半径には `std::invalid_argument` を送出する。

- 円の中心座標と半径は有限値であること。非有限値または負半径にはstd::invalid_argumentを送出する。
- 接触・包含の距離二乗や4次量は、比較する各項の大きさにGEOMETRY_EPSを掛けた相対許容誤差で判定する。中間値は有限なlong doubleに収まること。
- 同一点の半径0円同士は1を返す。CIRCLE_INTERSECTION_INFINITEは一致する正半径円に限る。
