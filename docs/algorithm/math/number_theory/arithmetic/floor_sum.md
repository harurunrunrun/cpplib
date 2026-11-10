---
title: Floor Sum (床関数和) [sum_of_floor_of_linear]
documentation_of: ../../../../../src/algorithm/math/number_theory/arithmetic/floor_sum.hpp
---

## 関数

```cpp
long long math::floor_sum(long long n, long long m, long long a, long long b)
```

$$
\sum_{i=0}^{n-1} \left\lfloor \frac{ai+b}{m} \right\rfloor
$$

を返す。`n >= 0`, `m > 0` でなければならず、満たさない場合は `std::runtime_error` を送出する。`a`, `b` は負でもよい。中間値は 128 bit 整数で計算し、答えが `long long` に収まらない場合は `std::overflow_error` を送出する。

## 時間計算量

| API | 時間計算量 | 補足 |
| --- | --- | --- |
| `math::floor_sum(n, m, a, b)` | $O(\log m)$ | Euclid の互除法型の反復。追加領域は $O(1)$。 |

## 注意点

- 引数の条件違反は `std::runtime_error`、結果（または中間値）が範囲を超える場合は `std::overflow_error` である。
