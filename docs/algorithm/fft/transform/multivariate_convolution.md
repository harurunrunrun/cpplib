---
title: Multivariate Convolution (多変数の切り詰め・周期畳み込み) [multivariate_convolution] [multivariate_convolution_cyclic]
documentation_of: ../../../../src/algorithm/fft/transform/multivariate_convolution.hpp
---

`multivariate_truncated_convolution<MOD>(shape,a,b)` は各変数の次数を `shape[i]` 未満へ切り詰めた積。混合基数の繰り上がりを色付き畳み込みで除く。`multivariate_cyclic_convolution(shape,a,b,prime)` は各変数を周期的に扱う。`multivariate_twisted_cyclic_convolution(shape,a,b,prime,twists)` では関係式 $x_i^{N_i}=\mathrm{twists}[i]^{N_i}$ で折り返す。いずれも最初の変数を最下位桁とする平坦配列。

## 時間計算量

次元数を $d$、全係数数を $L$ として切り詰め版は $O(dL\log L+d^2L)$、空間 $O(dL)$。周期版は $O(L\sum_i\log(N_i+1))$、空間 $O(L+\max_iN_i)$。小さい次元は直接DFT、それ以外はchirp変換と3素数CRTを使う。ねじれ版はさらに $O(dL+\sum_iN_i+d\log p)$。

## 注意点

各次元は2以上、空の次元列では定数同士の積。配列長は次元の積と一致させる。周期版の法は $10^9$ 以下の素数、各 $N_i$ は $p-1$ の約数。ねじれ係数は法で非零。3素数で整数畳み込みを復元できる大きさとNTTの長さに制限される。
