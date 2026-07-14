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
