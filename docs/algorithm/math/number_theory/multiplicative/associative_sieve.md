---
title: Associative Sieve Summatory Quotients (結合的篩による商ごとの総和) [ASSIEVE]
documentation_of: ../../../../../src/algorithm/math/number_theory/multiplicative/associative_sieve.hpp
---

4 つの状態からなる可換モノイドに値をとる乗法的な「状態関数」について、各 $\lfloor n/i \rfloor$ までの冪和を状態別にまとめて求める。

## `AssociativeSieveOperation`

```cpp
using AssociativeSieveOperation = std::array<std::array<std::uint8_t, 4>, 4>;
```

状態の二項演算表を表す。状態 $0$ が単位元で、演算は可換かつ結合的でなければならない。

## `AssociativeSieveValue`

```cpp
using AssociativeSieveValue = std::array<Modint998244353, 4>;
```

添字 $r$ の要素が、状態 $r$ に属する整数の冪和を表す。

## `associative_sieve_summatory_quotients`

```cpp
std::vector<math::AssociativeSieveValue> math::associative_sieve_summatory_quotients(
    std::uint64_t n,
    std::uint32_t exponent,
    const math::AssociativeSieveOperation& operation
);
```

素数冪 $p^e$ の状態を $(p \bmod 4) \cdot (e \bmod 4) \bmod 4$ とし、互いに素な数の積の状態を `operation` で合成する（$1$ の状態は $0$）。戻り値の長さは $\lfloor \sqrt n \rfloor$ で、$i-1$ 番目（$1 \le i \le \lfloor \sqrt n \rfloor$）は $1 \le v \le \lfloor n/i \rfloor$ の範囲の $v^{\mathrm{exponent}}$ を状態別に足し合わせた値である。

素数に関する部分和は、等差数列 $4k+1, 4k+3$ 上の冪和を多項式補間と多点評価で求めてから篩で計算し、その後に素数冪を逆順に掛け合わせる（min_25 篩と同様の手順）。

## 時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `associative_sieve_summatory_quotients` | $\widetilde O(n^{3/4} + \sqrt n + \mathrm{exponent}^2)$ | $O(\sqrt n \log n + \mathrm{exponent})$ |

## 注意点

- $1 \le n \le 10^{10}$、$\mathrm{exponent} \le 1000$ が必要で、違反時は `std::invalid_argument` を送出する。
- 演算表の値域（各値が $4$ 未満）・単位元・可換性・結合性を検査し、不正なら `std::invalid_argument` を送出する。
- 結果は $998244353$ を法とする。
