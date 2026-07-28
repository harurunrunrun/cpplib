---
title: Geometry Sign with Scaled Tolerance (scale付き許容誤差による幾何符号判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/geometry_scaled_sign.hpp
---

幾何的な入力scaleと丸め誤差scaleに応じた許容幅で値を-1、0、1へ符号化する。

旧ヘッダは2引数wrapperと3引数coreの両leafを提供する互換aggregatorである。

## API

```cpp
int geometry_scaled_sign(long double value, long double scale);
int geometry_scaled_sign(
    long double value,
    long double tolerance_scale,
    long double roundoff_scale
);
```

3引数overloadの許容幅は
`GEOMETRY_EPS * abs(tolerance_scale) + 64 * epsilon(long double) * abs(roundoff_scale)`。
2引数overloadは同じ `scale` を両scaleへ渡す。

## 時間計算量

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 両overload | $O(1)$ | $O(1)$ |

## 注意点

scaleの符号は結果に影響しない。`GEOMETRY_EPS` は `constants.hpp` の共通inline定数を使う。
valueまたは計算後の許容幅がNaNなら、比較結果により0を返す。
