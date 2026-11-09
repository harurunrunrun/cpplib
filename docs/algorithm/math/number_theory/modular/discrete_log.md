---
title: Discrete Log (離散対数) [discrete_logarithm_mod]
documentation_of: ../../../../../src/algorithm/math/number_theory/modular/discrete_log.hpp
---

$a^x \equiv b \pmod{m}$ を満たす最小の非負整数 $x$ を baby-step giant-step 法で求める。解が存在しない場合は $-1$ を返す。

## API

- `math::discrete_log_coprime(a, b, mod)`: $\gcd(a, \mathrm{mod}) = 1$ の場合に使う。
- `math::discrete_log(a, b, mod)`: $\gcd(a, \mathrm{mod}) \ne 1$ の場合も扱う。共通因子を順に取り除いてから `discrete_log_coprime` に帰着する。

引数・戻り値はすべて `long long`。

## 時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `math::discrete_log_coprime(a, b, mod)` | 期待 $O(\sqrt{\mathrm{mod}})$ | $O(\sqrt{\mathrm{mod}})$ |
| `math::discrete_log(a, b, mod)` | 期待 $O(\sqrt{\mathrm{mod}} + \log \mathrm{mod})$ | $O(\sqrt{\mathrm{mod}})$ |

baby-step の表に `std::unordered_map` を使うので、表の検索・挿入を期待 $O(1)$ とみなした計算量である。ハッシュの衝突が偏った場合の最悪計算量は保証しない。互いに素でない場合の前処理（共通因子の除去）は $O(\log \mathrm{mod})$ 回の反復である。

## 注意点

- どちらの API も `a`, `b` を $[0, \mathrm{mod})$ に正規化してから計算する。`mod == 1` または $b \equiv 1$ のときは $0$ を返す。
- `discrete_log_coprime` は $\gcd(a, \mathrm{mod}) = 1$ を要求し、違反時は `std::runtime_error` を送出する（ただし $b \equiv 1$ なら検査前に $0$ を返す）。
- `mod > 0` が必要で、違反時は `std::runtime_error` を送出する。
