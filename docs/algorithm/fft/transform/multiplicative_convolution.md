---
title: Multiplicative Convolution (乗法を添字とする畳み込み)
documentation_of: ../../../../src/algorithm/fft/transform/multiplicative_convolution.hpp
---

`prime_multiplicative_convolution<MOD>(a,b)` は、配列長の素数 $p$ に対して $c_k=\sum_{ij\equiv k\pmod p}a_i b_j$ を返す。原始根の指数へ置き換えて循環畳み込みを行う。`power_two_multiplicative_convolution<MOD>(a,b)` は長さ $2^n$ に対応し、2進付値と単数群の符号・5の冪へ分解して変換する。

## 時間計算量

配列長を $L$ として $O(L\log L)$、空間 $O(L)$。素数版には原始根の探索も含む。

## 注意点

両配列の長さは同じでなければならない。2冪版の係数法は奇素数で、必要な2冪長NTTが存在すること。長さ1も扱う。係数型・変換長には `convolution.hpp` と同じ制約がある。
