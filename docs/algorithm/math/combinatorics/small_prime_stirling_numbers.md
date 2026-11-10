---
title: Small Prime Stirling Numbers (小さい素数を法とする巨大添字スターリング数) [stirling_number_of_the_first_kind_small_p_large_n] [stirling_number_of_the_second_kind_small_p_large_n]
documentation_of: ../../../../src/algorithm/math/combinatorics/small_prime_stirling_numbers.hpp
---

固定した小さい素数を法として、第一種・第二種 Stirling 数を巨大な添字に対して求める。添字は非負の `long long` で、$10^{18}$ 程度の値も扱える。

## API

`SmallPrimeStirlingNumbers(p, first_kind = true, second_kind = true)` で構築する。不要な種類の表は `false` を渡して省略できる。

- `signed_first(n, k)`: $x(x-1)\cdots(x-n+1)$ の $x^k$ の係数（符号付き第一種 Stirling 数 $s(n, k)$）。
- `unsigned_first(n, k)`: 符号なし第一種 Stirling 数。
- `second(n, k)`: 第二種 Stirling 数 $S(n, k)$。
- `surjections(n, k)`: $n$ 個の区別できる要素から $k$ 個のラベル付き箱への全射の個数 $k! \, S(n,k)$。$k \ge p$ なら $0$。
- `binomial(n, k)`: Lucas の定理による二項係数。

範囲外の添字（$k < 0$、$k > n$ など）に対しては $0$ を返す。$s(0,0)=S(0,0)=1$ である。返り値は $[0,p)$ の整数。

## 時間計算量

構築 $O(p^2)$、各クエリ $O(\log_p (n+1))$。二項係数の各桁は階乗・逆階乗の表で $O(1)$ で処理し、クエリごとに $O(p)$ の計算はしない。

## 空間計算量

構築する種類 1 つあたり $p(p+1)/2$ 個の `uint16_t` と、階乗用の $O(p)$。$p=4999$ では 1 種類で約 25 MB になる。

## 注意点

- $2 \le p \le 5000$ の素数を指定する。条件に違反すると `invalid_argument` を送出する。
- 構築を省略した種類を照会すると `logic_error` を送出する。
- 負の `n`、負の `k`、`k > n` では $0$ を返す。

巨大な添字に対する公式は [上流の実装](https://github.com/yosupo06/library-checker-problems/blob/04c8de378bab67be926325de2871f0babb8e6451/enumerative_combinatorics/stirling_number_of_the_second_kind_small_p_large_n/sol/stirling_number_query.hpp) に基づく。Apache-2.0 の全文はヘッダ末尾に記載している。
