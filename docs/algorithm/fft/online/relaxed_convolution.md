---
title: Relaxed Convolution (逐次確定畳み込み)
documentation_of: ../../../../src/algorithm/fft/online/relaxed_convolution.hpp
---

二つの係数列を同じ次数から順に確定させながら、その時点の畳み込み係数を返す。
二進blockごとのNTT結果を再利用する。

## API

- `RelaxedConvolution<MOD>(capacity)`: 追加可能な係数対数を指定して構築する。
- `append(left_value, right_value)`: 両列の次の係数を追加し、追加後の次数
  $q$ に対する $\sum_{i=0}^{q}a_i b_{q-i}$ を返す。
- `size()`: 追加済み係数対数を返す。
- `capacity()`: 構築時の上限を返す。

## API別の時間計算量・空間計算量

`append` を合計 $N$ 回呼ぶと $O(N\log^2 N)$ 時間、$O(N)$ 追加領域。
1回あたりの償却時間は $O(\log^2 N)$。

## 注意点

`capacity` 回を超えて追加すると `std::length_error`。各NTT長が `MOD - 1`
を割り切る必要があり、満たさない場合は既存NTTの例外を送出する。
