---
title: FFT (高速フーリエ変換)
documentation_of: ../src/algorithm/fft/fft.hpp
---

複素数 FFT。

## 関数

- `fast_fourier_transform(a, inverse = false)`
    - FFT と逆 FFT。入力長は 2 冪。
- `convolution_fft(a, b)`
    - `double` 列の畳み込み。
- `convolution_ll(a, b)`
    - `long long` 列の畳み込み。結果は丸める。

## 計算量

- $O(n \log n)$

## API別計算量

$A=|a|$, $B=|b|$、$Z$ を $A+B-1$ 以上の最小の2冪とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `fast_fourier_transform(a,inverse)` | $O(N\log N)$ | 追加 $O(1)$ |
| `convolution_fft(a,b)` | $O(Z\log Z)$ | $O(Z)$ |
| `convolution_ll(a,b)` | $O(Z\log Z)$ | $O(Z)$ |

畳み込みの戻り値列挙は $O(A+B)$ で、上表の時間に含まれる。`convolution_ll` はFFT結果の全要素を丸める $O(A+B)$ 処理も含む。
## API詳細と数値誤差

- `fast_fourier_transform(a,inverse)` は複素数vectorをin-placeで変換する。
  長さは正の2冪が必要で、違反時は例外。`inverse=true` は逆変換後に長さで割る。
- `convolution_fft(a,b)` は実数列の通常畳み込みを長さ $A+B-1$ で返す。
  どちらかが空なら空。結果には浮動小数点誤差が含まれる。
- `convolution_ll(a,b)` は `double` FFTの実部を `llround` して返す。
  係数や列長が大きく精度を失う入力では正確性を保証しない。
