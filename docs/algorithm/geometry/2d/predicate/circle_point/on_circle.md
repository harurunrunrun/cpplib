---
title: 2D Point on Circle (二次元円周上の点判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/predicate/circle_point/on_circle.hpp
---

## API

- `bool on_circle(circle, point)`: 点が円周上にあるか判定する。

## 引数

`circle` は中心と非負の半径を持つ円、`point` は判定する点である。

## 戻り値

中心からの距離が半径と等しければ `true`。

## API別の時間計算量・空間計算量

- `on_circle`: 時間・追加領域 $O(1)$。

## 注意点

- 非有限値または負の半径には `std::invalid_argument` を送出する。
- 中心からの距離と半径の差を絶対許容誤差 `GEOMETRY_EPS` で比較し、等しいかを判定する。
