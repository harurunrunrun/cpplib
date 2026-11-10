---
title: Online Convolution (固定核オンライン畳み込み)
documentation_of: ../../../../src/algorithm/fft/online/online_convolution.hpp
---

あらかじめ与えた固定の核（kernel）$g$ に対し、入力列 $f$ の係数を 1 つずつ追加しながら、畳み込み $f g$ の係数を先頭から順に返す。

## API

```cpp
template<int MOD>
class OnlineConvolution;
```

- `explicit OnlineConvolution(std::vector<Modint<MOD>> kernel)`: 固定する核 $g$ の係数列を指定して構築する。
- `Modint<MOD> append(Modint<MOD> value)`: 入力列の次の係数 $f_q$ を追加し、$\sum_{i=0}^{q} f_i g_{q-i}$ を返す。ここで $q$ は追加した係数の次数（0-indexed）である。
- `std::size_t size() const`: 追加済みの係数の個数を返す。
- `const std::vector<Modint<MOD>>& kernel() const`: 核への const 参照を返す。

## 時間計算量・空間計算量

追加する係数の個数を $N$、核の長さを $K$ とすると、`append` を $N$ 回呼ぶ合計で $O(N \log^2 (\max(2, K)))$ 時間、$O(N + K)$ の追加領域を使う。

二進ブロックごとに、ブロック長 $16$ 以下は直接積を計算し、それより長いブロックだけ NTT で計算する。核の NTT 結果はブロック長ごとにキャッシュする。

## 注意点

- 空の核も許し、その場合 `append` は常に $0$ を返す。
- 使用する NTT 長が `MOD - 1` を割り切らない場合は、`number_theoretic_transform` の例外がそのまま送出される。
