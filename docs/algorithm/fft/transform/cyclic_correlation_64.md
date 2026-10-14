---
title: Cyclic Correlation over 64 Bit Rings (64 bit 環・体の循環相関)
documentation_of: ../../../../src/algorithm/fft/transform/cyclic_correlation_64.hpp
---

同じ長さ $N$ の列について `C[k]=sum_i A[i]*B[(i+k)%N]` を返す。
`cyclic_correlation_mod_2_64(a,b)` は法 $2^{64}$ の環、`cyclic_correlation_gf2_64(a,b)` は $GF(2^{64})$ の体で計算する。
入出力は `vector<uint64_t>`。体は不可約多項式 $x^{64}+x^4+x^3+x+1$ の多項式基底を使い、和はXORとなる。

## 時間計算量

最初の列を循環的に反転して既存の高速畳み込みへ渡し、次数 $N$ 以上を畳み戻す。
通常の畳み込み1回と $O(N)$ の追加処理で、時間は既存の各畳み込みと同じオーダー、追加領域 $O(N)$。

## 注意点

GCC 13 / C++20。列の長さが異なる場合は例外。空列同士の相関は空列。
既存の64 bit畳み込みと同じ長さ上限 $N<=524288$ が適用される。
