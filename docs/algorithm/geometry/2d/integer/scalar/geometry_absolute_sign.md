---
title: Geometry Sign with Absolute Tolerance (絶対許容誤差による幾何符号判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/integer/scalar/geometry_absolute_sign.hpp
---

浮動小数値を固定の絶対許容誤差で-1、0、1へ符号化する。

## API

```cpp
int geometry_sign(long double value);
```

`value > GEOMETRY_EPS` なら1、`value < -GEOMETRY_EPS` なら-1、
それ以外なら0を返す。

## 時間計算量

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `geometry_sign(value)` | $O(1)$ | $O(1)$ |

## 注意点

`GEOMETRY_EPS` は `constants.hpp` で一度だけ定義される共通inline定数である。
NaNは0、正負の無限大はそれぞれ1、-1を返す。
