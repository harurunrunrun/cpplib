---
title: 2D Point on Circle (二次元円周上の点判定)
documentation_of: ../src/algorithm/geometry/2d/predicate/on_circle.hpp
---

## API

- `on_circle(circle, point)`: 点が円周上にあるか判定する。

## 引数

`circle` は中心と非負半径を持つ円、`point` は判定点である。

## 戻り値

中心からの距離が半径と等しければ `true`。

## API別の時間計算量・空間計算量

時間・追加領域 $O(1)$。

## 注意点

負半径には `std::invalid_argument` を送出し、等値判定には許容誤差を用いる。
