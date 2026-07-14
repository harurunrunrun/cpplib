---
title: Fibonacci Range Sum (Fibonacci列の区間和) [FIBOSUM]
documentation_of: ../src/algorithm/math/fibonacci_range_sum.hpp
---

Fibonacci列の閉区間和を高速倍加法で求める。

# `fibonacci_range_sum`

```cpp
uint64_t fibonacci_range_sum(
    uint64_t left,
    uint64_t right,
    uint64_t modulus = 1'000'000'007
);
```

`F_0=0, F_1=1` として `sum(F_i, left <= i <= right) mod modulus` を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `fibonacci_range_sum` | $O(\log right)$ | $O(\log right)$ |

## 注意点

- `left <= right` かつ法が正でなければならない。
