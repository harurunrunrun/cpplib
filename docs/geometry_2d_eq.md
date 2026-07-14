---
title: 2D Floating Equality (二次元幾何の誤差付き等値判定)
documentation_of: ../src/algorithm/geometry/2d/eq.hpp
---

## API

`bool eq(long double first, long double second)`

## 引数

比較する2実数を渡す。

## 戻り値

差の絶対値が `GEOMETRY_EPS` 以下なら `true`。

## API別の時間計算量・空間計算量

- `eq`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

絶対誤差による判定であり、相対誤差判定ではない。
