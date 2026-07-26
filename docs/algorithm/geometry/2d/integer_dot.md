---
title: 2D Integer Dot Product Aggregator (二次元整数内積集約ヘッダ)
documentation_of: ../../../../src/algorithm/geometry/2d/integer_dot.hpp
---

厳密整数型と整数点入力の内積APIをまとめる後方互換集約ヘッダ。

## 構成

| leaf header | 提供するAPI |
| --- | --- |
| `geometry_integer_wide.hpp` | `GeometryIntegerWide` |
| `integer_dot_integer_points.hpp` | `integer_dot(first, second)` |

## 集約されるAPI

```cpp
GeometryIntegerWide integer_dot(first, second);
```

## API別の時間計算量・空間計算量

| API・操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 64-bit以下の座標の `integer_dot` | $O(1)$ | $O(1)$ |

## 注意点

既存コードはこのヘッダを変更せず利用できる。
