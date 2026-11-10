---
title: 2D Integer Dot Product Aggregator (二次元整数内積集約ヘッダ)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/integer_dot.hpp
---

厳密な整数型と、整数点を入力とする内積の API をまとめた後方互換の集約ヘッダである。既存のコードはこのヘッダをそのまま利用できる。

## 構成

| leaf header | 提供する API |
| --- | --- |
| `geometry_integer_wide.hpp` | `GeometryIntegerWide`（`ExactInteger` の別名） |
| `integer_dot_integer_points.hpp` | `integer_dot(first, second)` |

## 集約される API

```cpp
template<std::integral Coordinate>
GeometryIntegerWide integer_dot(const IntegerPoint<Coordinate>& first, const IntegerPoint<Coordinate>& second);
```

内積 $x_1 x_2 + y_1 y_2$ を、オーバーフローなしに `GeometryIntegerWide` で返す。

## 計算量

| API・操作 | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| 64 bit 以下の座標での `integer_dot` | $O(1)$ | $O(1)$ |
