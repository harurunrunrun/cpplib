---
title: Binomial Tables (二項係数と多項係数の前計算) [binomial_coefficient] [binomial_coefficient_prime_mod]
documentation_of: ../../../../src/algorithm/math/combinatorics/binomial_tables.hpp
---

`math::BinomialPrimeTable(prime, maximum_digit).binomial(n,k)` はLucasの定理で二項係数を求める。`math::BinomialCompositeTable(modulus).binomial(n,k)` は素数冪ごとに素因子を除いた階乗とその逆数を前計算し、CRTで復元する。後者の `multinomial(counts)` は各種類の個数を指定した並べ方の総数を返す。

## 時間計算量

素数版は前計算 $O(D+\log p)$・空間 $O(D)$、照会 $O(\log_p(n+1))$。合成数版は法の素因数分解を除く前計算 $O(\sum p_i^{e_i})$・空間 $O(\sum p_i^{e_i})$、照会 $O(\sum\log_{p_i}(n+1))$。多項係数は種類数回の二項係数照会。

## 注意点

法は正の32 bit整数。合成数版は法に比例する表を持つので、法が大きい場合のメモリ使用量に注意する。素数版は必要な各Lucas桁を表の範囲へ収める。`k>n` は0、法1は常に0。個数の和が `uint64_t` を超えると `overflow_error`。
