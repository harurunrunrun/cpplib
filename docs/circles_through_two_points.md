---
title: Circles through Two Points (2点を通る指定半径円)
documentation_of: ../src/algorithm/geometry/2d/circles_through_two_points.hpp
---

## API

- `circles_through_two_points(first, second, radius)`: 2点を円周上に持つ指定半径円を列挙する。

## 引数

`first`, `second` は通過点、`radius` は非負半径である。

## 戻り値

中心の辞書順で0〜2個の `Circle` を返す。

## API別の時間計算量・空間計算量

時間・出力・追加領域 $O(1)$。

## 注意点

異なる2点間距離が直径を超える場合は空。同一点と正半径では解が無限にあるため `std::domain_error` を送出する。負半径には `std::invalid_argument` を送出する。
