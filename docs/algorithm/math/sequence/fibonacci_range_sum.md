---
title: Fibonacci Range Sum (Fibonacci 列の区間和) [FIBOSUM]
documentation_of: ../../../../src/algorithm/math/sequence/fibonacci_range_sum.hpp
---

Fibonacci 列の閉区間の和を、指定した法で返す。
$\sum_{i=0}^{k} F_i = F_{k+2} - 1$ を用い、$F_n$ は fast doubling で求める。

## API

```cpp
std::uint64_t math::fibonacci_range_sum(
    std::uint64_t left,
    std::uint64_t right,
    std::uint64_t modulus = 1'000'000'007
);
```

## 引数・戻り値

- `left`, `right`: 0-indexed の閉区間 `[left, right]`。
- `modulus`: 正の法。省略時は $10^9+7$。
- 戻り値: $F_0 = 0$, $F_1 = 1$ としたときの $\sum_{i=\mathrm{left}}^{\mathrm{right}} F_i \bmod \mathrm{modulus}$。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `fibonacci_range_sum(left, right, modulus)` | $O(\log(\mathrm{right}+2))$ | $O(\log(\mathrm{right}+2))$（再帰の深さ） |

## 注意点

- `left > right` または `modulus == 0` なら `std::invalid_argument` を送出する。
- `right <= UINT64_MAX - 2` かつ $\mathrm{modulus} \le 2^{63}$ を前提とする。
