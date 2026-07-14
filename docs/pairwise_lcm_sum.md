---
title: Pairwise LCM Sum (全ペアLCM総和) [PRLCM]
documentation_of: ../src/algorithm/math/pairwise_lcm_sum.hpp
---

正整数列の異なる添字ペアすべてについて LCM を合計する。

# `pairwise_lcm_sum`

```cpp
int64_t pairwise_lcm_sum(
    const vector<int>& values,
    int64_t modulus = 998'244'353
);
```

`sum(lcm(values[i],values[j]), i<j) mod modulus` を返す。

## API別の時間計算量・空間計算量

`M=max(values)` とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `pairwise_lcm_sum` | $O(M\log M + M\log modulus)$ | $O(M)$ |

## 注意点

- 値は正で、値と法は互いに素でなければならない。既定法では `values[i] < 998244353` を前提とする。
- 逆元計算のため法は素数を前提とする。
