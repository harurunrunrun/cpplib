---
title: Normalized Equation from a Line (直線から作る正規化方程式)
documentation_of: ../../../../src/algorithm/geometry/2d/normalized_line_line.hpp
---

2点で表した直線を、一意な向きの単位法線を持つ方程式へ変換する。

## API

```cpp
NormalizedLine normalized_line(const Line& line);
```

## 引数・戻り値

`line` は異なる2点で定めた直線。
同じ直線を表す `a*x + b*y + c = 0` の係数を返す。
`hypot(a, b) = 1` かつ `a > 0` または `a == 0 && b >= 0` となる向きを選ぶ。

## 時間計算量

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `normalized_line(line)` | $O(1)$ | $O(1)$ |

## 注意点

退化直線では `std::invalid_argument` を送出する。
係数は `long double` であり、平方根と除算の丸め誤差を含む。
