---
title: Binomial Tables (二項係数と多項係数の前計算) [binomial_coefficient] [binomial_coefficient_prime_mod]
documentation_of: ../../../../src/algorithm/math/combinatorics/binomial_tables.hpp
---

二項係数と多項係数を、前計算した表を使って任意の法で求める。

## API

- `math::BinomialPrimeTable(prime, maximum_digit)`: 素数 `prime` を法とし、Lucas の定理で二項係数を求めるための表を作る。階乗表の大きさは `min(maximum_digit, prime - 1) + 1` である。
- `math::BinomialPrimeTable::binomial(n, k)`: $\binom{n}{k} \bmod p$ を返す。
- `math::BinomialCompositeTable(modulus)`: 法を素数冪に分解し、素数冪ごとに素因子を除いた階乗とその逆数を前計算する。
- `math::BinomialCompositeTable::binomial(n, k)`: 素数冪ごとに求めた値を中国剰余定理で復元して $\binom{n}{k} \bmod m$ を返す。
- `math::BinomialCompositeTable::multinomial(counts)`: 各種類の個数を `counts` で指定したときの並べ方の総数（多項係数）を法 $m$ で返す。

## 時間計算量

$p$ を素数版の法、$D$ を `maximum_digit`、合成数版の法を $m = \prod p_i^{e_i}$ とする。

- 素数版: 前計算 $O(D + \log p)$、空間 $O(D)$、`binomial` は $O(\log_p(n+1))$。
- 合成数版: 法の素因数分解を除いて前計算 $O(\sum p_i^{e_i})$、空間 $O(\sum p_i^{e_i})$、`binomial` は $O(\sum \log_{p_i}(n+1))$。
- `multinomial`: 種類数回の `binomial`。

## 注意点

法は正の 32 ビット整数である。合成数版は法に比例する大きさの表を持つため、法が大きい場合はメモリ使用量に注意する。`k > n` なら $0$ を、法が $1$ なら常に $0$ を返す。

素数版で `prime` が素数でない場合、合成数版で `modulus` が $0$ の場合は `invalid_argument` を送出する。素数版で $n$ または $k$ の $p$ 進表示の桁が表の範囲（`maximum_digit`）を超える場合は `out_of_range` を送出する。`multinomial` で個数の和が `uint64_t` を超える場合は `overflow_error` を送出する。
