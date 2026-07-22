---
title: Fibonacci Range Sum (Fibonacci列の区間和) [FIBOSUM]
documentation_of: ../src/algorithm/math/sequence/fibonacci_range_sum.hpp
---

Fibonacci列の閉区間和を指定した法で返す。

## API

```cpp
uint64_t fibonacci_range_sum(
    uint64_t left,
    uint64_t right,
    uint64_t modulus = 1'000'000'007
);
```

## 引数・戻り値

- `left`, `right`: 0始まりの閉区間 `[left,right]`。
- `modulus`: 正の法。省略時は $10^9+7$。
- 戻り値: $F_0=0,F_1=1$ とした
  $\sum_{i=left}^{right}F_i\bmod modulus$。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `fibonacci_range_sum(left,right,modulus)` | $O(\log(right+2))$ | $O(\log(right+2))$ |

## 注意点

- `left > right` または `modulus == 0` なら `invalid_argument` を送出する。
- `right <= UINT64_MAX - 2` かつ `modulus <= 2^63` を前提とする。
