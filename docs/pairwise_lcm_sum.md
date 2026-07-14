---
title: Pairwise LCM Sum (全ペアLCM総和) [PRLCM]
documentation_of: ../src/algorithm/math/pairwise_lcm_sum.hpp
---

正整数列の異なる添字ペアすべてについて LCM を合計する。

# `pairwise_lcm_sum`

```cpp
int64_t pairwise_lcm_sum(
    const vector<int>& values
);
```

`sum(lcm(values[i],values[j]), i<j) mod 998244353` を返す。

## 引数

- `values`: LCMを集計する正整数列。

## 戻り値

異なる添字ペアのLCM総和を998244353で割った余り。

## API別の時間計算量・空間計算量

`M=max(values)` とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `pairwise_lcm_sum` | $O(M\log M + M\log 998244353)$ | $O(M)$ |

## 注意点

- 値は正で、998244353 と互いに素でなければならない。違反時は `std::invalid_argument` を送出する。
