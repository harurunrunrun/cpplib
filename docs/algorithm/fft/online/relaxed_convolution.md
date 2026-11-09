---
title: Relaxed Convolution (逐次確定畳み込み)
documentation_of: ../../../../src/algorithm/fft/online/relaxed_convolution.hpp
---

2 つの係数列 $a, b$ を同じ次数ずつ先頭から確定させながら、その時点で確定する畳み込みの係数を返す。二進ブロックごとの NTT 結果（先頭部分の変換）を再利用する。

## API

```cpp
template<int MOD>
class RelaxedConvolution;
```

- `explicit RelaxedConvolution(std::size_t capacity)`: 追加できる係数の組の個数の上限を指定して構築する。
- `Modint<MOD> append(Modint<MOD> left_value, Modint<MOD> right_value)`: 両方の列の次の係数 $a_q, b_q$ を追加し、$\sum_{i=0}^{q} a_i b_{q-i}$ を返す。ここで $q$ は追加した係数の次数（0-indexed）である。
- `std::size_t size() const`: 追加済みの係数の組の個数を返す。
- `std::size_t capacity() const`: 構築時に指定した上限を返す。

## 計算量

`append` を合計 $N$ 回呼ぶと $O(N \log^2 N)$ 時間であり、1 回あたりの償却時間は $O(\log^2 N)$ である。構築時に長さ `capacity` の配列を確保するため、空間計算量は $O(\texttt{capacity})$ である。

## 注意点

- `capacity` 回を超えて `append` すると `std::length_error` を送出する。
- 各 NTT 長が `MOD - 1` を割り切る必要があり、満たさない場合は `number_theoretic_transform` の例外がそのまま送出される。
