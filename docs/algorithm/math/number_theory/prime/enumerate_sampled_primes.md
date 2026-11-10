---
title: Segmented Sampled Prime Enumeration (区間篩による素数の間引き) [enumerate_primes]
documentation_of: ../../../../../src/algorithm/math/number_theory/prime/enumerate_sampled_primes.hpp
---

## API

- `math::enumerate_sampled_primes(limit, stride = 1, offset = 0)`: `limit` 以下の素数の総数 `count`（`std::uint64_t`）と、0 始まりの素数の番号を `stride` で割った余りが `offset` である素数の列 `primes`（`std::vector<int>`）を、`SampledPrimeEnumeration` として返す。引数はすべて `int`。

素数列をすべて保持せずに $K$ 番目ごとに取り出す用途を想定する。`stride = 1` とすれば通常の全列挙になる。

## 時間計算量・空間計算量

奇数のみを扱う区間エラトステネス篩を用いる。$N$ を `limit` として、時間 $O(N \log \log(N+2))$、出力を除く領域 $O(\sqrt N)$。各基底素数について次に消す倍数を保持し、ブロックごとの切り上げ除算を避ける。

## 注意点

- $\mathrm{limit} \ge 0$、$\mathrm{stride} > 0$、$0 \le \mathrm{offset} < \mathrm{stride}$ が必要で、違反時は `std::invalid_argument` を送出する。
- GCC 13 / C++20 を想定する。
