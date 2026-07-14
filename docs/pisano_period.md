---
title: Pisano Period (Pisano周期) [PISANO]
documentation_of: ../src/algorithm/math/pisano_period.hpp
---

Fibonacci列を法で見たときの最小正周期を返す。

# `pisano_period`

```cpp
uint64_t pisano_period(uint64_t modulus);
```

`(F_k,F_(k+1)) mod modulus = (0,1)` となる最小の正整数 `k` を返す。

## 引数

- `modulus`: Fibonacci列を還元する法。

## 戻り値

`modulus` に対するPisano周期。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `pisano_period` | 法と周期候補の素因数分解時間、および候補因数ごとに $O(\log k)$ | $O(\log modulus)$ |

## 注意点

- `2 <= modulus <= 10^12` が必要である。下限違反では `std::invalid_argument`、上限違反では `std::out_of_range` を送出する。
