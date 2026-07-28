---
title: Dot Product of Integer Points (整数点ベクトルの内積)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/scalar/integer_dot_integer_points.hpp
---

同じ整数座標型を持つ2つの二次元ベクトルの内積を厳密に求める。

## API

```cpp
template<std::integral Coordinate>
GeometryIntegerWide integer_dot(
    const IntegerPoint<Coordinate>& first,
    const IntegerPoint<Coordinate>& second
);
```

## 引数・戻り値

`first` と `second` に同じ `Coordinate` 型の整数点ベクトルを渡す。
`first.x * second.x + first.y * second.y` の厳密値を返す。

## 時間計算量

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 64-bit以下の座標の `integer_dot` | $O(1)$ | $O(1)$ |

## 注意点

各座標を符号付き128-bit整数へ拡張してから `GeometryIntegerWide` で積を取る。
`long long` の最小値・最大値を含む入力でも符号付きoverflowを起こさない。
