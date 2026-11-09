---
title: 2D Circle Containment (二次元円盤の点包含)
documentation_of: ../../../../../../src/algorithm/geometry/2d/predicate/circle_point/contains_circle.hpp
---

## API

- `bool contains_circle(circle, point)`: 点が閉円盤に含まれるか判定する。

## 引数

`circle` は中心と非負の半径を持つ円、`point` は判定する点である。

## 戻り値

内部または円周上なら `true`、外部なら `false`。

## API別の時間計算量・空間計算量

- `contains_circle`: 時間・追加領域 $O(1)$。

## 注意点

- 非有限値または負の半径には `std::invalid_argument` を送出する。
- 中心からの距離と半径の差を絶対許容誤差 `GEOMETRY_EPS` で比較し、境界を判定する。
