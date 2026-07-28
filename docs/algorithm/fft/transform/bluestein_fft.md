---
title: Bluestein FFT (Bluestein高速Fourier変換)
documentation_of: ../../../../src/algorithm/fft/transform/bluestein_fft.hpp
---

任意長の複素離散Fourier変換をchirp畳み込みへ変換して計算する。

## API

- `bluestein_fourier_transform(values, inverse = false)`: `values` を任意長の
  DFTで上書きする。`inverse == true` では逆変換し、要素数で除算する。

## API別の時間計算量・空間計算量

要素数を $N$ とすると $O(N\log N)$ 時間、$O(N)$ 追加領域。
2冪長では既存のradix-2 FFTへ直接委譲する。

## 注意点

符号規約は既存の `fast_fourier_transform` と同じで、順変換が
$\exp(+2\pi i jk/N)$、逆変換が $\exp(-2\pi i jk/N)/N$。
浮動小数点丸め誤差を含む。空vectorは何もしない。
