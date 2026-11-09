---
title: Multivariate Convolution (多変数の切り詰め・周期畳み込み) [multivariate_convolution] [multivariate_convolution_cyclic]
documentation_of: ../../../../src/algorithm/fft/transform/multivariate_convolution.hpp
---

多変数多項式の積を、各変数について切り詰める版・周期的に折り返す版・ねじって折り返す版の 3 種類について計算する。係数はいずれも、最初の変数を最下位の桁とする混合基数で平坦化した配列で与える。

```cpp
template<int MOD = 998244353>
std::vector<Modint<MOD>> multivariate_truncated_convolution(
    const std::vector<int>& shape, const std::vector<Modint<MOD>>& a, const std::vector<Modint<MOD>>& b);

std::vector<std::uint32_t> multivariate_cyclic_convolution(
    const std::vector<int>& shape, const std::vector<std::uint32_t>& a,
    const std::vector<std::uint32_t>& b, std::uint32_t prime);

std::vector<std::uint32_t> multivariate_twisted_cyclic_convolution(
    const std::vector<int>& shape, const std::vector<std::uint32_t>& a,
    const std::vector<std::uint32_t>& b, std::uint32_t prime, const std::vector<std::uint32_t>& twists);
```

- `multivariate_truncated_convolution` は、$i$ 番目の変数の次数を `shape[i]` 未満に切り詰めた積を返す。平坦化した添字での一変数畳み込みに生じる混合基数の繰り上がりを、色付き（ランク付き）畳み込みで取り除く。
- `multivariate_cyclic_convolution` は、各変数を周期的に扱う（$x_i^{N_i} = 1$ とする）積を法 `prime` で返す。ここで $N_i = \texttt{shape}[i]$ である。
- `multivariate_twisted_cyclic_convolution` は、関係式 $x_i^{N_i} = \texttt{twists}[i]^{N_i}$ で折り返した積を法 `prime` で返す。

## 計算量

次元数を $d$、全係数の個数を $L$ とする。

- 切り詰め版: 時間 $O(dL\log L + d^2 L)$、空間 $O(dL)$。
- 周期版: 各軸について、$N_i \le 128$ なら直接 DFT（$O(L N_i)$）、それより大きければ chirp 変換と 3 素数 CRT による畳み込み（$O(L \log N_i)$）を行う。全体の時間は $O(L \sum_i c_i)$（$c_i$ は $N_i \le 128$ なら $N_i$、そうでなければ $\log N_i$）、空間は $O(L + \max_i N_i)$ である。
- ねじれ版: 周期版に加えて $O(dL + \sum_i N_i + d \log p)$ である。

## 注意点

- 各次元の大きさは $2$ 以上でなければならない。空の次元列なら定数どうしの積になる。
- 配列の長さは次元の積と一致させる。満たさない場合や、次元の積が `std::size_t` に収まらない場合は `std::invalid_argument` を送出する。
- 周期版・ねじれ版の法 `prime` は $10^9$ 以下の素数で、各 $N_i$ は $\texttt{prime} - 1$ の約数でなければならない。違反すると `std::invalid_argument` を送出する。
- ねじれ係数は法 `prime` で非零でなければならず、$0$ なら `std::invalid_argument` を送出する。
- 切り詰め版の変換長は `MOD` の NTT 長の上限に、周期版の chirp 畳み込みは 3 素数で整数畳み込みを復元できる大きさと NTT 長に制限される。
