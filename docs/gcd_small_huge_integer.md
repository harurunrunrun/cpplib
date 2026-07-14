---
title: Small and Huge Integer GCD (小整数と巨大整数のGCD) [GCD2]
documentation_of: ../src/algorithm/math/gcd_small_huge_integer.hpp
---

機械語整数と十進文字列整数の最大公約数を求める。

# `gcd_small_huge_integer`

```cpp
string gcd_small_huge_integer(uint32_t small, const string& huge);
```

`gcd(small, huge)` の正規化済み十進表記を返す。`small=0` では `huge` の先頭0を除いた値を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `gcd_small_huge_integer` | $O(|huge|+\log small)$ | $O(|huge|)$ |

## 注意点

- `huge` は空でない符号なし十進表記でなければならない。
