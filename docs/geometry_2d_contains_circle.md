---
title: 2D Circle Containment (二次元円盤の点包含)
documentation_of: ../src/algorithm/geometry/2d/contains_circle.hpp
---

## API

- `contains_circle(circle, point)`: 点が閉円盤に含まれるか判定する。

## 引数

`circle` は中心と非負半径を持つ円、`point` は判定点である。

## 戻り値

内部または円周上なら `true`、外部なら `false`。

## API別の時間計算量・空間計算量

時間・追加領域 $O(1)$。

## 注意点

負半径には `std::invalid_argument` を送出し、境界判定には許容誤差を用いる。
