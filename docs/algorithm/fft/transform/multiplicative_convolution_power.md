---
title: Multiplicative Convolution Powers (乗法剰余ごとの列の重み付き数え上げ)
documentation_of: ../../../../src/algorithm/fft/transform/multiplicative_convolution_power.hpp
---

素数または2冪の長さの重み列 `a` と非負整数 `K` を受け取り、長さ `K` の順序付き列の積の剰余ごとに重みの積を合計する。
`prime_multiplicative_power(a,K)` は素数長、`power_two_multiplicative_power(a,K)` は2冪長に対応する。
指数は `uint64_t`。`K=0` は空積 `1 mod N` の重みだけが1となる。

## 時間計算量

二分累乗で乗法畳み込みを繰り返す。長さ `N` に対して時間 `O(N log N log(K+1))`、作業領域 `O(N)`。

## 注意点

係数は `Modint<MOD>`。既定法は998244353。2冪版は既存のNTTが扱える長さと法を使用する。素数版は任意法畳み込みの長さ上限に従う。
長さ1は2冪版で扱い、すべての列が剰余0に入る。空の配列は受け付けない。GCC 13 / C++20対応。
