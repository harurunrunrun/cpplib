---
title: Bluestein FFT (Bluestein高速Fourier変換)
documentation_of: ../../../../src/algorithm/fft/transform/bluestein_fft.hpp
---

任意長の複素離散 Fourier 変換を、chirp 列との畳み込みに帰着して計算する。

## API

```cpp
void bluestein_fourier_transform(std::vector<std::complex<double>>& values, bool inverse = false)
```

`values` を任意長の DFT の結果で上書きする。`inverse == true` なら逆変換を行い、最後に要素数で割る。

## 計算量

要素数を $N$ とすると、$O(N \log N)$ 時間、$O(N)$ の追加領域を使う。長さが 2 の冪なら `fast_fourier_transform` にそのまま委譲する。

## 注意点

- 符号の規約は `fast_fourier_transform` と同じで、順変換は $X_k = \sum_j x_j \exp(+2\pi i jk/N)$、逆変換は $x_j = \frac{1}{N} \sum_k X_k \exp(-2\pi i jk/N)$ である。
- 浮動小数点の丸め誤差を含む。chirp の角度は `long double` で計算する。
- 空の `std::vector` に対しては何もしない。
- 2 の冪でない長さが $2^{30} + 1$（`INT_MAX / 2 + 1`）を超えると `std::length_error` を送出する。
