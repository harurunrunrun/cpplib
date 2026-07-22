---
title: 2D Integer Point (二次元整数点)
documentation_of: ../src/algorithm/geometry/2d/core/integer_point.hpp
---

## API

`IntegerPoint<Coordinate>{x, y}`

## 引数

`Coordinate` は整数型、`x`, `y` は座標である。

## 戻り値

`operator==` は座標一致、`operator<` は辞書順比較の結果を返す。

## API別の時間計算量・空間計算量

- 構築・比較: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

浮動小数点用 `Point` とは暗黙変換しない。
