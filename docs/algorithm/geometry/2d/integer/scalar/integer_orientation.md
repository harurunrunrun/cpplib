---
title: 2D Integer Orientation (二次元整数点向き判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/integer/scalar/integer_orientation.hpp
---

## API

```cpp
template<std::integral Coordinate>
int integer_orientation(
    const IntegerPoint<Coordinate>& first,
    const IntegerPoint<Coordinate>& second,
    const IntegerPoint<Coordinate>& third
);
```

## 引数

向きを調べる、同じ整数座標型の 3 点を渡す。

## 戻り値

反時計回りなら `1`、時計回りなら `-1`、共線なら `0`。

## API別の時間計算量・空間計算量

- `integer_orientation`: 時間 $O(1)$、追加領域 $O(1)$（多倍長整数の演算を定数回行う）。

## 注意点

座標差を `__int128_t` で求め、積と差を多倍長整数 `GeometryIntegerWide`（`ExactInteger`）で計算するので、`long long` の座標差でもオーバーフローせず厳密に判定できる。
