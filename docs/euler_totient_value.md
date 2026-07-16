---
title: Euler Totient Value (オイラーのトーシェント値) [ETF]
documentation_of: ../src/algorithm/math/euler_totient_value.hpp
---

正整数の Euler totient を64 bit素因数分解から求める。

## `euler_totient_value`

```cpp
uint64_t euler_totient_value(uint64_t n);
```

`1 <= x <= n` のうち `gcd(x,n)=1` となる `x` の個数を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `euler_totient_value` | 素因数分解時間（Pollard's rho の期待計算量） | $O(\log n)$ |

## 注意点

- `n >= 1` が必要である。
