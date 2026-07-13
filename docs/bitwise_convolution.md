---
title: Bitwise Convolution (ビット演算畳み込み)
documentation_of: ../src/algorithm/fft/bitwise_convolution.hpp
---

bit 演算に対する畳み込み。

入力長は同じ 2 冪。

## 関数

- `or_zeta_transform(a)`, `or_mobius_transform(a)`
    - OR ゼータ変換と逆変換。
- `and_zeta_transform(a)`, `and_mobius_transform(a)`
    - AND ゼータ変換と逆変換。
- `walsh_hadamard_transform(a, inverse = false)`
    - XOR 用の Walsh-Hadamard 変換。
- `or_convolution(a, b)`
    - `i | j = k` となる積和。
- `and_convolution(a, b)`
    - `i & j = k` となる積和。
- `xor_convolution(a, b)`
    - `i ^ j = k` となる積和。
- `xnor_convolution(a, b)`
    - `~(i ^ j) = k` となる積和。ただし反転は入力長に対応する bit 幅で行う。

## 計算量の概要

- $O(n \log n)$

## API別の時間計算量・空間計算量

$N$ は入力列の長さ（2冪）とする。`T` の四則演算を $O(1)$ と数える。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `or_zeta_transform(a)` / `or_mobius_transform(a)` | $O(N\log N)$ | $O(1)$ |
| `and_zeta_transform(a)` / `and_mobius_transform(a)` | $O(N\log N)$ | $O(1)$ |
| `walsh_hadamard_transform(a,inverse)` | $O(N\log N)$ | $O(1)$ |
| `or_convolution(a,b)` | $O(N\log N)$ | $O(N)$ |
| `and_convolution(a,b)` | $O(N\log N)$ | $O(N)$ |
| `xor_convolution(a,b)` | $O(N\log N)$ | $O(N)$ |
| `xnor_convolution(a,b)` | $O(N\log N)$ | $O(N)$ |

各convolutionは長さ $N$ の列を列挙して返す。変換関数は入力vectorをin-placeで更新する。

## 注意点

法0、非可逆な除算、入力size、整数overflowの扱いは各APIの説明に従う。中間値は明記した内部拡張型または要素型で表現できなければならない。
