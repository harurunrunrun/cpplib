---
title: Geometry Scaled Sign with Separate Scales (個別スケールによる幾何符号判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/integer/scalar/geometry_scaled_sign_value_tolerance_roundoff_scales.hpp
---

## API

- `int geometry_scaled_sign(long double value, long double tolerance_scale, long double roundoff_scale)`: 幾何許容誤差用のスケールと丸め誤差用のスケールを個別に指定し、`value` の符号を $-1, 0, 1$ で返す。

許容幅は、$\varepsilon$ を `long double` の機械イプシロンとして、$\texttt{GEOMETRY_EPS} \cdot \lvert t \rvert + 64 \varepsilon \lvert r \rvert$ である（$t$ は `tolerance_scale`、$r$ は `roundoff_scale`）。

## API別の時間計算量・空間計算量

時間・追加領域とも $O(1)$。

## 注意点

両スケールの符号は結果に影響しない。`value` が許容幅またはその符号を反転した値と等しい場合は 0 を返す。`value` または計算後の許容幅が NaN の場合は、比較がすべて偽になるため 0 を返す。
