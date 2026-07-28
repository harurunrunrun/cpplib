---
title: Geometry Integer Wide Type (幾何用多倍長整数型)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/geometry_integer_wide.hpp
---

整数幾何の中間値と厳密な結果に用いる多倍長符号付き整数型。

## API

```cpp
using GeometryIntegerWide = ExactInteger;
```

## 時間計算量

値のbit長を $L$ とする。

| 操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 型aliasの参照 | $O(1)$ | $O(1)$ |
| 加減算 | $O(L)$ | $O(L)$ |
| 乗算 | $O(L^2)$ | $O(L)$ |

## 注意点

固定幅整数へ暗黙に切り詰めない。必要な演算は `ExactInteger` の契約に従う。
