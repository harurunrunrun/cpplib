---
title: Small and Huge Integer GCD (小整数と巨大整数のGCD) [GCD2]
documentation_of: ../../../../../src/algorithm/math/number_theory/arithmetic/gcd_small_huge_integer.hpp
---

機械語に収まる整数と、十進文字列で与えた巨大な整数の最大公約数を求める。

## `gcd_small_huge_integer`

```cpp
string math::gcd_small_huge_integer(uint32_t small, const string& huge);
```

$\gcd(\mathrm{small}, \mathrm{huge})$ を、先頭に余分な 0 を含まない十進表記で返す。`huge` を 1 桁ずつ読んで `small` での剰余を求め、`std::gcd` を 1 回呼ぶ。`small == 0` では `huge` の先頭の 0 を除いた値（すべて 0 なら `"0"`）を返す。

## 時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `gcd_small_huge_integer` | $O(\lvert \mathrm{huge}\rvert+\log \mathrm{small})$ | $O(\lvert \mathrm{huge}\rvert)$ |

## 注意点

- `huge` は空でない符号なしの十進表記でなければならない。空文字列や数字以外の文字を含む場合は `std::invalid_argument` を送出する。
