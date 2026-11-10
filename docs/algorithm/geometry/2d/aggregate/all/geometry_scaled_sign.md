---
title: Geometry Sign with Scaled Tolerance (scale付き許容誤差による幾何符号判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/geometry_scaled_sign.hpp
---

入力の幾何的なスケールと丸め誤差のスケールに応じた許容幅で、値の符号を $-1, 0, 1$ に判定する。

このヘッダは、2 引数のラッパーと 3 引数の本体の両 leaf header を提供する互換の集約ヘッダである。

## API

```cpp
int geometry_scaled_sign(long double value, long double scale);
int geometry_scaled_sign(long double value, long double tolerance_scale, long double roundoff_scale);
```

3 引数版の許容幅は `GEOMETRY_EPS * abs(tolerance_scale) + 64 * epsilon(long double) * abs(roundoff_scale)` である。`value` がこの許容幅より大きければ $1$、許容幅の符号反転より小さければ $-1$、それ以外は $0$ を返す。2 引数版は同じ `scale` を両方のスケールとして渡す。

## 計算量

| API | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| 両オーバーロード | $O(1)$ | $O(1)$ |

## 注意点

- スケールの符号は結果に影響しない。`GEOMETRY_EPS` は `constants.hpp` の共通の `inline` 定数を使う。
- `value` または計算した許容幅が NaN なら、比較がすべて偽になるため $0$ を返す。
