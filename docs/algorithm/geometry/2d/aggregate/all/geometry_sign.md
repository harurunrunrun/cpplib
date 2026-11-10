---
title: 2D Geometry Sign Aggregator (二次元幾何の符号判定集約ヘッダ)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/geometry_sign.hpp
---

絶対許容誤差による符号判定と、スケール付き許容誤差による符号判定の API をまとめた後方互換の集約ヘッダである。

## 構成

| leaf header | 提供する API |
| --- | --- |
| `geometry_absolute_sign.hpp` | `geometry_sign(value)` |
| `geometry_scaled_sign.hpp` | `geometry_scaled_sign(value, scale)`, `geometry_scaled_sign(value, tolerance_scale, roundoff_scale)` |

どちらの leaf header も共通定数 `GEOMETRY_EPS` を `constants.hpp` から読み込み、独自には定義しない。

## 集約される API

```cpp
int geometry_sign(long double value);
int geometry_scaled_sign(long double value, long double scale);
int geometry_scaled_sign(long double value, long double tolerance_scale, long double roundoff_scale);
```

どの API も、負・許容誤差内・正をそれぞれ $-1, 0, 1$ で返す。

## 計算量

| API | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| 全オーバーロード | $O(1)$ | $O(1)$ |

## 注意点

- 絶対値だけで比べる量には `geometry_sign` を、座標のスケールによって次数が変わる内積・外積などには `geometry_scaled_sign` を使う。
- NaN は比較がすべて偽になるため $0$ を返す。
