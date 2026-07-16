---
title: Discrete Log (離散対数) [discrete_logarithm_mod]
documentation_of: ../src/algorithm/math/discrete_log.hpp
---

離散対数問題。

`a^x = b (mod m)` となる最小の非負整数 `x` を返す。存在しない場合は `-1`。

## 関数

- `math::discrete_log_coprime(a, b, mod)`
    - `gcd(a, mod) = 1` の場合。
- `math::discrete_log(a, b, mod)`
    - `gcd(a, mod) != 1` の場合も処理する。

## 計算量の概要

- $O(\sqrt {mod})$

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `math::discrete_log_coprime(a,b,mod)` | 期待 $O(\sqrt{mod})$ | $O(\sqrt{mod})$ |
| `math::discrete_log(a,b,mod)` | 期待 $O(\sqrt{mod}+\log mod)$ | $O(\sqrt{mod})$ |

baby-step giant-step の表に `unordered_map` を使うため、表の検索・挿入を期待 $O(1)$ とした計算量である。衝突が偏る場合の最悪計算量は保証しない。非互いに素な場合の前処理は $O(\log mod)$。

## 注意点

- 両APIとも `a`, `b` を $[0,mod)$ に正規化し、
  $a^x\equiv b\pmod {mod}$ を満たす最小の非負整数 $x$ を返す。
  `mod==1` では0、解がなければ-1。
- `discrete_log_coprime` は $\gcd(a,mod)=1$ を要求し、違反時は例外。
- `discrete_log` は互いに素でない場合もgcdを順に除去して処理する。
- `mod>0` が必要で、違反時は `runtime_error`。
