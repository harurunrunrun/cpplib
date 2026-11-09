---
title: Euler Totient Value (オイラーのトーシェント値) [ETF]
documentation_of: ../../../../../src/algorithm/math/number_theory/multiplicative/euler_totient_value.hpp
---

正整数に対するオイラーのトーシェント関数の値を、64 bit 整数の素因数分解から求める。

## `euler_totient_value`

```cpp
std::uint64_t math::euler_totient_value(std::uint64_t n);
```

$1 \le x \le n$ のうち $\gcd(x, n) = 1$ を満たす $x$ の個数 $\varphi(n)$ を返す。内部では `prime_factorization.hpp` の `euler_phi` を呼ぶ。

## 時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `euler_totient_value` | 素因数分解の時間（Pollard–Rho 法の期待計算量） | $O(\log n)$ |

## 注意点

- $n \ge 1$ が必要で、`n == 0` では `std::runtime_error` を送出する。
