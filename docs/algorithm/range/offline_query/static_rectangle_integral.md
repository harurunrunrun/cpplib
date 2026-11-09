---
title: Static Rectangle Integral (長方形密度の静的積分) [static_rectangle_add_rectangle_sum]
documentation_of: ../../../../src/algorithm/range/offline_query/static_rectangle_integral.hpp
---

`WeightedRectangle<T>{left, lower, right, upper, weight}` の各長方形上に一定の密度 `weight` を置き、それらを重ね合わせた密度関数を、
`RectangleIntegralQuery{left, lower, right, upper}` の各領域で積分する。
`static_rectangle_integral(rectangles, queries)` は、積分値を照会順に並べた `std::vector<T>` を返す。

整数格子上の単位セルの和と見ても、連続平面上の面積分と見ても同じ値になる。
負の座標・負の密度・空の長方形を許す。座標の型は `int64_t` である。

## 時間計算量

長方形数を $N$、照会数を $Q$ とすると $O((N+Q)\log(N+Q+1))$ 時間、$O(N+Q)$ 空間である。
x 座標方向の走査と、y 座標方向の Fenwick 木を用いる。

## 注意点

- 区間は半開で、`left <= right` かつ `lower <= upper` を満たす必要がある。逆順の場合は `std::invalid_argument` を送出する。
- `T` は `int64_t` からの構築、加算・減算・乗算・単項マイナス、`+=` を提供する必要がある。
- 座標の積を含むすべての中間値が `T` に収まる必要がある。
