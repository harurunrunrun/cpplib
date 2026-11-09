---
title: 2D Integer Cross Product (二次元整数外積)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/scalar/integer_cross.hpp
---

同じ整数座標型を持つ 2 つの二次元ベクトルの外積を厳密に求める。

## API

```cpp
template<std::integral Coordinate>
GeometryIntegerWide integer_cross(
    const IntegerPoint<Coordinate>& first,
    const IntegerPoint<Coordinate>& second
);
```

## 戻り値

`first.x * second.y - first.y * second.x` の厳密値を `GeometryIntegerWide`（`ExactInteger`）で返す。

## 計算量

64 bit 以下の座標では時間 $O(1)$、追加空間 $O(1)$。

## 注意点

各座標を符号付き 128 bit 整数へ拡張してから `GeometryIntegerWide` で積を取るため、`long long` の全範囲を受理し、途中計算で符号付き整数のオーバーフローを起こさない。
