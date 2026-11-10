---
title: Pisano Period (Pisano周期) [PISANO]
documentation_of: ../../../../../src/algorithm/math/number_theory/modular/pisano_period.hpp
---

フィボナッチ数列を法 $m$ で見たときの最小正周期（Pisano 周期）を返す。

## `pisano_period`

```cpp
std::uint64_t math::pisano_period(std::uint64_t modulus);
```

$(F_k \bmod m, F_{k+1} \bmod m) = (0, 1)$ となる最小の正整数 $k$ を返す（$m$ は `modulus`）。

## アルゴリズム

$m$ を素因数分解し、各素数冪 $p^e$ について周期の倍数（$p = 2$ なら $3 \cdot 2^{e-1}$、$p = 5$ なら $20 \cdot 5^{e-1}$、$5$ が平方剰余なら $(p-1)p^{e-1}$、そうでなければ $2(p+1)p^{e-1}$）を求め、その素因数で割れる限り割って最小周期を得る。周期の判定はダブリングによるフィボナッチ数の計算で行う。最後に各素数冪の周期の最小公倍数をとる。

## 時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `pisano_period` | 法と周期候補の素因数分解の時間 + 候補の素因数ごとに $O(\log k)$ | $O(\log m)$ |

## 注意点

- $2 \le m \le 10^{12}$ が必要である。$m < 2$ では `std::invalid_argument`、$m > 10^{12}$ では `std::out_of_range` を送出する。
