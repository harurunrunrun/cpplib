---
title: Nearest Golden Ratio Power (黄金比冪の最近整数) [POWERPHI]
documentation_of: ../../../../src/algorithm/math/sequence/nearest_golden_ratio_power.hpp
---

黄金比の非負整数冪に最も近い整数を、指定した法で求める。
$n \ge 2$ では $\varphi^n$ に最も近い整数は Lucas 数 $L_n = 2F_{n+1} - F_n$ に等しいので、Fibonacci 数を fast doubling で求めて計算する。

## `math::nearest_golden_ratio_power_mod`

```cpp
std::uint64_t math::nearest_golden_ratio_power_mod(
    std::uint64_t exponent,
    std::uint64_t modulus = 1'000'000'007
);
```

$\varphi = (1+\sqrt{5})/2$ として、$\varphi^{\mathrm{exponent}}$ に最も近い整数を `modulus` で割った余りを返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `nearest_golden_ratio_power_mod` | $O(\log \mathrm{exponent})$ | $O(\log \mathrm{exponent})$（再帰の深さ） |

## 注意点

- `modulus` は正でなければならない。$0$ を渡すと `std::invalid_argument` を送出する。
- $0$ 乗の答えは $1$、$1$ 乗の答えは $2$ である。
