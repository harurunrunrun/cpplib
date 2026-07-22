---
title: 2D Integer Dot Product (二次元整数内積)
documentation_of: ../src/algorithm/geometry/2d/scalar/integer_dot.hpp
---

## API

`GeometryIntegerWide integer_dot(first, second)`

## 引数

同じ整数座標型の2ベクトルを渡す。

## 戻り値

正確な内積を `GeometryIntegerWide`（`ExactInteger`）で返す。

## API別の時間計算量・空間計算量

- `integer_dot`: 64-bit以下の座標では時間 $O(1)$、追加領域 $O(1)$。

## 注意点

各座標を `__int128` へ拡張してから `ExactInteger` で積を取り、`long long` の最小値・最大値を含む入力でも符号付きoverflowを起こさない。
