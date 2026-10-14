---
title: Montgomery NTT Convolution (Montgomery NTT による畳み込み)
documentation_of: ../../../../src/algorithm/fft/transform/montgomery_ntt.hpp
---

`fast_convolution_ntt<MOD>(a,b)` は `vector<Modint<MOD>>` の通常の畳み込みを返す。空列との積は空列。`MOD` は $3\le MOD<2^{30}$ の奇素数で、使用する2冪変換長が `MOD-1` を割ることが必要。

## 時間計算量

内部表現を Montgomery 形式に変換し、剰余を `[0,2*MOD)` に保った radix-4 NTT を行う。時間 $O((N+M)\log(N+M))$、領域 $O(N+M)$。短い列との積では直接計算を選ぶ。

類題: 多項式を二乗した後で指定区間の係数を取り出す。通常の Modint との変換はAPI内部で行う。

## 注意点

GCC 13 / C++20。法・可逆性・添字範囲は上記の各APIの条件を満たす必要がある。
