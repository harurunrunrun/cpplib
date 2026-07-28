---
title: Geometry Scaled Sign with Separate Scales (個別scaleによる幾何符号判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/integer/scalar/geometry_scaled_sign_value_tolerance_roundoff_scales.hpp
---

## API

- `geometry_scaled_sign(value, tolerance_scale, roundoff_scale)`: 幾何許容誤差用scaleと丸め誤差用scaleを個別指定し、`value` の符号を `-1,0,1` で返す。

許容幅は `GEOMETRY_EPS * abs(tolerance_scale) + 64 * epsilon(long double) * abs(roundoff_scale)` である。

## API別の時間計算量・空間計算量

時間・追加領域 $O(1)$。

## 注意点

両scaleの符号は結果に影響しない。`value` が許容幅またはその負値と等しい場合は0を返す。`value` または計算後の許容幅がNaNなら比較結果により0を返す。
