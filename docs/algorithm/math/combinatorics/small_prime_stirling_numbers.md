---
title: Small Prime Stirling Numbers (小さい素数を法とする巨大添字スターリング数) [stirling_number_of_the_first_kind_small_p_large_n] [stirling_number_of_the_second_kind_small_p_large_n]
documentation_of: ../../../../src/algorithm/math/combinatorics/small_prime_stirling_numbers.hpp
---

固定した小さい素数を法として、第1種・第2種スターリング数を大きい添字で照会する。添字は `long long` の非負整数で、$10^{18}$ にも対応する。

## API

`SmallPrimeStirlingNumbers(p, first_kind=true, second_kind=true)` を構築する。不要な種類の表は `false` で省略できる。

- `signed_first(n, k)`: $x(x-1)\cdots(x-n+1)$ の $x^k$ の係数。
- `unsigned_first(n, k)`: 符号なし第1種スターリング数。
- `second(n, k)`: 第2種スターリング数。
- `surjections(n, k)`: $n$ 個の区別できる要素から $k$ 個のラベル付き箱への全射の個数 $k!S(n,k)$。
- `binomial(n, k)`: Lucas の定理による二項係数。

存在しない列番号に対して0を返す。$s(0,0)=S(0,0)=1$。戻り値は $[0,p)$ の整数。

## 時間計算量

構築 $O(p^2)$、各照会 $O(\log_p(n+1))$。二項係数の小さい桁は階乗・逆階乗で $O(1)$ に処理し、各照会で $O(p)$ の計算をしない。

## 空間計算量

構築する種類1個あたり $p(p+1)/2$ 個の `uint16_t` と、階乗用の $O(p)$。$p=4999$ の1種類で約25 MB。

## 注意点

$2 \le p \le 5000$ の素数を指定する。条件違反は `invalid_argument`。構築を省略した種類を照会すると `logic_error`。負の `n`、負の `k`、`k > n` は0を返す。

巨大添字の公式は [上流の実装](https://github.com/yosupo06/library-checker-problems/blob/04c8de378bab67be926325de2871f0babb8e6451/enumerative_combinatorics/stirling_number_of_the_second_kind_small_p_large_n/sol/stirling_number_query.hpp) に基づく。Apache-2.0 の全文はヘッダ末尾に記載している。
