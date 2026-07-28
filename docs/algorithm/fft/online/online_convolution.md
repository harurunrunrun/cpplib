---
title: Online Convolution (固定核オンライン畳み込み)
documentation_of: ../../../../src/algorithm/fft/online/online_convolution.hpp
---

既知の固定kernelに対し、入力列だけを順に追加して畳み込みprefixを返す。

## API

- `OnlineConvolution<MOD>(kernel)`: 固定する右辺係数列を指定する。
- `append(value)`: 入力列の次の係数を追加し、現在次数の畳み込み係数を返す。
- `size()`: 追加済み入力数を返す。
- `kernel()`: 固定kernelへのconst参照を返す。

## API別の時間計算量・空間計算量

入力数を $N$、kernel長を $K$ とすると、全追加は
$O(N\log^2(\max(2,K)))$ 時間、$O(N+K)$ 追加領域。
小blockは直接積を使い、長いblockだけNTTする。

## 注意点

空kernelも許可し、その場合は常に0を返す。利用するNTT長が `MOD - 1` を
割り切らない場合は既存NTTの例外を送出する。
