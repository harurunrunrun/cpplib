---
title: 2D Integer Cross Product (二次元整数外積)
documentation_of: ../src/algorithm/geometry/2d/integer_cross.hpp
---

## API

`GeometryIntegerWide integer_cross(first, second)`

## 引数

同じ整数座標型の2ベクトルを渡す。

## 戻り値

正確な外積を `boost::multiprecision::int256_t` で返す。

## API別の時間計算量・空間計算量

- `integer_cross`: 固定256 bit演算で時間 $O(1)$、追加領域 $O(1)$。

## 注意点

`long long` の全範囲を受理し、途中計算で符号付きoverflowを起こさない。
