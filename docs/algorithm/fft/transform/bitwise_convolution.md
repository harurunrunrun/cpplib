---
title: Bitwise Convolution (ビット演算畳み込み) [bitwise_and_convolution] [bitwise_xor_convolution]
documentation_of: ../../../../src/algorithm/fft/transform/bitwise_convolution.hpp
---

添字のビット演算（OR・AND・XOR・XNOR）に関する畳み込みと、そのための変換をまとめたものである。入力列の長さは 2 の冪でなければならず、畳み込みでは 2 つの入力の長さが等しくなければならない。満たさない場合は `std::runtime_error` を送出する。

## 関数

- `or_zeta_transform(a)`, `or_mobius_transform(a)`
    - OR に関するゼータ変換（部分集合和）とその逆変換（メビウス変換）。
- `and_zeta_transform(a)`, `and_mobius_transform(a)`
    - AND に関するゼータ変換（上位集合和）とその逆変換。
- `walsh_hadamard_transform(a, inverse = false)`
    - XOR 畳み込み用の Walsh-Hadamard 変換。`inverse == true` なら最後に各要素を `T(n)` で割る。
- `or_convolution(a, b)`
    - $c_k = \sum_{i \mathbin{\mathrm{or}} j = k} a_i b_j$ を返す。
- `and_convolution(a, b)`
    - $c_k = \sum_{i \mathbin{\mathrm{and}} j = k} a_i b_j$ を返す。
- `xor_convolution(a, b)`
    - $c_k = \sum_{i \oplus j = k} a_i b_j$ を返す。
- `xnor_convolution(a, b)`
    - $\lnot (i \oplus j) = k$ となる組の積和を返す。ただし否定は入力長に対応するビット幅で行う。

変換関数は引数の `std::vector<T>` をインプレースで更新する。畳み込み関数は長さ $N$ の新しい列を返す。

## 計算量

$N$ を入力列の長さ（2 の冪）とし、`T` の四則演算を $O(1)$ とみなす。

| API | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| `or_zeta_transform(a)` / `or_mobius_transform(a)` | $O(N\log N)$ | $O(1)$ |
| `and_zeta_transform(a)` / `and_mobius_transform(a)` | $O(N\log N)$ | $O(1)$ |
| `walsh_hadamard_transform(a, inverse)` | $O(N\log N)$ | $O(1)$ |
| `or_convolution(a, b)` | $O(N\log N)$ | $O(N)$ |
| `and_convolution(a, b)` | $O(N\log N)$ | $O(N)$ |
| `xor_convolution(a, b)` | $O(N\log N)$ | $O(N)$ |
| `xnor_convolution(a, b)` | $O(N\log N)$ | $O(N)$ |

## 注意点

- `walsh_hadamard_transform` の逆変換と `xor_convolution`・`xnor_convolution` は `T(n)` による除算を行うため、`T` で $N$ が可逆である必要がある（`Modint` の法が $2$ の場合などは不可）。
