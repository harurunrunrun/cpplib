---
title: FFT (高速フーリエ変換)
documentation_of: ../../../../src/algorithm/fft/transform/fft.hpp
---

`std::complex<double>` による FFT と、それを用いた実数列・整数列の畳み込みである。

## 関数

- `void fast_fourier_transform(std::vector<std::complex<double>>& a, bool inverse = false)`
    - FFT とその逆変換。入力長は 2 の冪でなければならない。
- `std::vector<double> convolution_fft(const std::vector<double>& a, const std::vector<double>& b)`
    - `double` 列の畳み込み。
- `std::vector<long long> convolution_ll(const std::vector<long long>& a, const std::vector<long long>& b)`
    - `long long` 列の畳み込み。結果は最も近い整数に丸める。

## 計算量

$A$ を `a` の長さ、$B$ を `b` の長さ、$Z$ を $A + B - 1$ 以上の最小の 2 の冪とする。`fast_fourier_transform` では $N$ を入力長とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `fast_fourier_transform(a, inverse)` | $O(N\log N)$ | 追加 $O(1)$ |
| `convolution_fft(a, b)` | $O(Z\log Z)$ | $O(Z)$ |
| `convolution_ll(a, b)` | $O(Z\log Z)$ | $O(Z)$ |

## 注意点

- `fast_fourier_transform(a, inverse)` は複素数列をインプレースで変換する。順変換は $X_k = \sum_j a_j \exp(+2\pi i jk/N)$ の規約である。長さは正の 2 の冪でなければならず、違反すると `std::runtime_error` を送出する。`inverse == true` なら逆変換の後に長さで割る。
- `convolution_fft(a, b)` は実数列の通常の畳み込みを長さ $A + B - 1$ で返す。どちらかが空なら空列を返す。結果には浮動小数点誤差が含まれる。
- `convolution_ll(a, b)` は `double` の FFT の結果の実部を `std::llround` で丸めて返す。係数や列長が大きく精度が失われる入力では正確な値を保証しない。
