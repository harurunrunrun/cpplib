---
title: Geometry Scaled Sign with One Scale (単一スケールによる幾何符号判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/integer/scalar/geometry_scaled_sign_value_scale.hpp
---

## API

- `int geometry_scaled_sign(long double value, long double scale)`: 同じ `scale` を幾何許容誤差と丸め誤差の両方に用いて、`value` の符号を $-1, 0, 1$ で返す。

## API別の時間計算量・空間計算量

時間・追加領域とも $O(1)$。

## 注意点

3 引数版に `geometry_scaled_sign(value, scale, scale)` として処理を委ねる。`scale` の符号は結果に影響しない。`value` が許容幅の境界上にある場合は 0 を返す。
