---
title: Static Rectangle Integral (長方形密度の静的積分) [static_rectangle_add_rectangle_sum]
documentation_of: ../../../../src/algorithm/range/offline_query/static_rectangle_integral.hpp
---

`WeightedRectangle<T>{left,lower,right,upper,weight}` を重ねた密度関数を、
`RectangleIntegralQuery{left,lower,right,upper}` の各領域で積分する。
`static_rectangle_integral(rectangles,queries)` は積分値の `vector<T>` を返す。
整数格子の単位セル和としても、連続平面の面積積分としても同じ値となる。
負の座標・負の密度・空長方形を許す。座標は `int64_t`。

## 時間計算量

長方形数 $N$、照会数 $Q$ として $O((N+Q)\log(N+Q+1))$ 時間、$O(N+Q)$ 空間。
x 座標の sweep と y 座標の Fenwick 木を使う。

## 注意点

端点は半開で `left<=right`, `lower<=upper`。逆順は `invalid_argument`。
`T` は整数座標からの構築、加減乗算を提供する。積を含む全中間値が `T` に収まる必要がある。
