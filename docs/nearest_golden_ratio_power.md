---
title: Nearest Golden Ratio Power (黄金比冪の最近整数) [POWERPHI]
documentation_of: ../src/algorithm/math/nearest_golden_ratio_power.hpp
---

黄金比の非負整数冪に最も近い整数を、指定した法で求める。

# `nearest_golden_ratio_power_mod`

```cpp
uint64_t nearest_golden_ratio_power_mod(
    uint64_t exponent,
    uint64_t modulus = 1'000'000'007
);
```

`((1 + sqrt(5)) / 2) ^ exponent` に最も近い整数を `modulus` で割った余りを返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `nearest_golden_ratio_power_mod` | $O(\log exponent)$ | $O(\log exponent)$ |

## 注意点

- `modulus` は正でなければならない。0を渡すと `std::invalid_argument` を送出する。
- 0乗の答えは1、1乗の答えは2である。
