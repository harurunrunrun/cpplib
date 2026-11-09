---
title: Blocked Large NTT Convolution (変換長の上限を超える畳み込み)
documentation_of: ../../../../src/algorithm/fft/transform/large_convolution_ntt.hpp
---

```cpp
template<int MOD>
std::vector<Modint<MOD>> large_convolution_ntt(
    const std::vector<Modint<MOD>>& a,
    const std::vector<Modint<MOD>>& b,
    std::size_t maximum_transform_size = (MOD - 1 を割り切る最大の 2 の冪)
);
```

出力長が素数体での最大 NTT 長を超える場合でも、通常の畳み込みを計算する。第 3 引数 `maximum_transform_size` には、デフォルト値より小さい 2 の冪も指定できる。`MOD` の条件は `montgomery_ntt.hpp` と同じである。

出力長が `maximum_transform_size` 以下なら `fast_convolution_ntt` をそのまま呼ぶ。

## 計算量

$L = \texttt{maximum_transform_size}$ とする。入力を長さ $L/2$ のブロックに分け、各ブロックを長さ $L$ で 1 回だけ変換する。ブロック番号の和が等しい積を周波数領域で足し合わせ、その和（対角線）ごとに 1 回逆変換する。

左右のブロック数を $A, B$、入力長を $N, M$ として、時間 $O((A+B) L \log L + ABL)$、空間 $O((A+B) L + N + M)$ である。対角線ごとの配列は 1 本ずつ使い回し、すべてを同時には保持しない。

例えば、各入力が最大 $2^{24}$ 項の列を法 $998244353$（最大 NTT 長 $2^{23}$）で畳み込む問題に使える。小さい変換上限を指定すれば、同じ処理のブロック境界を小規模な愚直解と比較してテストできる。

## 注意点

- GCC 13 / C++20 を想定する。
- `maximum_transform_size` が 2 の冪でない、または `MOD - 1` を割り切らない場合は `std::invalid_argument` を送出する。
- どちらかの入力が空なら空列を返す。
