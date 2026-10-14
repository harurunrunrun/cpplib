---
title: Restricted Stirling Numbers (最小巡回長・最小ブロックサイズ付き Stirling 列)
documentation_of: ../../../../src/algorithm/math/combinatorics/restricted_stirling_numbers.hpp
---

`restricted_stirling_numbers_first_kind_fixed_k<MOD>(N,K,r)` は、`n=0,...,N` のそれぞれについて、`n` 個のラベル付き要素をちょうど `K` 個の巡回に分け、すべての巡回長を `r` 以上とする順列の個数を返す。第一種は符号なし。

`restricted_stirling_numbers_second_kind_fixed_k<MOD>(N,K,r)` は、すべてのブロックサイズを `r` 以上とする、ちょうど `K` 個の非空ブロックへの集合分割数を返す。`r=1` で通常の Stirling 数、`r=2` で固定点のない順列や一要素ブロックのない分割となる。

## 時間計算量

指数型母関数の係数列を形式的冪級数の冪で求める。第一種の母関数は `((sum_{j>=r} x^j/j)^K)/K!`、第二種は `((sum_{j>=r} x^j/j!)^K)/K!`。各係数に `n!` を掛ける。多項式乗算時間を $M(N)$ として $O(M(N))$ 時間、$O(N)$ 追加領域。`K=0` または `K*r>N` の場合は $O(N)$ 時間で返す。

## 注意点

GCC 13 / C++20。`MOD` は素数、`N<MOD`、`r>=1` が必要で、使用する NTT 長を法が許容すること。戻り値の長さは常に `N+1`。`K=0` は空集合のみを1通りと数え、それ以外は0。`K>N` など実現不可能な条件はゼロ列を返す。
