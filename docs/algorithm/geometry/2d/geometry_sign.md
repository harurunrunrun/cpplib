---
title: 2D Geometry Sign Aggregator (二次元幾何の符号判定集約ヘッダ)
documentation_of: ../../../../src/algorithm/geometry/2d/geometry_sign.hpp
---

絶対許容誤差とscale付き許容誤差の符号判定APIをまとめる後方互換集約ヘッダ。

## 構成

| leaf header | 提供するAPI |
| --- | --- |
| `geometry_absolute_sign.hpp` | `geometry_sign(value)` |
| `geometry_scaled_sign.hpp` | `geometry_scaled_sign(value, scale)`, `geometry_scaled_sign(value, tolerance_scale, roundoff_scale)` |

両leafは共通定数 `GEOMETRY_EPS` を `constants.hpp` から読み込み、独自には定義しない。

## 集約されるAPI

```cpp
int geometry_sign(long double value);
int geometry_scaled_sign(long double value, long double scale);
int geometry_scaled_sign(
    long double value,
    long double tolerance_scale,
    long double roundoff_scale
);
```

各APIは負、許容誤差内、正をそれぞれ-1、0、1で返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 全overload | $O(1)$ | $O(1)$ |

## 注意点

絶対値だけを比較する量には `geometry_sign`、座標scaleで次数が変わる内積・外積などには
`geometry_scaled_sign` を使う。NaNは全比較がfalseとなるため0を返す。
