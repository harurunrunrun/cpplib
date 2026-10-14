---
title: Blocked Large NTT Convolution (変換長の上限を超える畳み込み)
documentation_of: ../../../../src/algorithm/fft/transform/large_convolution_ntt.hpp
---

`large_convolution_ntt<MOD>(a,b,maximum_transform_size)` は、出力長が素数体の最大NTT長を超えても通常の畳み込みを計算する。第3引数の既定値は `MOD-1` を割る最大2冪で、より小さい2冪も指定できる。`MOD` の条件は `montgomery_ntt.hpp` と同じ。

## 時間計算量

長さ $L/2$ のブロックへ分け、各ブロックを長さ $L$ で一度変換する。同じブロック番号和を持つ積を周波数領域で足し、対角線ごとに1回逆変換する。左・右のブロック数を $A,B$ として、時間 $O((A+B)L\log L+ABL)$、領域 $O((A+B)L+N+M)$。すべての対角線の配列を同時保持しない。

類題: 各入力が最大 $2^{24}$ の列を法998244353で畳み込む。小さい変換上限を指定すれば、同じ処理のブロック境界を小規模な愚直解と比較できる。

## 注意点

GCC 13 / C++20。法・可逆性・添字範囲は上記の各APIの条件を満たす必要がある。
