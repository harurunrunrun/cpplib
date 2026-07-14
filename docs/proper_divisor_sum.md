---
title: Proper Divisor Sum (真の約数の総和) [DIVSUM2]
documentation_of: ../src/algorithm/math/proper_divisor_sum.hpp
---

64 bit 整数を素因数分解し、自分自身を除く正の約数の総和を返す。

# `proper_divisor_sum`

```cpp
uint64_t proper_divisor_sum(uint64_t n);
```

`sum(d, d | n, d < n)` を返す。`n = 1` の結果は `0`。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `proper_divisor_sum` | 素因数分解時間（Pollard's rho の期待計算量） | $O(\log n)$ |

## 注意点

- `n >= 1` が必要である。
- 結果が `uint64_t` に収まらない場合は `overflow_error` を送出する。
