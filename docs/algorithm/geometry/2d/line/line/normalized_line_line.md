---
title: Normalized Equation from a Line (直線から作る正規化方程式)
documentation_of: ../../../../../../src/algorithm/geometry/2d/line/line/normalized_line_line.hpp
---

2 点で表した直線を、向きが一意に定まる単位法線を持つ方程式に変換する。

## API

```cpp
NormalizedLine normalized_line(const Line& line);
```

## 引数・戻り値

`line` は相異なる 2 点で定めた直線である。
同じ直線を表す方程式 $ax + by + c = 0$ の係数 `a`, `b`, `c` を返す。
係数は $\sqrt{a^2 + b^2} = 1$ を満たし、$a \gt 0$、または $a = 0$ かつ $b \ge 0$ となる向きを選ぶ。

## 時間計算量

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `normalized_line(line)` | $O(1)$ | $O(1)$ |

## 注意点

方向ベクトルの長さが `GEOMETRY_EPS` 以下の退化直線では `std::invalid_argument` を送出する。
係数は `long double` であり、平方根と除算の丸め誤差を含む。
