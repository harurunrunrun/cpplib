---
title: Small Prime Permutation Cycle Counts (巡回数と固定点数を指定した順列の個数)
documentation_of: ../../../../src/algorithm/math/combinatorics/small_prime_permutation_cycle_counts.hpp
---

$n$ 個の区別できる要素の順列のうち、巡回数がちょうど $k$、固定点がちょうど $f$ 個であるものを、小さい素数 $p$ を法として数える。固定点も長さ1の巡回として $k$ に含める。

## API

`SmallPrimePermutationCycleCounts(p)` を構築し、`count(n, k, f)` で $[0,p)$ の整数を返す。$2\le p\le5000$ の素数を指定する。合成数や範囲外の法には `invalid_argument` を送出する。

添字は非負の `long long` で、$10^{18}$ を含む。負の引数、$f>k$、$k>n$ など実現できない条件では0を返す。空順列について `count(0, 0, 0) = 1`。

## アルゴリズム

固定点の集合を選んだ後、残る $m=n-f$ 要素の順列が固定点を持たず、巡回数が $r=k-f$ であることを包除原理で課す。符号なし第1種スターリング数を $c$ とすると、答えは

$$\binom nf\sum_{j=0}^{\min(m,r)}(-1)^j\binom mj c(m-j,r-j)\pmod p.$$

Lucas の定理による `BinomialPrimeTable(p,p-1)` と、巨大添字を扱う `SmallPrimeStirlingNumbers` の第1種だけを組み合わせる。固定点以外の各巡回の長さが2以上なので、$2r>m$ は直ちに0と判定する。

## 時間計算量

構築 $O(p^2)$。照会 $O((k-f+1)\log_p(n+1))$。$n$ が巨大でも $k-f\le128$ など残る巡回数が小さい場合に適する。

## 空間計算量

$O(p^2)$。第1種表は `uint16_t` の三角配列で、$p=4999$ では約25 MB。

## 注意点

巡回数から固定点数を引いた値に比例する照会である。一般の大きな $k-f$ を定数時間で処理する API ではない。GCC 13、C++20 で利用できる。
