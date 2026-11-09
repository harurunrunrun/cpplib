---
title: Small Prime Permutation Cycle Counts (巡回数と固定点数を指定した順列の個数)
documentation_of: ../../../../src/algorithm/math/combinatorics/small_prime_permutation_cycle_counts.hpp
---

$n$ 個の区別できる要素の順列のうち、巡回数がちょうど $k$、固定点がちょうど $f$ 個であるものの個数を、小さい素数 $p$ を法として数える。固定点も長さ 1 の巡回として $k$ に含める。

## API

```cpp
explicit SmallPrimePermutationCycleCounts(int prime);
int count(long long n, long long k, long long fixed) const;
```

`SmallPrimePermutationCycleCounts(p)` を構築し、`count(n, k, f)` で $[0, p)$ の整数を得る。$p$ には $2 \le p \le 5000$ の素数を指定する。合成数や範囲外の法では `invalid_argument` を送出する。

引数は非負の `long long` で、$10^{18}$ 程度の値も扱える。負の引数、$f > k$、$k > n$ など実現できない条件では $0$ を返す。空の順列について `count(0, 0, 0) = 1` である。

## アルゴリズム

固定点の集合を選んだ後、残る $m = n - f$ 要素の順列が固定点を持たず、巡回数が $r = k - f$ であるという条件を包除原理で課す。符号なし第一種 Stirling 数を $c$ とすると、答えは

$$
\binom nf\sum_{j=0}^{\min(m,r)}(-1)^j\binom mj c(m-j,r-j)\pmod p
$$

である。Lucas の定理による二項係数と、巨大な添字を扱う `SmallPrimeStirlingNumbers` の第一種のみを組み合わせる。固定点以外の巡回の長さはすべて 2 以上なので、$2r > m$ なら（および $m > 0$ かつ $r = 0$ なら）直ちに $0$ を返す。

## 時間計算量

構築 $O(p^2)$、クエリ $O((k-f+1) \log_p (n+1))$。$n$ が巨大でも、$k - f \le 128$ のように固定点以外の巡回数が小さい場合に適する。

## 空間計算量

$O(p^2)$。第一種の表は `uint16_t` の三角配列で、$p = 4999$ では約 25 MB になる。

## 注意点

クエリの計算量は巡回数から固定点数を引いた値に比例する。一般の大きな $k - f$ を高速に処理する API ではない。
