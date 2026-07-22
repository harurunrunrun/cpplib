---
title: 2D Geometry Sign (二次元幾何の符号判定)
documentation_of: ../src/algorithm/geometry/2d/scalar/geometry_sign.hpp
---

浮動小数値を許容誤差付きで符号化する。

## API

- `geometry_sign(value)`: `value > GEOMETRY_EPS` なら1、`value < -GEOMETRY_EPS` なら-1、それ以外は0を返す。
- `geometry_scaled_sign(value, scale)`: 許容幅を `GEOMETRY_EPS * abs(scale)` とし、同じ `scale` に基づく丸め誤差も加えて符号化する。
- `geometry_scaled_sign(value, tolerance_scale, roundoff_scale)`: 幾何的許容幅と丸め誤差のscaleを個別に指定して符号化する。

## API別の時間計算量・空間計算量

- 全overload: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

`geometry_sign` は `GEOMETRY_EPS` による絶対誤差判定である。
`geometry_scaled_sign(value, tolerance_scale, roundoff_scale)` の許容幅は
`GEOMETRY_EPS * abs(tolerance_scale) + 64 * epsilon(long double) * abs(roundoff_scale)`
である。外積・内積のように入力scaleで次数が変わる量にはscale付きoverloadを使う。
