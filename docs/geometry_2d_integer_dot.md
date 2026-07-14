---
title: 2D Integer Dot Product (二次元整数内積)
documentation_of: ../src/algorithm/geometry/2d/integer_dot.hpp
---

## API

`GeometryIntegerWide integer_dot(first, second)`

## 引数

同じ整数座標型の2ベクトルを渡す。

## 戻り値

正確な内積を `boost::multiprecision::int256_t` で返す。

## API別の時間計算量・空間計算量

- `integer_dot`: 固定256 bit演算で時間 $O(1)$、追加領域 $O(1)$。

## 注意点

各座標を `__int128` へ拡張してから256 bit積を取り、`long long` の最小値・最大値を含む入力でも符号付きoverflowを起こさない。
