---
title: Square-free Integer Count (平方因子を持たない整数の個数) [SQFREE]
documentation_of: ../../../../../src/algorithm/math/number_theory/multiplicative/squarefree_integer_count.hpp
---

上限以下の正の無平方数（1 より大きい平方数で割り切れない整数）の個数を数える。

## `squarefree_integer_count`

```cpp
std::uint64_t math::squarefree_integer_count(std::uint64_t n);
```

$1 \le x \le n$ で、$1$ より大きい平方数で割り切れない $x$ の個数を返す。$n = 0$ では $0$ を返す。内部では `counting_squarefrees.hpp` の `count_squarefree_numbers` を呼ぶ。

## 時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `squarefree_integer_count` | $O(n^{2/5} \log \log n)$ | $O(n^{2/5})$ |

## 注意点

- $n \le 10^{18}$ が必要で、範囲外では `std::invalid_argument` を送出する。
